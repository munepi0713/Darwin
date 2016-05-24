
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/graph/graph_utility.hpp>

#include <Common/ConfigLoader.h>
#include <Common/File.h>
#include <Common/GetTime.h>
#include <Common/PrintRange.h>
#include <Common/Statistics.h>
#include <Common/Text.h>

#include <Garnet/Affinity.h>
#include <Garnet/Dictionary.h>
#include <Garnet/Drivers.h>
#include <Garnet/Garnet.h>
#include <Garnet/IVGraphProcessor.h>
#include <Garnet/IVTreeStrategy.h>

#include <PicturePerfectEngine/Archive.h>
#include <PicturePerfectEngine/PicturePerfectEngine.h>


namespace fs = boost::filesystem;

namespace {

	struct Report {
		Statistics ivsize;  // Keep aliases.
		Statistics ivrsize; // Alias resolved.
		Statistics ivgsize; // IVGraph. Common part unioned.
		Statistics ivgfsize; // IVGraph. Forest.
		Statistics pgsize;  // PPEGraph. I and V are separated.
		Statistics pgfsize;  // PPEGraph. I and V are separated.
		Statistics isize;   // I tree of PPE Graph.
		Statistics vsize;   // V tree of PPE Graph. 
		std::vector<Statistics> fitness;

		Statistics iv1size;
		Statistics ivr1size;
		Statistics ivg1size;
		Statistics ivgf1size;
		Statistics pg1size;
		Statistics pgf1size;
		Statistics i1size;
		Statistics v1size;
		std::vector<Statistics> fitness1;

		uint32_t   numRanks;
	};

}//end of local namespace

/**
 *  Make statistics report per generation for all generations.
 *    Size process for all generations.
 *      IV of All individuals: Size sum, Max size, Average size, Stdev of size, Min size.
 *      IV of Rank1 individuals: Size sum, Max size, Average size, Stdev of size, Min size.
 *      PP of All individuals: Size sum, Max size, Average size, Stdev of size, Min size.
 *      PP of Rank1 individuals: Size sum, Max size, Average size, Stdev of size, Min size.
 *      Number of ranks.
 */
void Garnet::Drivers::executeReport(StringMap& params)
{
	Garnet::Configuration conf;

	// 1. Load the configuration file.
	fs::path projdir(params["_Project"]);
	fs::path conffile = projdir / (params["_Project"] + ".garnet_conf.txt");
	fs::ifstream fin(conffile);
	if ( fin.fail() ) {
		throw std::string("E1: Failed to open ") + conffile.string();
	}
    conf.load(fin, 0);
	fin.close();
	conf.dir = params["_Project"] + "/";
	conf.prj = conffile.string();
	conf.title = params["_ProjectName"];

	// 2. Determine a generation range for the finding. ([-g, -G] or [0, the last])
	int gen = 0;
	if (conf.nextGeneration > -1) {
		gen = conf.nextGeneration;
	}
	else {
		int temp = Garnet::findLastGeneration(projdir);
		if (temp == -1) {
			throw "No results to report.";
		}
		gen = temp;
	}

	uint32_t    maxGeneration = 999999;
#if 0
	Garnet::Configuration conf;

	// Determine project location and name.
	resolveProjectInformation(args, &conf);

	// 0. Load arguments
	std::string infilename;
	std::string outfilename;
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
	if ( args.find("_Output") != args.end() ) {
		outfilename = args["_Output"];
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
	if ( args.find("_MaxGeneration") != args.end() ) {
		maxGeneration = strtoul(args["_MaxGeneration"].c_str(), 0, 0);
	}
#endif

//	const size_t Ni = conf.populationSize;
	const size_t Nf = conf.fitness.size();
//	const size_t Nt = conf.tasks.size();
	const size_t Nc = conf.numChromosomes;
	const size_t Ns = conf.tasks.front().images.size();
	      size_t Nk = 0u;

	// 3.0 Temporary alias.
	std::vector<std::string>      constList;
	//ConfigLoader::StringMap       aliases;
	std::map<std::string, Tree>   aliases;

	for (auto it = conf.aliases.begin(); it != conf.aliases.end(); it++) {
		aliases[it->first.substr(1, it->first.size() - 2)] = it->second;
	}

	// 1. Constant ----------------------------------------
	// Search Const_N nodes from building blocks, evaluator def, and fitness vector defs.
	// Const_N are treated as source tuple so those are added to sources.
	auto constFinder = [](const Tree& t, std::vector<std::string>& constList) {
		for (auto k = 0u; k != t.getNumVertices(); k++) {
			auto u = t.getVertex(k);
			if (Text::startsWith(u->label, "Const_")) {
				constList.push_back(u->label);
			}
		}
	};

	// 1.1 Const_N in building blocks.
	for (auto it = conf.units.cbegin(); it != conf.units.cend(); it++) {
		for (auto jt = it->second.cbegin(); jt != it->second.cend(); jt++) {
			constFinder(resolveAliases(Tree(*jt), aliases.begin(), aliases.end()), constList);
		}
	}

    // 1.4 Remove duplications.
	std::sort(constList.begin(), constList.end());
	std::unique(constList.begin(), constList.end());
	Nk = constList.size();

	
	// 2 Add aliases.
	// 2.1 (Const_N) to (_LoadN) (Stage A, B, C)
	for (auto k = 0u; k < Nk; k++) {
		aliases[constList[k]] = (boost::format("(_Src%u)") % k).str();
	}
	// 2.2 (SrcN) to (_LoadN) (Stage A, B)
	for (auto s = 0u; s < Ns; s++) {
		aliases[(boost::format("Src%u") % s).str()] = (boost::format("(_Src%u)") % (Nk + s)).str();
	}

	// 3.1 Create directory.
	fs::path repodir = projdir / "report";
	fs::create_directories(repodir);

	// 3.2 Gather data.
	std::stringstream sreport;
	for (uint32_t g = 0; g < maxGeneration; g++) {
		std::cout << "Generation " << g << ": Checking ... " << std::flush;

		StopWatch watch;

		fs::path gendir = projdir / (boost::format("%06u") % g).str();

		// Check if the directory for the generation g exists.
		if ( !boost::filesystem::exists(gendir) ) {
			// Not exists. Finish.
			std::cout << "Terminated." << std::endl;
			break;
		}

		// Load generation file of this generation.
		fs::path genfile = Common::File::findGenerationFile(projdir, gen);
		fs::ifstream fin(genfile);
		if ( fin.fail() ) {
			throw std::string("[" __FUNCTION__ "] E2: Failed to open the input file ") + genfile.string() + ".";
		}
		fin.seekg(0, std::ios_base::end);
		size_t size = (size_t)fin.tellg();
		fin.seekg(0, std::ios_base::beg);
		std::vector<char> buffer(size + 1);
		fin.read(buffer.data(), size);
		buffer[size] = 0;
		fin.close();

		// Gather data.
		std::cout << "Gathering ... " << std::flush;
		Report report;
		report.fitness.assign(Nf, Statistics());
		report.fitness1.assign(Nf, Statistics());
		const uint32_t INVALID = std::numeric_limits<uint32_t>::max();
		uint32_t i = INVALID; // # of individual. initialized as invalid.
		uint32_t ivSize = INVALID;
		uint32_t ivrSize = INVALID;
		uint32_t ivgSize = INVALID;
		uint32_t ivgfSize = INVALID;
		uint32_t pgSize = INVALID;
		uint32_t pgfSize = INVALID;
		uint32_t iSize = INVALID;
		uint32_t vSize = INVALID;
		uint32_t rank = INVALID;
		uint32_t maxRank = 0;
		std::vector<double> fitness(Nf);
		auto flushReport = [&] {
			report.ivsize.put(ivSize);
			report.ivrsize.put(ivrSize);
			report.ivgsize.put(ivgSize);
			report.ivgfsize.put(ivgfSize);
			report.pgsize.put(pgSize);
			report.pgfsize.put(pgfSize);
			report.isize.put(iSize);
			report.vsize.put(vSize);
			for (auto f = 0u; f < Nf; f++) {
				report.fitness[f].put(fitness[f]);
			}
			if ( rank == 1 ) {
				report.iv1size.put(ivSize);
				report.ivr1size.put(ivrSize);
				report.ivg1size.put(ivgSize);
				report.ivgf1size.put(ivgfSize);
				report.pg1size.put(pgSize);
				report.pgf1size.put(pgfSize);
				report.i1size.put(iSize);
				report.v1size.put(vSize);
				for (auto f = 0u; f < Nf; f++) {
					report.fitness1[f].put(fitness[f]);
				}
			}
			if ( rank > maxRank ) {
				maxRank = rank;
			}
		};
		std::stringstream sin(buffer.data());
		while ( !sin.eof() ) {
			std::string line;
			std::getline(sin, line);
			if ( sin.fail() ) break;

			if ( Text::startsWith(line, "# Individual") ) {
				strings tokens = parse(line);
				assert(tokens.size() >= 3);
				if ( i != INVALID ) {
					flushReport();
				}
				ivSize = 0;
				ivrSize = 0;
				ivgSize = 0;
				ivgfSize = 0;
				pgSize = 0;
				pgfSize = 0;
				iSize = 0;
				vSize = 0;
				i = std::stoi(tokens[2]);
			}
			else if ( Text::startsWith(line, "Rank") ) {
				strings tokens = parse(line);
				assert(tokens.size() >= 3 && (uint32_t)std::stoi(tokens[1]) == i);
				rank = stoi(tokens[2]);
			}
#if 1
			else if ( Text::startsWith(line, "# IGraph") ) {
				strings tokens = parse(line);
				assert(tokens.size() >= 3 && (uint32_t)std::stoi(tokens[2]) == i);
				if ( tokens.size() > (3 + Nc) ) {
					iSize = 0;
					for (auto c = 0u; c < Nc; c++) {
						Tree tree(tokens[3 + c]);
						iSize += tree.getNumVertices() - 1; // Deduct 'store' operator.
					}
				}
			}
			else if ( Text::startsWith(line, "# VGraph") ) {
				strings tokens = parse(line);
				assert(tokens.size() >= 3 && (uint32_t)std::stoi(tokens[2]) == i);
				if ( tokens.size() > (3 + Nc) ) {
					vSize = 0;
					for (auto c = 0u; c < Nc; c++) {
						Tree tree(tokens[3 + c]);
						vSize += tree.getNumVertices() - 1; // Deduct 'store' operator.
					}
				}
			}
			else if ( Text::startsWith(line, "Chromosomes") ) {
				strings tokens = parse(line);
				assert(tokens.size() >= (3 + Nc) && (uint32_t)std::stoi(tokens[1]) == i);
				ivSize = 0;
				ivrSize = 0;
				ivgSize = 0;
				ivgfSize = 0;
				pgSize = 0;
				pgfSize = 0;
				for (auto c = 0u; c < Nc; c++) {
					Tree tree(tokens[3 + c]);
					ivSize += tree.getNumVertices();

					Tree treer("(_IVStore0(!))");
					treer.replace("!", tree);
					Garnet::resolveAliases(treer, aliases.begin(), aliases.end());
					ivrSize += treer.getNumVertices() - 1; // Deduct 'store' operator.

					std::vector<Tree> trees;
					trees.push_back(treer);

					{
					Garnet::IVGraph iv;
					Garnet::PPEGraph pg;
					Garnet::IVGraphProcessor ivp;

					ivp.setInformation(IVGraphProcessor::GROUP_METHOD, IVGraphProcessor::GROUP_AS_FOREST);
					ivp.compile(trees);

					iv = ivp.getLastGraphInfo()[0].iv;
					ivgfSize += boost::num_vertices(iv) - 2; // Deduct 'store' operators.

					pg = ivp.getLastGraphInfo()[0].pg;
					pgfSize += boost::num_vertices(pg) - 2; // Deduct 'store' operators.
					}
					{
					Garnet::IVGraph iv;
					Garnet::PPEGraph pg;

					Garnet::IVGraphProcessor ivp;

					ivp.setInformation(IVGraphProcessor::GROUP_METHOD, IVGraphProcessor::GROUP_AS_DAG);
					ivp.compile(trees);

					iv = ivp.getLastGraphInfo()[0].iv;
					ivgfSize += boost::num_vertices(iv) - 2; // Deduct 'store' operators.

					pg = ivp.getLastGraphInfo()[0].pg;
					pgfSize += boost::num_vertices(pg) - 2; // Deduct 'store' operators.
					}
				}
			}
			else if ( Text::startsWith(line, "# FVector") ) {
				strings tokens = parse(line);
				assert(tokens.size() >= (3 + Nf) && (uint32_t)std::stoi(tokens[2]) == i);
				for (auto f = 0u; f < Nf; f++) {
					fitness[f] = stod(tokens[3 + f]);
				}
			}
		}
		flushReport(); // Flush last individual.

		// Write to buffer.
		std::cout << "Formatting ... " << std::flush;
#if 1
		sreport << boost::format("%u, ")
			% g;
		sreport << boost::format("%u,%u,%.3f,%.3f,%u, %u,%u,%.3f,%.3f,%u, %u,%u,%.3f,%.3f,%u, %u,%u,%.3f,%.3f,%u, %u,%u,%.3f,%.3f,%u, %u,%u,%.3f,%.3f,%u, %u,%u,%.3f,%.3f,%u, %u,%u,%.3f,%.3f,%u,  ")
			% report.ivsize.getTotal()
			% report.ivsize.getMax()
			% report.ivsize.getAverage()
			% report.ivsize.getStdev()
			% report.ivsize.getMin()
			% report.ivrsize.getTotal()
			% report.ivrsize.getMax()
			% report.ivrsize.getAverage()
			% report.ivrsize.getStdev()
			% report.ivrsize.getMin()
			% report.ivgsize.getTotal()
			% report.ivgsize.getMax()
			% report.ivgsize.getAverage()
			% report.ivgsize.getStdev()
			% report.ivgsize.getMin()
			% report.pgsize.getTotal()
			% report.pgsize.getMax()
			% report.pgsize.getAverage()
			% report.pgsize.getStdev()
			% report.pgsize.getMin()
			% report.isize.getTotal()
			% report.isize.getMax()
			% report.isize.getAverage()
			% report.isize.getStdev()
			% report.isize.getMin()
			% report.vsize.getTotal()
			% report.vsize.getMax()
			% report.vsize.getAverage()
			% report.vsize.getStdev()
			% report.vsize.getMin()
			% report.ivgfsize.getTotal()
			% report.ivgfsize.getMax()
			% report.ivgfsize.getAverage()
			% report.ivgfsize.getStdev()
			% report.ivgfsize.getMin()
			% report.pgfsize.getTotal()
			% report.pgfsize.getMax()
			% report.pgfsize.getAverage()
			% report.pgfsize.getStdev()
			% report.pgfsize.getMin();
		sreport << boost::format("%u,%u,%.3f,%.3f,%u, %u,%u,%.3f,%.3f,%u, %u,%u,%.3f,%.3f,%u, %u,%u,%.3f,%.3f,%u, %u,%u,%.3f,%.3f,%u, %u,%u,%.3f,%.3f,%u, %u,%u,%.3f,%.3f,%u, %u,%u,%.3f,%.3f,%u,  ")
			% report.iv1size.getTotal()
			% report.iv1size.getMax()
			% report.iv1size.getAverage()
			% report.iv1size.getStdev()
			% report.iv1size.getMin()
			% report.ivr1size.getTotal()
			% report.ivr1size.getMax()
			% report.ivr1size.getAverage()
			% report.ivr1size.getStdev()
			% report.ivr1size.getMin()
			% report.ivg1size.getTotal()
			% report.ivg1size.getMax()
			% report.ivg1size.getAverage()
			% report.ivg1size.getStdev()
			% report.ivg1size.getMin()
			% report.pg1size.getTotal()
			% report.pg1size.getMax()
			% report.pg1size.getAverage()
			% report.pg1size.getStdev()
			% report.pg1size.getMin()
			% report.i1size.getTotal()
			% report.i1size.getMax()
			% report.i1size.getAverage()
			% report.i1size.getStdev()
			% report.i1size.getMin()
			% report.v1size.getTotal()
			% report.v1size.getMax()
			% report.v1size.getAverage()
			% report.v1size.getStdev()
			% report.v1size.getMin()
			% report.ivgf1size.getTotal()
			% report.ivgf1size.getMax()
			% report.ivgf1size.getAverage()
			% report.ivgf1size.getStdev()
			% report.ivgf1size.getMin()
			% report.pgf1size.getTotal()
			% report.pgf1size.getMax()
			% report.pgf1size.getAverage()
			% report.pgf1size.getStdev()
			% report.pgf1size.getMin();
#endif
		for (auto f = 0u; f < Nf; f++) {
			sreport << boost::format("%.3f,%.3f,%.3f,%.3f, ") 
				% report.fitness[f].getMax()
				% report.fitness[f].getAverage()
				% report.fitness[f].getStdev()
				% report.fitness[f].getMin();
		}
		sreport << "  ";
		for (auto f = 0u; f < Nf; f++) {
			sreport << boost::format("%.3f,%.3f,%.3f,%.3f, ") 
				% report.fitness1[f].getMax()
				% report.fitness1[f].getAverage()
				% report.fitness1[f].getStdev()
				% report.fitness1[f].getMin();
		}
		sreport << boost::format("%u\n")
			% maxRank;
		watch.stop();
		std::cout << boost::format("Done. %.3f[sec].") % watch.get() << std::endl;
	}

	std::cout << "Writing ... " << std::flush;
	fs::path repofile = repodir / "fitness.csv";
	fs::ofstream freport(repofile);
	freport << "Generation,";
	freport << "IV Total,IV Max,IV Ave,IV Stdev,IV Min,"
		    << "IVR Total,IVR Max,IVR Ave,IVR Stdev,IVR Min,"
		    << "IVG Total,IVG Max,IVG Ave,IVG Stdev,IVG Min,"
		    << "PG Total,PG Max,PG Ave,PG Stdev,PG Min,"
		    << "I Total,I Max,I Ave,I Stdev,I Min,"
		    << "V Total,V Max,V Ave,V Stdev,V Min,"
	        << "IVf Total,IVf Max,IVf Ave,IVf Stdev,IVf Min,"
		    << "PGf Total,PGf Max,PGf Ave,PGf Stdev,PGf Min,"
		    << "Rank1 IV Total,Rank1 IV Max,Rank1 IV Ave,Rank1 IV Stdev,Rank1 IV Min,"
		    << "Rank1 IVR Total,Rank1 IVR Max,Rank1 IVR Ave,Rank1 IVR Stdev,Rank1 IVR Min,"
		    << "Rank1 IVG Total,Rank1 IVG Max,Rank1 IVG Ave,Rank1 IVG Stdev,Rank1 IVG Min,"
		    << "Rank1 PG Total,Rank1 PG Max,Rank1 PG Ave,Rank1 PG Stdev,Rank1 PG Min,"
		    << "Rank1 I Total,Rank1 I Max,Rank1 I Ave,Rank1 I Stdev,Rank1 I Min,"
		    << "Rank1 V Total,Rank1 V Max,Rank1 V Ave,Rank1 V Stdev,Rank1 V Min,"
		    << "Rank1 IVf Total,Rank1 IVf Max,Rank1 IVf Ave,Rank1 IVf Stdev,Rank1 IVf Min,"
		    << "Rank1 PGf Total,Rank1 PGf Max,Rank1 PGf Ave,Rank1 PGf Stdev,Rank1 PGf Min,";
	for (auto f = 0u; f < Nf; f++) {
		freport << boost::format("Fitness[%u] Max,Fitness[%u] Ave,Fitness[%u] Stdev,Fitness[%u] Min,") % f % f % f % f;
	}
	for (auto f = 0u; f < Nf; f++) {
		freport << boost::format("Rank1 Fitness[%u] Max,Rank1 Fitness[%u] Ave,Rank1 Fitness[%u] Stdev,Rank1 Fitness[%u] Min,") % f % f % f % f;
	}
	freport << "# of Ranks,\n"
			<< sreport.str();
	freport.close();
	std::cout << "Done" << std::endl;

	// A. Show information.
#if 0
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

namespace {

std::array<std::string, 5> _types = {
	"detail",
	"filters",
	"fitness",
	"front",
	"test",
};
const enum {
	REPORT_DETAIL,
	REPORT_FILTERS,
	REPORT_FITNESS,
	REPORT_FRONT,
	REPORT_TEST,
};

int executeReportProjectDetail(ConfigLoader::StringMap& params)
{

	return 1;
}

int executeReportProjectFilters(ConfigLoader::StringMap& params)
{

	return 1;
}

int executeReportProjectFitness(ConfigLoader::StringMap& params)
{

	return 1;
}

int executeReportProjectFront(ConfigLoader::StringMap& params)
{

	return 1;
}

int executeReportProjectTest(ConfigLoader::StringMap& params)
{

	return 1;
}

int executeReportCaseDetail(ConfigLoader::StringMap& params)
{
    Garnet::Configuration conf;

	// 1. Load the configuration file.
	fs::path projdir(params["_Project"]);
	fs::path conffile = projdir / (params["_Project"] + ".garnet_conf.txt");
	fs::ifstream fin(conffile);
	if ( fin.fail() ) {
		throw std::string("E1: Failed to open ") + conffile.string();
	}
    conf.load(fin, 0);
	fin.close();
	conf.dir = params["_Project"] + "/";
	conf.prj = conffile.string();
	conf.title = params["_ProjectName"];

	// Include test cases.
	//std::copy(conf.tests.begin(), conf.tests.end(), conf.tasks.end());
	conf.tasks.insert(conf.tasks.end(), conf.tests.begin(), conf.tests.end());

	// 2. Determine a generation range for the finding. ([-g, -G] or [0, the last])
	int gen = 0;
	if (params.find("_InitialGeneration") != params.end()) {
		gen = stoi(params.at("_InitialGeneration"));
	}
	else {
		int temp = Garnet::findLastGeneration(projdir);
		if (temp == -1) {
			throw "No results to report.";
		}
		gen = temp;
	}
	std::cout << boost::format("Generation    : %d\n") % gen;

	// 3. Output for each generation
	boost::system::error_code ec;
	fs::path repodir = projdir / "reports";

	// 2. Load generation file.
	Garnet::Generation generation;

	// 2.1
	fs::path genfile = Common::File::findGenerationFile(projdir, gen);
	Common::File::readFromFile(genfile,
		[&generation](std::istream& ins) {
		generation.load(ins, 0);
	});

	// 2.2 Remove trees except the best one. 
	uint32_t bestOneIndex = findBestIndividualIndex(generation.individuals);
	Garnet::Individual& bestIndividual = generation.individuals[bestOneIndex];

	// 3. Make a report
	for (auto c = 0u; c < bestIndividual.chromosome.size(); c++) {
		auto& g = bestIndividual.chromosome[c].getGraph();
		// 3.1 Create directory.
		fs::path treedir = repodir / (boost::format("g%06u_i%04u_c%02u") % gen % bestOneIndex % c).str();
		boost::filesystem::create_directories(treedir, ec);

		// 3.2 Save graph as GraphML format.
		boost::dynamic_properties dp;
		dp.property("name", get(&Garnet::Tree::VertexProp::label, g));

		fs::path graphfile = treedir / (boost::format("%s_g%06u_i%04u_c%02u.graphml") % conf.title % gen % bestOneIndex % c).str();
		fs::ofstream fgraph(graphfile);
		boost::write_graphml(fgraph, g, dp);
		fgraph.close();

		// 3.3 Save intermediate result for all nodes.
		//     by executing sub-trees rooted by every nodes, excluding terminal nodes.
		Garnet::Generation gtemp;
		for (auto n = 0u; n < boost::num_vertices(g); n++) {
			std::string sexp = Garnet::getSExpression<std::string>(boost::vertex(n, g), g);
			//std::cout << boost::format("\n%u: %s") % n % sexp;

			Garnet::Tree tree(sexp);

			Garnet::Individual ind;
			ind.chromosome.push_back(tree);
			gtemp.individuals.push_back(ind);
		}

		uint32_t Ni = gtemp.individuals.size();
		uint32_t Nt = conf.tasks.size();

		{
			conf.numThreads.clear();
			conf.numThreads.push_back(1);
			conf.numVerticesLimit = 10000;
			conf.populationSize = Ni;
		}
		{
			std::cout << boost::format("Population size: %d\n") % Ni;
			std::cout << boost::format("Number of Tasks: %d\n") % Nt;
			std::cout << boost::format("Image size: %d x %d\n") % conf.imageWidth % conf.imageHeight;
			std::cout << "----- ----- ----- -----" << std::endl;
		}

		// 1. Create GA Engine
		Garnet::GPEnginePtr  ga;
		ga = Garnet::GPEngine::create(conf);
		ga->evaluate(gtemp, false);

		// 2. Make CSV and HTML.
		// 2.0 Preparing HTML flusher.
		auto makeHTMLName = [&](uint32_t u) -> std::string {
			return (boost::format("%s_g%06u_i%04u_c%02u%s.html")
				% conf.title
				% gen
				% bestOneIndex
				% c
				% ((u / 100) == 0 ? "" : (boost::format("_%02d") % (u / 100)).str())
				).str();
		};
		auto writeHTML = [&treedir](const std::string& filename, const std::string& body) {
			fs::path htmlfile = treedir / filename;
			fs::ofstream fhtml(htmlfile);
			fhtml << "<?DOCTYPE html>"
				<< "\n<html>"
				<< "\n<head>"
				<< "\n<title>" << filename << "</title>"
				<< "\n</head>"
				<< "\n<body>"
				<< "\n<h1>" << filename << "</h1>"
				<< body
				<< "\n</body>"
				<< "\n</html>";
			fhtml.close();
		};
		// 2.1
		std::stringstream svalue;
		std::stringstream shtml;
		std::map<uint32_t, uint32_t> parent; // <K,V>=<child, parent>
		auto vertices = boost::vertices(g);
		for (auto it = vertices.first; it != vertices.second; it++) {
			auto u = *it;

			// Save image.
			for (auto t = 0u; t < Nt; t++) {
				fs::path dst = treedir / (boost::format("n%05ut%03u.png") % u % t).str();
				const Garnet::IVTreeStrategy::IVTreeIntermediatePtr& result = std::dynamic_pointer_cast<Garnet::IVTreeStrategy::IVTreeIntermediate>(gtemp.individuals[u].chromosome[0].intermediate);
				cv::imwrite(dst.string(), result->iv[t].image());
			}

			// Skip making CSV/HTML for terminal nodes.
			if (boost::out_degree(u, g) != 0) {
				// 2.1 Make CSV.
				svalue << boost::format("%05u,") % u;
				for (auto t = 0u; t < Nt; t++) {
					const Garnet::IVTreeStrategy::IVTreeIntermediatePtr& result = std::dynamic_pointer_cast<Garnet::IVTreeStrategy::IVTreeIntermediate>(gtemp.individuals[u].chromosome[0].intermediate);
					svalue << boost::format("%.6f,") % result->iv[t].value();
				}
				svalue << "\n";

				// 2.2 Make HTML.
				auto makeRow = [&](const std::string& label, unsigned node) {
					shtml << "\n<tr><th>" << label << "</th>";
					for (auto t = 0u; t < Nt; t++) {
						shtml << boost::format("\n<td><a href=\"n%05ut%03u.png\"><img width=\"80\" src=\"n%05ut%03u.png\"></a></td>")
							% node % t
							% node % t;
					}
					shtml << "\n</tr>\n<tr><th></th>";
					for (auto t = 0u; t < Nt; t++) {
						const Garnet::IVTreeStrategy::IVTreeIntermediatePtr& result = std::dynamic_pointer_cast<Garnet::IVTreeStrategy::IVTreeIntermediate>(gtemp.individuals[node].chromosome[0].intermediate);
						shtml << boost::format("<td>%.6f</td>\n") % result->iv[t].value();
					}
					shtml << "\n</tr>";
				};
				shtml << boost::format("\n<div id=\"node%06u\"><h3>node %06u (%s)</h3>") % u % u % g[u].label;
				shtml << "\n<table>";
				if (parent.find(u) != parent.end()) {
					makeRow((boost::format("result -> <a href=\"%s#node%06u\">node %06u (%s)</a>")
						% makeHTMLName(parent[u])
						% parent[u] 
						% parent[u] 
						% g[parent[u]].label).str(), u);
				}
				else {
					makeRow("result", u);
				}
				auto edges = boost::out_edges(u, g);
				for (auto e = edges.first; e != edges.second; e++) {
					if (boost::out_degree(e->m_target, g) > 0) {
						makeRow((boost::format("<a href=\"%s#node%06u\">node %06u (%s)</a>")
							% makeHTMLName(e->m_target)
							% e->m_target
							% e->m_target
							% g[e->m_target].label).str(), e->m_target);
					}
					else {
						makeRow((boost::format("node %06u (%s)")
							% e->m_target
							% g[e->m_target].label).str(), e->m_target);
					}
					parent[e->m_target] = u;
				}
				shtml << "\n</table>\n</div>";
			}

			// Write
			if (((u % 100) == 99) || (u == boost::num_vertices(g) - 1)) {
				writeHTML(makeHTMLName(u), shtml.str());
				//shtml.str("");
				shtml = std::stringstream();
			}
		}

		// 3.1 Write CSV.
		fs::path valuefile = treedir / "intermediate_values.csv";
		fs::ofstream fvalue(valuefile);
		fvalue << svalue.str();
		fvalue.close();

		// 3.2 Write HTML.
		fs::path htmlfile = repodir / (boost::format("%s_g%06u_best_detail.html") % conf.title % gen).str();
		fs::ofstream fhtml(htmlfile);
		fhtml << "<?DOCTYPE html>"
			<< "\n<html>"
			<< "\n<head>"
			<< "\n<title>" << boost::format("%s_g%06u Detail Report") % conf.title % gen << "</title>"
			<< "\n</head>"
			<< "\n<body>"
			<< "\n<h1>" << boost::format("%s_g%06u Detail Report") % conf.title % gen << "</h1>"
			<< boost::format("\n<a href=\"%s/%s\">%s</a>") % treedir.filename().string() % makeHTMLName(0) % makeHTMLName(0)
			<< "\n</body>"
			<< "\n</html>";
		fhtml.close();
#if 0
		fs::path htmlfile = treedir / (boost::format("%s_g%06u_i%04u_c%02u.html") % conf.title % gen % bestOneIndex % c).str();
		fs::ofstream fhtml(htmlfile);
		fhtml << "<?DOCTYPE html>"
			  << "\n<html>"
			  << "\n<head>"
			  << "\n<title>" << boost::format("%s_g%06u_i%04u_c%02u.html") % conf.title % gen % bestOneIndex % c << "</title>"
			  << "\n</head>"
			  << "\n<body>"
			  << "\n<h1>" << boost::format("%s_g%06u_i%04u_c%02u") % conf.title % gen % bestOneIndex % c << "</h1>"
		      << shtml.str()
		      << "\n</body>"
		      << "\n</html>";
		fhtml.close();
#endif
	}

	// A. Show information.
#if 0
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

	return 0;
}

int executeReportCaseFilters(ConfigLoader::StringMap& params)
{

	return 1;
}

int executeReportCaseFitness(ConfigLoader::StringMap& params)
{
    Garnet::Configuration conf;

	// 1. Load the configuration file.
	fs::path projdir(params["_Project"]);
	fs::path conffile = projdir / (params["_Project"] + ".garnet_conf.txt");
	fs::ifstream fin(conffile);
	if ( fin.fail() ) {
		throw std::string("E1: Failed to open ") + conffile.string();
	}
    conf.load(fin, 0);
	fin.close();
	conf.dir = params["_Project"] + "/";
	conf.prj = conffile.string();
	conf.title = params["_ProjectName"];

	// 2. Determine a generation range for the finding. ([-g, -G] or [0, the last])
	int firstGen = 0, lastGen;
	if (conf.nextGeneration > -1) {
		firstGen = conf.nextGeneration;
	}
	if (params.find("_MaxGeneration") != params.end()) {
		lastGen = std::stoul(params["_MaxGeneration"], 0, 0);
	}
	else {
		lastGen = Garnet::findLastGeneration(projdir);
	}

	//
	uint32_t Nf = conf.fitness.size();

	// 3. Output for each generation
	boost::system::error_code ec;
	fs::path repodir = projdir / "reports";
	fs::path csvfile = repodir / "fitness.csv";
	fs::ofstream fcsv(csvfile);
	// 3.1 Header
	fcsv << "Generation,";
	for (auto f = 0u; f < Nf; f++) fcsv << boost::format("Best %u,") % f;
	for (auto f = 0u; f < Nf; f++) fcsv << boost::format("Rank1 Ave %u,") % f;
	for (auto f = 0u; f < Nf; f++) fcsv << boost::format("All Ave %u,") % f;
	fcsv << std::endl;
	// 3.2 Rows
	StopWatch watch;
	for (auto g = firstGen; g <= lastGen; g++) {
		// 4. Load generation.
		Garnet::Generation generation;
		fs::path genfile = Common::File::findGenerationFile(projdir, g);
		Common::File::readFromFile(genfile,
			[&generation](std::istream& ins) {
			generation.load(ins, 0);
		});

		// 4. Output data.
		std::vector<Statistics> rank1(Nf), all(Nf);
		// 4.0 Determine the best rank is 0 or 1. (It may vary depending on strategy.)
		uint32_t bestRank = generation.individuals[0].rank;
		// 4.1 Find the best individual, which is the closest to the utopia point. Write its data.
		uint32_t bestOneIndex = findBestIndividualIndex(generation.individuals);
		const auto& best = generation.individuals[bestOneIndex].fitness;
		// 4.2 Average rank 1 individuals, and all individuals.
		for (auto i = 0u; i < generation.individuals.size(); i++) {
			const auto& ind = generation.individuals[i];
			for (auto f = 0u; f < Nf; f++) {
				all[f].put(ind.fitness[f]);
				if (ind.rank == bestRank) rank1[f].put(ind.fitness[f]);
			}
		}
		// 4.3 Write its data.
		fcsv << g << ","
			 << print_range(best.begin(), best.end(), ",", double_digit_putter(10, 6)) << ","
			 << print_range(rank1.begin(), rank1.end(), ",", make_mem_fun_putter(std::mem_fun_ref(&Statistics::getAverage))) << ","
			 << print_range(all.begin(), all.end(), ",", make_mem_fun_putter(std::mem_fun_ref(&Statistics::getAverage)))
			 << std::endl;

		// 4.4 Notification
		if ((g - firstGen) % 10 == 9) std::cout << ".";
		if ((g - firstGen) % 100 == 99) {
			std::cout << (g + 1) << boost::format(" %.0f [sec]\n") % watch.lap();
		}
	}
	fcsv.close();
	std::cout << " Finished.";

	return 0;
}

int executeReportCaseFront(ConfigLoader::StringMap& params)
{

	return 1;
}

int executeReportCaseTest(ConfigLoader::StringMap& params)
{
    Garnet::Configuration conf;

	// 1. Load the configuration file.
	fs::path projdir(params["_Project"]);
	fs::path conffile = projdir / (params["_Project"] + ".garnet_conf.txt");
	fs::ifstream fin(conffile);
	if ( fin.fail() ) {
		throw std::string("E1: Failed to open ") + conffile.string();
	}
    conf.load(fin, 0);
	fin.close();
	conf.dir = params["_Project"] + "/";
	conf.prj = conffile.string();
	conf.title = params["_ProjectName"];

	// 2. Load a generation. (-g Xxxx or search the last generation)
	uint32_t gen;
	// 2.1 Determine which generation should be reported.
	if (params.find("_InitialGeneration") != params.end()) {
		gen = stoi(params["_InitialGeneration"]);
	}
	else {
		int temp = Garnet::findLastGeneration(projdir);
		if (temp == -1) {
			throw "No results to report.";
		}
		gen = temp;
	}
	// 2.2 Load generation.
	Garnet::Generation generation;
	fs::path genfile = Common::File::findGenerationFile(projdir, gen);
	Common::File::readFromFile(genfile,
		[&generation](std::istream& ins) {
		generation.load(ins, 0);
	});

	// 3. Find the best individual, which is the closest to the utopia point.
	uint32_t bestOneIndex = findBestIndividualIndex(generation.individuals);
	// 3.2 Create new generation which contains only the best individual.
	Garnet::Generation bestGenerationTask;
	bestGenerationTask.individuals.push_back(generation.individuals[bestOneIndex]);
	Garnet::Generation bestGenerationTest;
	bestGenerationTest.individuals.push_back(generation.individuals[bestOneIndex]);

	// 4. Evaluate the individual.
	// 4.1 Evaluate with Task datasets.
	{
		Garnet::GPEnginePtr  ga;
		ga = Garnet::GPEngine::create(conf);
		if (!ga) throw "Invalid GA Engine Type.";
		ga->evaluate(bestGenerationTask);
	}

	// 4.2 Evaluate with Test datasets.
	{
		Garnet::Configuration confTest = conf;
		confTest.tasks = conf.tests;
		Garnet::GPEnginePtr  ga;
		ga = Garnet::GPEngine::create(confTest);
		if (!ga) throw "Invalid GA Engine Type.";
		ga->evaluate(bestGenerationTest);
	}


	// 5. Save results.
	const auto& bestTask = bestGenerationTask.individuals[0];
	const auto& bestTest = bestGenerationTest.individuals[0];
	const uint32_t Nc = conf.numChromosomes;
	const uint32_t Nr = conf.evaluator.size();
	const uint32_t Nf = conf.fitness.size();
	const uint32_t Nt = conf.tasks.size();
	const uint32_t Nu = conf.tests.size();
	boost::system::error_code ec;
	fs::path repodir = projdir / "reports";
	fs::path treedir = repodir / (boost::format("g%06u_i%04u") % gen % bestOneIndex).str();
	if (!fs::exists(treedir)) {
		fs::create_directory(treedir, ec);
	}
	// 5.1 Save images.
	auto saveImages = [&treedir, &Nc](unsigned Nt, const Garnet::Individual& ind, const std::string& format) {
		for (auto c = 0u; c < Nc; c++) {
			auto intermediate = ind.chromosome[c].getIntermediate<Garnet::IVTreeStrategy::IVTreeIntermediate>();
			for (auto t = 0u; t < Nt; t++) {
				fs::path pngfile = treedir / (boost::format(format) % c % t).str();
				intermediate->iv[t].saveImage(pngfile.string());
			}
		}
	};
	if (Nt > 0) saveImages(Nt, bestTask, "test_c%02u_t%02u.png");
	if (Nu > 0) saveImages(Nu, bestTest, "test_c%02u_u%02u.png");
	// 5.2 Save values.
	fs::path csvfile = treedir / "test_values.csv";
	fs::ofstream fcsv(csvfile);
	// Create saver.
	auto saveCsvRows = [&fcsv, Nc, Nr, Nf](unsigned Nt, const Garnet::Individual& ind, const std::string& type) {
		for (auto t = 0u; t < Nt; t++) {
			fcsv << type << "," << t << ",";
			for (auto c = 0u; c < Nc; c++) {
				const Garnet::Chromosome& chromo = ind.chromosome[c];
				auto intermediate = chromo.getIntermediate<Garnet::IVTreeStrategy::IVTreeIntermediate>();
				fcsv << boost::format("%.6f,") % intermediate->iv[t].value();
			}
			for (auto r = 0u; r < Nr; r++) {
				fcsv << boost::format("%.6f,") % ind.rawFitness[r][t];
			}
			for (auto f = 0u; f < Nf; f++) {
				fcsv << boost::format("%.6f,") % ind.fitness[f];
			}
			fcsv << std::endl;
		}
	};
	// Header
	fcsv << "Type,Number,";
	for (auto c = 0u; c < Nc; c++) {
		fcsv << boost::format("Tree %02u,") % c;
	}
	for (auto r = 0u; r < Nr; r++) {
		fcsv << boost::format("RawFit %02u,") % r;
	}
	for (auto f = 0u; f < Nf; f++) {
		fcsv << boost::format("Fitness %02u,") % f;
	}
	fcsv << std::endl;
	// Data Rows
	if (Nt > 0) saveCsvRows(Nt, bestTask, "Task");
	if (Nu > 0) saveCsvRows(Nu, bestTest, "Test");
	fcsv.close();
	// 5.3 Generation HTML.
	std::vector<int> indices(conf.tasks.size() + conf.tests.size());
	std::iota(indices.begin(), indices.end(), 0);

	fs::path htmlfile = treedir / "test_reports.html";
	fs::ofstream fhtml(htmlfile);
	fhtml << "<!DOCTYPE html>"
		  << "\n<html>"
		  << "\n<head>"
		  << "\n<meta charset=\"utf-8\">"
		  << "\n<style>"
//		  << "\n.default_table {border: solid 1px Black;}"
		  << "\n.default_table th, tr, td {border: solid 1px Black;}"
		  << "\n.chromosome th, tr, td {border: solid 1px Black; font: 8pt Consolas;}"
		  << "\n</style>"
		  << "\n</head>"
		  << "\n<body>"
		  << "\n<h1>" << treedir.filename() << "</h1>";
	auto saveHtmlTable = [&fhtml, Nc, Nr, Nf](unsigned Nt, const Garnet::Individual& ind, const std::string& header, const std::string& filename) {
		// Header & Opening table
		fhtml << "\n<h2>" << header << "</h2>"
			<< "\n<table class=\"default_table\">"
			<< "\n<tbody>";

		// Numbers
		{
			fhtml << boost::format("\n<tr><th>Number</th>");
			for (auto t = 0u; t < Nt; t++) {
				fhtml << boost::format("<th>%u</th>") % (t + 1);
			}
			fhtml << "</tr>";
		}

		// Chromosome
		for (auto c = 0u; c < Nc; c++) {
			auto intermediate = ind.chromosome[c].getIntermediate<Garnet::IVTreeStrategy::IVTreeIntermediate>();

			// I
			fhtml << boost::format("\n<tr><th>Chromo[%d] I</th>") % c;
			for (auto t = 0u; t < Nt; t++) {
				auto format = (boost::format("<td><img src=\"%s\"></td>") % filename).str();
				fhtml << boost::format(format) % c % t;
			}
			fhtml << "</tr>";

			// V
			fhtml << boost::format("\n<tr><th>Chromo[%d] V</th>") % c;
			for (auto t = 0u; t < Nt; t++) {
				fhtml << "<td>" << boost::format("%.6f") % intermediate->iv[t].value() << "</td>";
			}
			fhtml << "</tr>";
		}

		// Raw Fitness
		for (auto r = 0u; r < Nr; r++) {
			fhtml << boost::format("\n<tr><th>RawFitness[%d]</th>") % r;
			for (auto t = 0u; t < Nt; t++) {
				fhtml << "<td>" << boost::format("%.6f") % ind.rawFitness[r][t] << "</td>";
			}
			fhtml << "</tr>";
		}
		for (auto f = 0u; f < Nf; f++) {
			fhtml << boost::format("<th>Fitness[%d]</th>") % f;
			for (auto t = 0u; t < Nt; t++) {
				fhtml << "<td>" << boost::format("%.6f") % ind.fitness[f] << "</td>";
			}
			fhtml << "</tr>";
		}

		// Closing table.
		fhtml << "\n</tbody>"
			<< "\n</table>";
	};
	if (Nt > 0) saveHtmlTable(Nt, bestTask, "Training", "test_c%02u_t%02u.png");
	if (Nu > 0) saveHtmlTable(Nu, bestTest, "Testing",  "test_c%02u_u%02u.png");

	// Tree
	fhtml << "\n<h2>Tree</h2>"
		<< "\n<table class=\"chromosome\">";
	for (auto c = 0u; c < Nc; c++) {
		auto intermediate = bestTask.chromosome[c].getIntermediate<Garnet::IVTreeStrategy::IVTreeIntermediate>();
		fhtml << boost::format("\n<tr><th>Chromo[%d]</th><td>") % c << bestTask.chromosome[c] << "</td></tr>";
		fhtml << boost::format("\n<tr><th>Chromo[%d] IGraph</th><td>") % c << intermediate->igraph << "</td></tr>";
		fhtml << boost::format("\n<tr><th>Chromo[%d] VGraph</th><td>") % c << intermediate->vgraph << "</td></tr>";
	}
	fhtml << "\n</table>"
		  << "\n</body>"
		  << "\n</html>";
	fhtml.close();

	return 0;
}

}//end of anonymous namespace

int Garnet::executeReport(const strings& args)
{
	ConfigLoader::StringMap params = parseCommandLine(args);

	//
	strings types = parse(params["_ReportTypes"]);

	if (types.empty()) {
		throw "One or more report types must be specified.";
	}

	std::vector<uint32_t> typeNumbers;
	for_each(types.begin(), types.end(), [&typeNumbers](const std::string& type) {
		auto it = std::find(_types.begin(), _types.end(), type);
		if (it == _types.end()) {
			throw (boost::format("Report type %s is invalid.") % type).str();
		}

		typeNumbers.push_back(std::distance(_types.begin(), it));
	});

	//
	if (boost::equals(params["_IsProject"], "True")) {
		for_each(typeNumbers.begin(), typeNumbers.end(), [&params](const uint32_t type) {
			switch (type) {
			case REPORT_DETAIL :
				executeReportProjectDetail(params);
				break;
			case REPORT_FILTERS :
				executeReportProjectFilters(params);
				break;
			case REPORT_FITNESS :
				executeReportProjectFitness(params);
				break;
			case REPORT_FRONT :
				executeReportProjectFront(params);
				break;
			case REPORT_TEST :
				executeReportProjectTest(params);
				break;
			}
		});
	}
	else {
		for_each(typeNumbers.begin(), typeNumbers.end(), [&params](const uint32_t type) {
			switch (type) {
			case REPORT_DETAIL :
				executeReportCaseDetail(params);
				break;
			case REPORT_FILTERS :
				Garnet::Drivers::executeReport(params);
				//executeReportCaseFilters(params);
				break;
			case REPORT_FITNESS :
				executeReportCaseFitness(params);
				break;
			case REPORT_FRONT :
				executeReportCaseFront(params);
				break;
			case REPORT_TEST :
				executeReportCaseTest(params);
				break;
			}
		});
	}

	return 0;
}
