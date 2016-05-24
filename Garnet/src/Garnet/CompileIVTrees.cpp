
#include <boost/format.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

#include <Common/GetTime.h>
#include <Common/Text.h>

#include <Garnet/Garnet.h>
#include <Garnet/IVTreeStrategy.h>

PicturePerfect::ExecutablePtr Garnet::IVTreeStrategy::compileIVTrees(PicturePerfect::PicturePerfectEnginePtr ppe, const std::vector<Garnet::Tree>& trees, Garnet::CompileIVTreesTraceInfo* info)
{
	ConfigStore conf;
	TraceStore trace;

	conf.write("/convertIVTreesToIVGraph/IVTrees to IVGraph/GroupingMethod", IV_GRAPH_AS_DAG);
	conf.write("/convertIVGraphToPPEGraph/Remove Introns/ShouldRemoveIntrons", true);
	conf.write("/generateScript/Generate Script/TopologicalSortMethod", TOPOSORT_DEFAULT);

	PicturePerfect::ExecutablePtr executable = compileIVTrees(ppe, trees, conf, trace);

	if ( info ) {
		info->iv = trace.read<IVGraph>("/compileIVTrees/IVTrees to IVGraph/IVGraph");
	}

	return executable;
}

PicturePerfect::ExecutablePtr Garnet::IVTreeStrategy::compileIVTrees(PicturePerfect::PicturePerfectEnginePtr ppe, const std::vector<Garnet::Tree>& trees, const Garnet::ConfigStore& conf, Garnet::TraceStore& trace)
{
#define PROCESS_TRACE(statement) if ( isProcessTrace() ) statement;
	PROCESS_TRACE(std::cout << "[" __FUNCTION__ "] Begin." << std::endl);

	IVGraph iv;
	PPEGraph pg;
	std::stringstream ppscript;
	StopWatch watches[4];

	// 1. Convert IVTrees to IVGraph.
	watches[0].reset();
	Garnet::convertIVTreesToIVGraph(trees, iv, conf, trace);
	watches[0].stop();
#if 1
	trace.write("compileIVTrees", "IVTrees to IVGraph", "IVGraph", iv);
	trace.write("compileIVTrees", "IVTrees to IVGraph", "Time", watches[0].get());
#elif 0
	if ( info ) {
		info->iv = iv;
	}
#else
	if ( isProcessTrace() ) {
		std::cout << "-----\n";
		boost::print_graph(iv, get(&IVGraph::vertex_property_type::label, iv));
	}
#endif

	// 2. Convert IVGraph to PPEGraph.
	watches[1].reset();
	Garnet::convertIVGraphToPPEGraph(iv, pg, conf, trace);
	watches[1].stop();
#if 1
	trace.write("compileIVTrees", "IVGraph to PPEGraph", "PPEGraph", pg);
	trace.write("compileIVTrees", "IVGraph to PPEGraph", "Time", watches[1].get());
#elif 0
	if ( info ) {
		info->pg = pg;
	}
#else
	if ( isProcessTrace() ) {
		std::cout << "-----\n";
		boost::print_graph(pg, get(&PPEGraph::vertex_property_type::label, pg));
	}
#endif

	// 3. Generate script.
	watches[2].reset();
	Garnet::generateScript(pg, ppscript, conf, trace);
	watches[2].stop();
#if 1
	trace.write("compileIVTrees", "Generate Script", "Script", ppscript.str());
	trace.write("compileIVTrees", "Generate Script", "Time", watches[2].get());
#elif 0
	if ( info ) {
		info->script = ppscript.str();
	}
#endif
#if 1
	if ( isProcessTrace() ) {
		std::cout << "-----\n" << ppscript.str() << std::endl;
	}
	if ( isScriptTrace() ) {
		Garnet::getTraceStream() << ppscript.str() << std::flush;
	}
#endif

	// 4. Compile
	watches[3].reset();
	PicturePerfect::ExecutablePtr executable = ppe->compile(ppscript);
	watches[3].stop();
	trace.write("compileIVTrees", "Compile", "Time", watches[3].get());

	// A. Show information.
	if ( isLocationTrace() ) {
		std::cout << boost::format("\n[%s] Convert IVTrees to IVGraph  %.6fs") % __FUNCTION__ % watches[0].get();
		std::cout << boost::format("\n[%s] Convert IVGraph to PPEGraph %.6fs") % __FUNCTION__ % watches[1].get();
		std::cout << boost::format("\n[%s] Generate script             %.6fs") % __FUNCTION__ % watches[2].get();
		std::cout << boost::format("\n[%s] Compile                     %.6fs") % __FUNCTION__ % watches[3].get();
		std::cout << std::endl;
	}

	PROCESS_TRACE(std::cout << "[" __FUNCTION__ "] End." << std::endl);

	return executable;
}

