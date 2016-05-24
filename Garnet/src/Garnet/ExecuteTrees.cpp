
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

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

#include <PicturePerfectEngine/PicturePerfectEngine.h>
#include <PicturePerfectEngine/Archive.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

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


/**
 *
 */
void Garnet::Drivers::executeTrees(StringMap& args)
{
	Garnet::Configuration conf;

    //-----------------------------------------------------
	// Determine project location and name.
    //-----------------------------------------------------
	resolveProjectInformation(args, &conf);

    //-----------------------------------------------------
	// 0. Load arguments
    //-----------------------------------------------------
	std::string infilename;
	{
		// Load from configuration file.
		std::ifstream fin;
        fin.open(conf.prj.c_str());
	    if ( fin.fail() ) {
		    throw std::string("[" __FUNCTION__ "] E2: Failed to open a project file '") + conf.prj + "'.";
	    }
        conf.load(fin, 0);
	}
	if ( args.find("_Input") != args.end() ) {
		infilename = args["_Input"];
	}

	// Override if parameters are specified by command line agruments.
	if ( args.find("_RandomSeed") != args.end() ) {
		conf.randomSeed = strtoul(args["_RandomSeed"].c_str(), 0, 0);
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
	{
		conf.numThreads.clear();
		conf.numThreads.push_back(1);
	}
	{
		std::cout << boost::format("Image size: %d x %d\n") % conf.imageWidth % conf.imageHeight << std::endl;
		std::cout << "----- ----- ----- -----" << std::endl;
	}

    //-----------------------------------------------------
	// 1. Create GA Engine
    //-----------------------------------------------------
    Garnet::GPEnginePtr  ga;
	{
		std::ofstream ftrace("trace.------.txt");
		setTraceStream(&ftrace);

		ga = Garnet::GPEngine::create(conf);
		if ( !ga ) {
			throw "Invalid GA Engine Type.";
		}

		setTraceStream(0);
		ftrace.close();
	}

    //-----------------------------------------------------
    // 2. Prepare trees as a generation.
    //-----------------------------------------------------
	Garnet::Generation generation;

	if ( !infilename.empty() ) {
		std::ifstream fin(infilename.c_str());
		generation = Generation::createFromFile(fin);
	}
	else {
		std::vector<std::string> treeExps;
		treeExps = Text::split(args["_Trees"], ';');

		for (auto it = treeExps.cbegin(); it != treeExps.cend(); it++) {
			Individual ind;
			ind.chromosome.push_back(Tree(*it));
			generation.individuals.push_back(ind);
		}
	}

    //-----------------------------------------------------
    // 3. Evaluate.
    //-----------------------------------------------------
    ga->evaluate(generation);

    //-----------------------------------------------------
	// 4. Show result.
    //-----------------------------------------------------
	// 4.1 Show numbers.
	if ( args.find("_Number") != args.end() ) {
		std::cout << "\nResults:";
		for (auto i = 0u; i < generation.individuals.size(); i++) {
			std::cout << "\n  Tree " << i << " " << print_range(
				generation.individuals[i].fitness.cbegin(), 
				generation.individuals[i].fitness.cend(), 
				", ",
				double_digit_putter(9, 6));
		}
		std::cout << "\n----- ----- ----- -----" << std::endl;
	}

	// 4.2 Save tree images.
	if ( args.find("_TreeImage") != args.end() ) {
		std::vector<size_t> targets;
		targets.push_back(0);

		std::cout << "\nSave Tree Images.";
		ga->saveLastImages("", generation.individuals, targets);
		std::cout << "\n----- ----- ----- -----" << std::endl;
	}

	// 4.2 Display images.
#if 0
	if ( args.find("_Window") != args.end() ) {
		for (auto i = 0u; i < generation.individuals.size(); i++) {
			for (auto f = 0u; f < generation.individuals[i].fitness.size(); f++) {
				std::string key = (boost::format("Tree#%d, Fitness#%d") % i).str();
				cv::namedWindow(key, CV_WINDOW_AUTOSIZE);
				cv::imshow(key, ga->saveLastImages());
			}
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
#endif
}
