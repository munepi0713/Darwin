
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/graph/graphml.hpp>

#include <Common/ConfigLoader.h>
#include <Common/GetTime.h>
#include <Common/Text.h>
#include <Common/PrintRange.h>

#include <Garnet/Affinity.h>
#include <Garnet/Drivers.h>
#include <Garnet/Garnet.h>
#include <Garnet/Dictionary.h>
#include <Garnet/IVTreeStrategy.h>

#include <PicturePerfectEngine/PicturePerfectEngine.h>
#include <PicturePerfectEngine/Archive.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

namespace {


}//end of local namespace


/**
 *
 */
void Garnet::Drivers::executeDetail(StringMap& args)
{
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
	if ( infilename.empty() || outfilename.empty() ) {
		throw "usage: Garnet -m detail <conf file> -i <generation log> -o <output name> -d <output directory>";
	}

	// 2. Load generation file.
	Garnet::Generation generation;

	// 2.1
	{
		std::ifstream fin(infilename.c_str());
		if ( fin.fail() ) {
			throw std::string("[" __FUNCTION__ "] E2: Failed to open the input file ") + infilename + ".";
		}

		generation = Generation::createFromFile(fin);
	}

	// 2.2 Remove trees except the best one. 
	generation.individuals.resize(1);

	// 3. Make a report
	auto& g = generation.individuals[0].chromosome[0].getGraph();

	// 3.1 Create directory.
	std::string dir = (boost::format("%s%s") % conf.dir % outfilename).str();
	boost::filesystem::create_directories(dir);

	// 3.2 Save graph as GraphML format.
	{
		boost::dynamic_properties dp;
		dp.property("name", get(&Garnet::Tree::VertexProp::label, g));

		std::ofstream fout((boost::format("%s/%s.graphml") % dir % outfilename).str());
		boost::write_graphml(fout, g, dp);
		fout.close();
	}

	// 3.3 Save intermediate result for all nodes.
	//     by executing sub-trees rooted by every nodes, excluding terminal nodes.
	Generation gtemp;
	for (auto n = 0u; n < boost::num_vertices(g); n++) {
		std::string sexp = Garnet::getSExpression<std::string>(boost::vertex(n, g), g);
		//std::cout << boost::format("\n%u: %s") % n % sexp;

		Tree tree(sexp);

		Individual ind;
		ind.chromosome.push_back(tree);
		gtemp.individuals.push_back(ind);
	}

	uint32_t Ni = gtemp.individuals.size();
	uint32_t Nt = conf.tasks.size();

	{
		conf.numThreads.clear();
		conf.numThreads.push_back(1);
		conf.numVerticesLimit = 50000;
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

	std::stringstream svalue;
	ga->evaluate(gtemp);

	for (auto n = 0u; n < boost::num_vertices(g); n++) {
		if ( boost::out_degree(boost::vertex(n, g), g) == 0 ) {
			// Skip terminal nodes.
			continue;
		}
		
		svalue << boost::format("%05u,") % n;

		for (auto t = 0u; t < Nt; t++) {
			std::string fn = (boost::format("%s/n%06u_C%02u_T%03u.png") % dir % n % 0 % t).str();
			const IVTreeStrategy::IVTreeIntermediatePtr& result = std::dynamic_pointer_cast<IVTreeStrategy::IVTreeIntermediate>(gtemp.individuals[n].chromosome[0].intermediate);
			cv::imwrite(fn, result->iv[t].image());
			svalue << boost::format("%.6f,") % result->iv[t].value();
		}
		svalue << "\n";
	}
	std::ofstream fvalue((boost::format("%s/value.csv") % dir).str());
	fvalue << svalue.str();
	fvalue.close();

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
