
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <regex>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include <Common/ConfigLoader.h>
#include <Common/GetTime.h>
#include <Common/Text.h>
#include <Common/PrintRange.h>

#include <Garnet/Affinity.h>
#include <Garnet/Drivers.h>
#include <Garnet/Garnet.h>
#include <Garnet/Dictionary.h>
#include <Garnet/IVGraphProcessor.h>

#include <PicturePerfectEngine/PicturePerfectEngine.h>
#include <PicturePerfectEngine/Archive.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

using namespace PicturePerfect;

namespace {

void  loadTestTrees(std::vector<Garnet::Tree>& trees)
{
	trees.push_back(Garnet::Tree("(Sink0(Add(Erode3x3(Gray(Src0)))(Gray(Src1))))"));
	trees.push_back(Garnet::Tree("(Sink1(Gray(Src1))"));
}

void  loadTestIVGraph(Garnet::IVGraph& iv)
{
	Garnet::IVGraph::vertex_descriptor v[] = {
		add_vertex(iv),
		add_vertex(iv),
		add_vertex(iv),
		add_vertex(iv),
		add_vertex(iv),
		add_vertex(iv),
	};

	iv[v[0]].label = "Sink0";
	iv[v[1]].label = "Add";
	iv[v[2]].label = "Erode3x3";
	iv[v[3]].label = "Src0";
	iv[v[4]].label = "Sink1";
	iv[v[5]].label = "Src1";

	add_edge(v[0], v[1], iv);
	add_edge(v[1], v[2], iv);
	add_edge(v[1], v[5], iv);
	add_edge(v[2], v[3], iv);
	add_edge(v[4], v[5], iv);
}

void  loadTestPPEGraph(Garnet::PPEGraph& pg)
{
	Garnet::PPEGraph::vertex_descriptor v[] = {
		add_vertex(pg), add_vertex(pg),
		add_vertex(pg), add_vertex(pg),
		add_vertex(pg), add_vertex(pg),
		add_vertex(pg), add_vertex(pg),
		add_vertex(pg), add_vertex(pg),

		add_vertex(pg), add_vertex(pg),
		add_vertex(pg), add_vertex(pg),
		add_vertex(pg), add_vertex(pg),
		add_vertex(pg), add_vertex(pg),
		add_vertex(pg), add_vertex(pg),

		add_vertex(pg), add_vertex(pg),
	};

	// Sink0
	pg[v[0]].label = "Move";
	pg[v[0]].type  = _RI1B;
	pg[v[0]].color = 0;
	pg[v[1]].label = "Move";
	pg[v[1]].type  = _RV1D;
	pg[v[1]].color = 0;

	// Sink1
	pg[v[2]].label = "Move";
	pg[v[2]].type  = _RI1B;
	pg[v[2]].color = 1;
	pg[v[3]].label = "Move";
	pg[v[3]].type  = _RV1D;
	pg[v[3]].color = 1;

	// Add
	pg[v[4]].label = "Add";
	pg[v[4]].type  = _I1B;
	pg[v[5]].label = "Move";
	pg[v[5]].type  = _V1D;

	// Subtract
	pg[v[6]].label = "Subtract";
	pg[v[6]].type  = _I1B;
	pg[v[7]].label = "Move";
	pg[v[7]].type  = _V1D;

	// Diff
	pg[v[8]].label = "Diff";
	pg[v[8]].type  = _I1B;
	pg[v[9]].label = "Move";
	pg[v[9]].type  = _V1D;

	// Gray
	pg[v[10]].label = "BGRGray";
	pg[v[10]].type  = _I1B;
	pg[v[11]].label = "Move";
	pg[v[11]].type  = _V1D;

	// Red
	pg[v[12]].label = "Split3rd";
	pg[v[12]].type  = _I1B;
	pg[v[13]].label = "Move";
	pg[v[13]].type  = _V1D;

	// Green
	pg[v[14]].label = "Split2nd";
	pg[v[14]].type  = _I1B;
	pg[v[15]].label = "Move";
	pg[v[15]].type  = _V1D;

	// Blue
	pg[v[16]].label = "Split1st";
	pg[v[16]].type  = _I1B;
	pg[v[17]].label = "Move";
	pg[v[17]].type  = _V1D;
	
	// Src0
	pg[v[18]].label = "Move";
	pg[v[18]].type  = _SI3B;
	pg[v[18]].color = 0;
	pg[v[19]].label = "Move";
	pg[v[19]].type  = _SV1D;
	pg[v[19]].color = 0;

	// Src1
	pg[v[20]].label = "Move";
	pg[v[20]].type  = _SI3B;
	pg[v[20]].color = 1;
	pg[v[21]].label = "Move";
	pg[v[21]].type  = _SV1D;
	pg[v[21]].color = 1;

	add_edge(v[0], v[4], pg);
	add_edge(v[1], v[19], pg);

	add_edge(v[2], v[8], pg);
	add_edge(v[3], v[21], pg);

	add_edge(v[4], v[6], pg);
	add_edge(v[4], v[8], pg);

	add_edge(v[6], v[10], pg);
	add_edge(v[6], v[12], pg);

	add_edge(v[8], v[14], pg);
	add_edge(v[8], v[16], pg);

	add_edge(v[10], v[18], pg);

	add_edge(v[12], v[20], pg);

	add_edge(v[14], v[20], pg);

	add_edge(v[16], v[18], pg);
}

}//end of local namespace

void Garnet::Drivers::executeModules(StringMap& args, std::map<std::string, bool>& activeModules)
{
	std::vector<Tree> trees;
	IVGraph iv;
	PPEGraph pg;
	std::stringstream ppscript;
	PicturePerfect::PicturePerfectEnginePtr ppe;
	PicturePerfect::ExecutablePtr executable;
	PicturePerfect::ArchivePtr archive;
	std::map<std::string, std::stringstream> sections;
	std::vector<double> fitness;
	StopWatch watches[5];

	// Banner
	std::cout << "Module test." << std::endl;

	//-----------------------------------------------------
    //  0. Set up.
	//-----------------------------------------------------
	std::string infilename;
	std::string outfilename;
	if ( args.find("_Input") != args.end() ) {
		infilename = args["_Input"];
	}
	else if ( args.find("_Project") != args.end() ) {
		infilename = args["_Project"];
	}
	if ( args.find("_Output") != args.end() ) {
		outfilename = args["_Output"];
	}

	//-----------------------------------------------------
    //  1. IV Tree to IV Graph
	//-----------------------------------------------------
	if ( activeModules["tree2graph"] ) {
		// 1.1 Input source.
		std::stringstream ss;
		if ( !infilename.empty() ) {
			std::ifstream fin(infilename.c_str());
			//pg.load(fin);
		}
		else {
			loadTestTrees(trees);
		}

		// 1.2 Show source information
		std::cout << boost::format("IVTrees: %u trees.\n") 
				% trees.size();
		for (unsigned i = 0; i < trees.size(); i++) {
			std::cout << boost::format("[%u] %s\n") 
					% i
					% trees[i].getSExpression();
		}
		std::cout << "----- ----- ----- -----" << std::endl;

		// 1.3 Execute.
		watches[0].reset();
#if 1
		//iv = Garnet::IVGraphProcessor().convertToIVGraph(trees);
#else
		Garnet::convertIVTreesToIVGraph(trees, iv);
#endif
		watches[0].stop();

		// 1.4 Output result
		if ( !activeModules["iv2ppe"] ) {
			if ( !outfilename.empty() ) {
				std::ofstream outs(outfilename.c_str());
				//outs << ss.str() << std::endl;
				std::cout << boost::format("Output: %s") 
						% outfilename;
				std::cout << std::endl;
			}
		}
	}

	//-----------------------------------------------------
    //  2. IV Graph to PPE Graph
	//-----------------------------------------------------
	if ( activeModules["iv2ppe"] ) {
		// 2.1 Input source.
		if ( !activeModules["tree2graph"] ) {
			if ( !infilename.empty() ) {
				std::ifstream fin(infilename.c_str());
				//iv.load(fin);
			}
			else {
				loadTestIVGraph(iv);
			}
		}
	}

	if ( activeModules["tree2graph"] || activeModules["iv2ppe"] ) {
		// 2.2 Show source information.
		std::cout << boost::format("IVGraph: %u vertices, %u edges\n")
				% boost::num_vertices(iv)
				% boost::num_edges(iv);
		print_graph(iv, get(&IVGraph::vertex_property_type::label, iv));
		std::cout << "----- ----- ----- -----" << std::endl;
	}

	if ( activeModules["iv2ppe"] ) {
		// 2.3 Execute.
		watches[1].reset();
#if 1
		//pg = Garnet::IVGraphProcessor().convertToPPEGraph(iv);
#else
		Garnet::convertIVGraphToPPEGraph(iv, pg);
#endif
		watches[1].stop();

		// 2.4 Output result.
		if ( !activeModules["genscript"] ) {
			if ( !outfilename.empty() ) {
				std::ofstream outs(outfilename.c_str());
				//outs << ss.str() << std::endl;
			}
		}
	}

	//-----------------------------------------------------
    //  3. PPE Graph to PPE Script
	//-----------------------------------------------------
	if ( activeModules["genscript"] ) {
		// 3.1 Input source.
		if ( !activeModules["iv2ppe"] ) {
			if ( !infilename.empty() ) {
				std::ifstream fin(infilename.c_str());
				//pg.load(fin);
			}
			else {
				loadTestPPEGraph(pg);
			}
		}
	}

	if ( activeModules["iv2ppe"] || activeModules["genscript"] ) {
		print_graph(pg, get(&PPEGraph::vertex_property_type::label, pg));
		std::cout << "----- ----- ----- -----" << std::endl;
	}

	if ( activeModules["genscript"] ) {
		// 3. Execute.
		watches[2].reset();
		ppscript << "Begin Execution\n";
#if 1
		//ppscript.str(PPEGraphProcessor().generateScript(pg));
#else
		Garnet::generateScript(pg, ppscript);
#endif
		ppscript << "End Execution\n";
		watches[2].stop();

		// 4. Output result.
		if ( !activeModules["runscript"] ) {
			if ( !outfilename.empty() ) {
				std::ofstream outs(outfilename.c_str());
				outs << ppscript.str() << std::endl;
			}
			else {
				std::cout << ppscript.str() << std::endl;
				std::cout << "----- ----- ----- -----" << std::endl;
			}
		}
	}

	//-----------------------------------------------------
    //  4. Execute PPE Script
	//-----------------------------------------------------
	if ( activeModules["runscript"] ) {
		// 4.1 Input source.
		if ( !activeModules["genscript"] ) {
			// 4.1.1 Read sections from the script.
			if ( !infilename.empty() ) {
				std::ifstream fin(infilename.c_str());
				ConfigLoader::readSections(fin, sections);
			}
		}
		else {
			ConfigLoader::readSections(ppscript, sections);
		}
		sections["ArchiveSave"].str(sections["Archive"].str());
	}

	if ( activeModules["genscript"] || activeModules["runscript"] ) {
		for (auto it = sections.begin(); it != sections.end(); it++) {
			std::cout << boost::format("[%s] Section %s\n") % __FUNCTION__ % it->first;
			std::cout << it->second.str();
		}
	}

	if ( activeModules["runscript"] ) {
		// 4.2 Execute.
		unsigned width  = args.find("_SizeX") == args.end() ? 256 : stoi(args["_SizeX"]);
		unsigned height = args.find("_SizeY") == args.end() ? 256 : stoi(args["_SizeY"]);

		// 4.2.1 Save the directory path where the script is.
		if ( !infilename.empty() ) {
			std::wstring basedir = Text::toWideChar(infilename);
			std::wstring::size_type pos = basedir.find_last_of(L"\\/");
			if ( pos != std::wstring::npos ) {
				basedir = basedir.substr(0, pos);
				PicturePerfect::PicturePerfectEngine::setBaseDirectory(basedir);
			}
		}

		// 4.2.2 Configure PPE.
		ppe = PicturePerfect::PicturePerfectEngine::createInstance();
		ppe->configure(sections[""]);

		// 4.2.3 Load initial contents into Archive.
		archive = PicturePerfect::Archive::createInstance(cv::Size(width, height));
		archive->load(sections["Archive"]);

		// 4.2.4 Compile
		watches[3].reset();
		executable = ppe->compile(sections["Execution"]);
		watches[3].stop();

		// 4.2.5 Execute.
		watches[4].reset();
		ppe->execute(executable, archive);
		watches[4].stop();

		// 4.2.3 Gather raw fitnesses.
		uint32_t index = 0;
		while (archive->exists(ADDR_RV1D + index)) {
			fitness.push_back(archive->getValue(ADDR_RV1D + index));
			index++;
		}

		// 4.3 Output result.
		{
			if ( sections.find("ArchiveSave") != sections.end() ) {
				archive->save(sections["ArchiveSave"]);
			}
		}
	}

	if ( activeModules["runscript"] ) {
		for (size_t i = 0; i < fitness.size(); i++) {
			std::cout << boost::format("Fitness[%d]=%f\n") % i % fitness[i];
		}
		std::cout << "----- ----- ----- -----" << std::endl;
	}

	// Show information.
	if ( activeModules.size() > 0 ) { // && isTrace() ) {
		if ( activeModules["tree2graph"] ) std::cout << boost::format("[%s] Trees to IVGraph    %.6fs\n") % __FUNCTION__ % watches[0].get();
		if ( activeModules["iv2ppe"] )     std::cout << boost::format("[%s] IVGraph to PPEGraph %.6fs\n") % __FUNCTION__ % watches[1].get();
		if ( activeModules["genscript"] )  std::cout << boost::format("[%s] PPEGraph to Script  %.6fs\n") % __FUNCTION__ % watches[2].get();
		if ( activeModules["runscript"] )  std::cout << boost::format("[%s]   # of Ops          %u\n") % __FUNCTION__ % executable->getNumberOfOperators();
		if ( activeModules["runscript"] )  std::cout << boost::format("[%s]   # of Data         %u\n") % __FUNCTION__ % executable->getNumberOfData();
		if ( activeModules["runscript"] )  std::cout << boost::format("[%s] Compilation         %.6fs\n") % __FUNCTION__ % watches[3].get();
		if ( activeModules["runscript"] )  std::cout << boost::format("[%s] Execution           %.6fs\n") % __FUNCTION__ % watches[4].get();
		std::cout << std::flush;
	}
}

/**
 *
 */
void Garnet::Drivers::executeTree(StringMap& args)
{
	std::vector<std::string> treeExps;
	std::vector<Tree> trees;
	IVGraph iv;
	PPEGraph pg;
	std::stringstream ppscript;
	PicturePerfect::PicturePerfectEnginePtr ppe;
	PicturePerfect::ArchivePtr archive;
	std::map<std::string, std::stringstream> sections;
	std::vector<double> fitness;
	StopWatch watches[5];
	//ConfigLoader::StringMap aliases;
	std::map<std::string, Garnet::Tree> aliases;

	Garnet::Configuration conf;

	// 0. Load arguments
	if ( args.find("_Project") != args.end() ) {
		// Load from configuration file.
		std::string prj = args["_Project"];
	    std::ifstream fin;
        fin.open(prj.c_str());
	    if ( fin.fail() ) {
		    throw std::string("[" __FUNCTION__ "] E2: Failed to open a project file '") + prj + "'.";
	    }
        conf.load(fin, 0);
	}

	if ( args.find("_SizeX") != args.end() ) {
		conf.imageWidth = strtol(args["_SizeX"].c_str(), 0, 0);
	}
	if ( args.find("_SizeY") != args.end() ) {
		conf.imageHeight = strtol(args["_SizeY"].c_str(), 0, 0);
	}
	if ( args.find("_Dictionaries") != args.end() ) {
		strings dics = Text::split(args["_Dictionaries"], ';');
		for (auto it = dics.begin(); it != dics.end(); it++) {
			std::ifstream fin;
			fin.open(it->c_str());
			if ( fin.fail() ) {
				throw std::string("[" __FUNCTION__ "] E2: Failed to open an alias dictionary file '") + *it + "'.";
			}
			conf.load(fin, 0);
		}
	}
	treeExps = Text::split(args["_Trees"], ';');

	{
		std::cout << boost::format("Image size: %d x %d\n") % conf.imageWidth % conf.imageHeight << std::endl;
		std::cout << "----- ----- ----- -----" << std::endl;
	}

	// 1. Initialize PPE and Archive.
	ppe = PicturePerfect::PicturePerfectEngine::createInstance();

	archive = PicturePerfect::Archive::createInstance(cvSize(conf.imageWidth, conf.imageHeight));

	for (auto it = conf.aliases.begin(); it != conf.aliases.end(); it++) {
		aliases[it->first.substr(1, it->first.size() - 2)] = it->second;
	}

	// 2. Process source IVs.
	for (int i = 0; i < 10; i++) {
#if 1
		std::string key = (boost::format("_Load%d") % i).str();
#else
		std::string key = (boost::format("_Src%d") % i).str();
#endif
		if ( args.find(key) == args.end() ) break;

		std::string image;
		double      value;
		std::string::size_type pos = args[key].find_last_of(',');
		if ( pos != std::string::npos ) {
			image = args[key].substr(0, pos);
			value = strtod(args[key].c_str() + (pos + 1), 0);
		}
		else {
			image = args[key];
			value = 0.0;
		}
		
		if ( image.empty() || Text::equals(image, "__BLACK__") ) {
			archive->imageAt(ADDR_SI3B + i) = cvScalar(0, 0, 0);
		}
		else if ( Text::equals(image, "__WHITE__") ) {
			archive->imageAt(ADDR_SI3B + i) = cvScalar(255, 255, 255);
		}
		else {
			archive->loadImage(ADDR_SI3B + i, image);
		}
		archive->setValue(ADDR_SV1D + i, value);
	}

	// 3. Prepare IVTrees
	for (unsigned i = 0; i < treeExps.size(); i++) {
		// Convert tree format from S-expression to native.
#if 1
		trees.push_back(			
			Garnet::resolveAliases(
				Garnet::Tree(treeExps[i]),
				aliases.cbegin(),
				aliases.cend()));
#else
		trees.push_back(
			Garnet::Tree(
				Garnet::resolveAliases(
					treeExps[i],
					conf.aliases.cbegin(),
					conf.aliases.cend())));
#endif

		// Reserve result archive.
		archive->at<cv::Mat>(ADDR_RI1B + i) = cvScalar(0);
		archive->at<double>(ADDR_RV1D + i) = 0;
	}
	{
		std::cout << boost::format("IVTrees: %u trees.\n") 
				% trees.size();
		for (unsigned i = 0; i < trees.size(); i++) {
			std::cout << boost::format("[%u] %s\n") 
					% i
					% trees[i].getSExpression();
		}
		std::cout << "----- ----- ----- -----" << std::endl;
	}

	// 4. Convert IVTrees to IVGraph.
	watches[0].reset();
#if 1
	//iv = IVGraphProcessor().convertToIVGraph(trees);
#else
	Garnet::convertIVTreesToIVGraph(trees, iv);
#endif
	watches[0].stop();
	{
		// 2.2 Show source information.
		std::cout << boost::format("IVGraph: %u vertices, %u edges\n")
				% boost::num_vertices(iv)
				% boost::num_edges(iv);
		print_graph(iv, get(&IVGraph::vertex_property_type::label, iv));
		std::cout << "----- ----- ----- -----" << std::endl;
	}

	// 5. Convert IVGraph to PPEGraph.
	watches[1].reset();
#if 1
	//pg = IVGraphProcessor().convertToPPEGraph(iv);
#else
	Garnet::convertIVGraphToPPEGraph(iv, pg);
#endif
	watches[1].stop();
	{
		print_graph(pg, get(&PPEGraph::vertex_property_type::label, pg));
		std::cout << "----- ----- ----- -----" << std::endl;
	}

	// 6. Generate script.
	watches[2].reset();
	ppscript << "Begin Execution\n";
#if 1
	//ppscript.str(PPEGraphProcessor().generateScript(pg));
#else
	Garnet::generateScript(pg, ppscript);
#endif
	ppscript << "End Execution\n";
	watches[2].stop();
	/*{
		std::cout << "----- ----- ----- -----" << std::endl;
	}//*/

	ConfigLoader::readSections(ppscript, sections);
	{
		std::cout << sections["Execution"].str();
		std::cout << "----- ----- ----- -----" << std::endl;
	}

	// 7. Run script.
	// 7.1 Compile
	PicturePerfect::ExecutablePtr executable;
	watches[3].reset();
	executable = ppe->compile(sections["Execution"]);
	watches[3].stop();

	// 7.2 Execute.
	watches[4].reset();
	ppe->execute(executable, archive);
	watches[4].stop();
	{
		std::cout << "----- ----- ----- -----" << std::endl;
	}

	// 7.3 Gather raw fitnesses.
	uint32_t index = 0;
	while (archive->exists(ADDR_RV1D + index)) {
		fitness.push_back(archive->getValue(ADDR_RV1D + index));
		index++;
	}

	// 8. Show result.
	// 8.1 Show numbers.
	if ( args.find("_Number") != args.end() ) {
		std::cout << "Results:\n";
		for (unsigned i = 0; i < fitness.size(); i++) {
			std::cout << boost::format("  [%d] %.9f\n") % i % fitness[i];
		}
		std::cout << "----- ----- ----- -----" << std::endl;
	}

	// 8.2 Display images.
	if ( args.find("_Window") != args.end() ) {
		for (unsigned i = 0; i < fitness.size(); i++) {
			std::string key = (boost::format("Result%d") % i).str();
			cv::namedWindow(key, CV_WINDOW_AUTOSIZE);
			cv::imshow(key, archive->getImage(ADDR_RI1B + i));
		}
		cv::waitKey();
		std::cout << "----- ----- ----- -----" << std::endl;
	}

	// A. Show information.
	{
		std::cout << boost::format("[%s] Trees to IVGraph    %.6fs\n") % __FUNCTION__ % watches[0].get();
		std::cout << boost::format("[%s] IVGraph to PPEGraph %.6fs\n") % __FUNCTION__ % watches[1].get();
		std::cout << boost::format("[%s] PPEGraph to Script  %.6fs\n") % __FUNCTION__ % watches[2].get();
		std::cout << boost::format("[%s] Compilation         %.6fs\n") % __FUNCTION__ % watches[3].get();
		std::cout << boost::format("[%s] Execution           %.6fs\n") % __FUNCTION__ % watches[4].get();
		std::cout << boost::format("[%s] Number of Operators %.6u\n") % __FUNCTION__ % executable->getNumberOfOperators();
		std::cout << boost::format("[%s] Number of Data      %.6u\n") % __FUNCTION__ % executable->getNumberOfData();
		std::cout << std::flush;
	}
}

void Garnet::Drivers::executeFitnessVector(StringMap& args)
{
	using namespace Garnet;
	using namespace PicturePerfect;

	GPEnginePtr engine;
	Configuration conf;
	Generation generation;
	StopWatch watches[5];

	// 0. Parse arguments.
	std::string infilename;
	std::string prjfilename;
	std::string outfilename;
	if ( args.find("_Input") != args.end() ) {
		infilename = args["_Input"];
	}
	if ( args.find("_Project") != args.end() ) {
		prjfilename = args["_Project"];
	}
	if ( args.find("_Output") != args.end() ) {
		outfilename = args["_Output"];
	}

	// 1. Configuration
	if ( !prjfilename.empty() ) {
		std::ifstream fin(prjfilename.c_str());
		conf.load(fin, 0);
	}
	else {
	}

	// 2. Create Garnet engine.
	engine = GPEngine::create(conf);

	// 3. Create generation.
	if ( !infilename.empty() ) {
		std::ifstream fin(infilename.c_str());
		generation = Generation::createFromFile(fin);
	}
	else {
		generation = Generation::create(engine);
	}

	//
	uint32_t Ni = static_cast<uint32_t>(generation.individuals.size());


	// 4. Evaluation
	StopWatch watch;
	engine->evaluate(generation);
	watch.stop();

	// A. Show information.
	{
		std::vector<size_t> targets;
		for (auto i = 0u; i < Ni; i++) {
			targets.push_back(i);
		}
		engine->saveLastImages(args["_WorkingDirectory"], generation.individuals, targets);

		std::cout << "----- ----- ----- -----\n";

		for (auto i = 0u; i < Ni; i++) {
			std::cout << boost::format("Individual %u: (") % i 
				      << print_range(generation.individuals[i].fitness.begin(), generation.individuals[i].fitness.end(), ", ")
					  << ")\n  "
					  << print_range(generation.individuals[i].chromosome.begin(), generation.individuals[i].chromosome.end(), ",\n  ")
					  << "\n";
		}

		std::cout << "----- ----- ----- -----\n";

		double compTime, execTime;
		uint32_t numOps, numData;

		std::cout << boost::format("[%s] Evaluation Total        %.6fs\n") % __FUNCTION__ % watch.get();

		engine->getInformation(INFO_COMPILATION_TIME, &compTime);
		engine->getInformation(INFO_EXECUTION_TIME, &execTime);
		engine->getInformation(INFO_NUMBER_OF_IV_OPERATORS, &numOps);
		engine->getInformation(INFO_NUMBER_OF_IV_DATA, &numData);
		std::cout << boost::format("[%s]   Tree: Compilation     %.6fs\n") % __FUNCTION__ % compTime;
		std::cout << boost::format("[%s]   Tree: Execution       %.6fs\n") % __FUNCTION__ % execTime;
		std::cout << boost::format("[%s]   Tree: # of Operators  %.6u\n") % __FUNCTION__ % numOps;
		std::cout << boost::format("[%s]   Tree: # of Data       %.6u\n") % __FUNCTION__ % numData;

		engine->getInformation(INFO_COMPILATION_TIME, &compTime);
		engine->getInformation(INFO_EXECUTION_TIME, &execTime);
		engine->getInformation(INFO_NUMBER_OF_IV_OPERATORS_OF_EVALUATOR, &numOps);
		engine->getInformation(INFO_NUMBER_OF_IV_DATA_OF_EVALUATOR, &numData);
		std::cout << boost::format("[%s]   RFit: Compilation     %.6fs\n") % __FUNCTION__ % compTime;
		std::cout << boost::format("[%s]   RFit: Execution       %.6fs\n") % __FUNCTION__ % execTime;
		std::cout << boost::format("[%s]   RFit: # of Operators  %.6u\n") % __FUNCTION__ % numOps;
		std::cout << boost::format("[%s]   RFit: # of Data       %.6u\n") % __FUNCTION__ % numData;

		engine->getInformation(INFO_COMPILATION_TIME, &compTime);
		engine->getInformation(INFO_EXECUTION_TIME, &execTime);
		engine->getInformation(INFO_NUMBER_OF_IV_OPERATORS_OF_CONDITIONER, &numOps);
		engine->getInformation(INFO_NUMBER_OF_IV_DATA_OF_CONDITIONER, &numData);
		std::cout << boost::format("[%s]   FVec: Compilation     %.6fs\n") % __FUNCTION__ % compTime;
		std::cout << boost::format("[%s]   FVec: Execution       %.6fs\n") % __FUNCTION__ % execTime;
		std::cout << boost::format("[%s]   FVec: # of Operators  %.6u\n") % __FUNCTION__ % numOps;
		std::cout << boost::format("[%s]   FVec: # of Data       %.6u\n") % __FUNCTION__ % numData;

#if 0
		// TODO: Enable this section by a certain modification.
		PicturePerfectEnginePtr ppe = getPPE();
		ppe->getInformation(INFO_NUMBER_OF_OPERATORS, &numOps);
		std::cout << "Op#,  Calls, Average[ms],  Stdev[ms],  Name\n";
		for (auto i = 0u; i < numOps; i++) {
			const Statistics& stats = ppe->getStatistics((uint16_t)i);
			if ( stats.getCount() == 0 ) continue;
			std::cout << boost::format("%3u, %8u, %8.3f, %8.3f, %s\n")
				% i
				% stats.getCount()
				% (stats.getAverage() * 1000)
				% (stats.getStdev() * 1000)
				% ppe->getOpName((uint16_t)i);
		}
#endif

		std::cout << std::flush;
	}
}

ConfigLoader::StringMap Garnet::parseCommandLine(const strings& args)
{
	namespace fs = boost::filesystem;

	ConfigLoader::StringMap ret;

#if 1
	//
	int mode = 0;
	for (auto it = args.begin(); it != args.end(); it++) {
		auto& arg = *it;
		if (arg[0] == '/' || arg[0] == '-') {
			switch (arg[1]) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'G':
			case 'K':
			case 'S':
			case 'd':
			case 'g':
			case 'i':
			case 'm':
			case 'o':
			case 's':
			case 'v':
			case 'r': // report types
			case 't': // tree
			case 'a': // alias dictionary
			case 'c': // CSV 
			case 'f': // Result Image File
			case 'h': // Summary HTML
			case 'x': // Size X
			case 'y': // Size Y
				mode = arg[1];
				break;
			case 'w': // Window
				ret["_Window"] = "True";
				break;
			case 'n': // Number
				ret["_Number"] = "True";
				break;
			case 'I': // Tree Image
				ret["_TreeImage"] = "True";
				break;
			case 'T': // Trace
				Garnet::upgradeTraceLevel();
				mode = arg[1];
				break;
			}
		}
		else {
			switch (mode) {
			case 0:
				ret["_Project"] = arg;
				break;
			case '0':
				ret["_Src0"] = arg;
				break;
			case '1':
				ret["_Src1"] = arg;
				break;
			case '2':
				ret["_Src2"] = arg;
				break;
			case '3':
				ret["_Src3"] = arg;
				break;
			case '4':
				ret["_Src4"] = arg;
				break;
			case '5':
				ret["_Src5"] = arg;
				break;
			case '6':
				ret["_Src6"] = arg;
				break;
			case '7':
				ret["_Src7"] = arg;
				break;
			case '8':
				ret["_Src8"] = arg;
				break;
			case '9':
				ret["_Src9"] = arg;
				break;
			case 'G':
				ret["_MaxGeneration"] = arg;
				break;
			case 'K': // Cross Validation
				ret["_CrossValidation"] = arg;
				break;
			case 'S': // Random Cases
				ret["_RandomCases"] = arg;
				break;
			case 'T': // Trace Flags
				Garnet::setTrace(arg);
				break;
			case 'a': // alias dictionary
				if (!ret["_Dictionaries"].empty()) {
					ret["_Dictionaries"] += ";";
				}
				ret["_Dictionaries"] += arg;
				break;
			case 'c': // CSV 
				ret["_CsvFileName"] = arg;
				break;
			case 'd':
				ret["_Directory"] = arg;
				break;
			case 'f': // Result Image File
				ret["_ResultFileName"] = arg;
				break;
			case 'g':
				ret["_InitialGeneration"] = arg;
				break;
			case 'h': // Summary HTML
				ret["_SummaryFileName"] = arg;
				break;
			case 'i':
				ret["_Input"] = arg;
				break;
			case 'm':
				ret["_Module"] = arg;
				break;
			case 'o':
				ret["_Output"] = arg;
				break;
			case 'r':
				ret["_ReportTypes"] = arg;
				break;
			case 's':
				ret["_RandomSeed"] = arg;
				break;
			case 't': // tree
				if (!ret["_Trees"].empty()) {
					ret["_Trees"] += ";";
				}
				ret["_Trees"] += arg;
				break;
			case 'v':
				ret["_Verbose"] = arg;
				break;
			case 'x': // Size X
				ret["_SizeX"] = arg;
				break;
			case 'y': // Size Y
				ret["_SizeY"] = arg;
				break;
			}
			mode = 0;
		}
	}

	// Argument resolution.
	if (ret.find("_Project") == ret.end()) {
		throw "A project name must be specified.";
	}
	// Project name.
	//   A project name is made by removing path and extension from _Project.
	//   Note: All extensions will be removed if multiple extensions are included.
	fs::path projpath(ret["_Project"]);
	{
		fs::path projname = projpath;
		while (projname.has_extension()) {
			projname.replace_extension("");
		}
		ret["_ProjectName"] = projname.string();
	}
	// Working directory.
	//   If _Directory is specified, it will be used.
	//   otherwise, use _ProjectName.
	if (ret.find("_Directory") != ret.end()) {
		ret["_WorkingDirectory"] = ret["_Directory"];
	}
	else {
		ret["_WorkingDirectory"] = ret["_ProjectName"];
	}
	// Is Projet?
	fs::path projfile = projpath / (ret["_ProjectName"] + ".garnet_proj.txt");
	if (fs::exists(projfile)) {
		ret["_IsProject"] = "True";
	}
	// The current directory.
	ret["_CurrentPath"] = fs::current_path().string();
#else
	//
	int mode = 0;
	for (auto it = args.begin(); it != args.end(); it++) {
		auto& arg = *it;
		if (arg[0] == '/' || arg[0] == '-') {
			switch (arg[1]) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'G':
			case 'd':
			case 'g':
			case 'i':
			case 'm':
			case 'o':
			case 's':
			case 'v':
			case 't': // tree
			case 'a': // alias dictionary
			case 'c': // CSV 
			case 'f': // Result Image File
			case 'h': // Summary HTML
			case 'x': // Size X
			case 'y': // Size Y
				mode = arg[1];
				break;
			case 'w': // Window
				ret["_Window"] = "True";
				break;
			case 'n': // Number
				ret["_Number"] = "True";
				break;
			case 'I': // Tree Image
				ret["_TreeImage"] = "True";
				break;
			case 'T': // Trace
				Garnet::upgradeTraceLevel();
				mode = arg[1];
				break;
			}
		}
		else {
			switch (mode) {
			case '0':
				ret["_Src0"] = arg;
				break;
			case '1':
				ret["_Src1"] = arg;
				break;
			case '2':
				ret["_Src2"] = arg;
				break;
			case '3':
				ret["_Src3"] = arg;
				break;
			case '4':
				ret["_Src4"] = arg;
				break;
			case '5':
				ret["_Src5"] = arg;
				break;
			case '6':
				ret["_Src6"] = arg;
				break;
			case '7':
				ret["_Src7"] = arg;
				break;
			case '8':
				ret["_Src8"] = arg;
				break;
			case '9':
				ret["_Src9"] = arg;
				break;
			case 'G':
				ret["_MaxGeneration"] = arg;
				break;
			case 'd':
				ret["_WorkingDirectory"] = arg;
				break;
			case 'g':
				ret["_InitialGeneration"] = arg;
				break;
			case 'i':
				ret["_Input"] = arg;
				break;
			case 'm':
				ret["_Module"] = arg;
				break;
			case 'o':
				ret["_Output"] = arg;
				break;
			case 's':
				ret["_RandomSeed"] = arg;
				break;
			case 'v':
				ret["_Verbose"] = arg;
				break;
			case 't': // tree
				if (!ret["_Trees"].empty()) {
					ret["_Trees"] += ";";
				}
				ret["_Trees"] += arg;
				break;
			case 'a': // alias dictionary
				if (!ret["_Dictionaries"].empty()) {
					ret["_Dictionaries"] += ";";
				}
				ret["_Dictionaries"] += arg;
				break;
			case 'c': // CSV 
				ret["_CsvFileName"] = arg;
				break;
			case 'f': // Result Image File
				ret["_ResultFileName"] = arg;
				break;
			case 'h': // Summary HTML
				ret["_SummaryFileName"] = arg;
				break;
			case 'x': // Size X
				ret["_SizeX"] = arg;
				break;
			case 'y': // Size Y
				ret["_SizeY"] = arg;
				break;
			case 'T': // Trace Flags
				Garnet::setTrace(arg);
				break;
			case 0:
				ret["_Project"] = arg;
				break;
			}
			mode = 0;
		}
	}

	// Argument resolution.
	if (ret.find("_Project") == ret.end()) {
		throw "A project name must be specified.";
	}
	// Project title.
	//   'title' is made by removing path and extension from _Project.
	//   Note: All extensions will be removed if multiple extensions are included.
	{
		std::string::size_type st, ed;

		st = ret.at("_Project").find_last_of("/\\");
		st = (st == std::string::npos ? 0 : st + 1);

		ed = ret.at("_Project").find_first_of('.', st);

		ret["_Title"] = ret.at("_Project").substr(st, ed - st);
	}
	// Working directory.
	//   If _WorkingDirectory is specified, use it.
	//   otherwise, if _Project includes a path, use "thepath/title".
	//   otherwise, use "./title"
	if (ret.find("_WorkingDirectory") != ret.end()) {
		ret["_Directory"] = ret.at("_WorkingDirectory") + "/";
	}
	else {
		std::string::size_type pos = ret.at("_Project").find_last_of("/\\");
		ret["_Directory"] = (boost::format("%s/%s/")
			% (pos != std::string::npos ? ret.at("_Project").substr(0, pos) : ".")
			% ret["_Title"]).str();
	}
	// Project filename.
	{
		std::string::size_type st, pos;

		st = ret.at("_Project").find_last_of("/\\");
		st = (st == std::string::npos ? 0 : st + 1);

		if ((pos = ret.at("_Project").find(".", st)) != std::string::npos) {
			ret["_ProjectName"] = ret.at("_Project");
		}
		else {
			ret["_ProjectName"] = ret.at("_Project") + ".garnet";
		}
	}
	//
	std::string projfile = (boost::format("%s/%s.garnet_proj.txt") % ret["_ProjectName"] % ret["_ProjectName"]).str();
	if (boost::filesystem::exists(boost::filesystem::path(projfile))) {
		ret["_IsProject"] = "True";
	}
#endif

	//
	if (Garnet::isTrace()) {
		std::cout << boost::format("Trace: Loc:%c,Scr:%c,Sto:%c,Pro:%c")
			% (Garnet::isLocationTrace() ? "Y" : "N")
			% (Garnet::isScriptTrace() ? "Y" : "N")
			% (Garnet::isStorageTrace() ? "Y" : "N")
			% (Garnet::isProcessTrace() ? "Y" : "N")
			<< std::endl;
	}

	return ret;
}

namespace fs = boost::filesystem;


uint32_t Garnet::findBestIndividualIndex(const Garnet::IndividualList& individuals)
{
	uint32_t ret;
	double bestDist = std::numeric_limits<double>::max();
	for (auto i = 0u; i < individuals.size(); i++) {
		const auto& ind = individuals[i];
		double dist = std::inner_product(ind.fitness.begin(), ind.fitness.end(), ind.fitness.begin(), 0.0);
		if (dist < bestDist) {
			bestDist = dist;
			ret = i;
		}
	}
	return ret;
};

int  Garnet::findLastGeneration(const boost::filesystem::path& projdir)
{
	int lastGeneration = -1;
	std::for_each(fs::directory_iterator(projdir), fs::directory_iterator(), [&lastGeneration](const fs::directory_entry& entry) {
		const fs::path& path = entry.path();
		std::string name = path.filename().string();
		if (std::regex_match(name, std::regex("[0-9]{6}"))) {
			fs::path genfile = path / (boost::format("generation.%s.txt") % name).str();
			fs::path genfilegz = path / (boost::format("generation.%s.txt.gz") % name).str();
			if (fs::exists(genfile) || fs::exists(genfilegz)) {
				int g = std::stol(name);
				if (g > lastGeneration) {
					lastGeneration = g;
				}
			}
		}
	});
	return lastGeneration;
};
