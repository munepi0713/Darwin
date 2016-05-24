
#include <deque>

#include <boost/format.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/topological_sort.hpp>

#include <Common/GetTime.h>
#include <Common/Text.h>

#include <Garnet/Garnet.h>
#include <Garnet/IVTree.h>
#include <Garnet/UniteTrees.h>

void Garnet::convertIVTreesToIVGraph(const std::vector<Garnet::Tree>& trees, Garnet::IVGraph& iv, unsigned method)
{
	ConfigStore conf;
	TraceStore trace;

	conf.write("/convertIVTreesToIVGraph/IVTrees to IVGraph/GroupingMethod", method);

	convertIVTreesToIVGraph(trees, iv, conf, trace);
}

void Garnet::convertIVTreesToIVGraph(const std::vector<Garnet::Tree>& trees, Garnet::IVGraph& iv, const ConfigStore& conf, TraceStore& trace)
{
	StopWatch watch;
	switch ( conf.read<unsigned>("/convertIVTreesToIVGraph/IVTrees to IVGraph/GroupingMethod") ) {
	case IV_GRAPH_AS_FOREST :
		uniteTreesToForest<Garnet::Tree::GraphType>(trees.begin(), trees.end(), iv, [] (const Garnet::Tree::GraphType::vertex_property_type& src_p, Garnet::IVGraph::vertex_property_type& dst_p) {
			dst_p.label = src_p.label;
		});
		break;
	case IV_GRAPH_AS_DAG :
	default :
		uniteTreesToDag<Garnet::Tree::GraphType>(trees.begin(), trees.end(), iv, [] (const Garnet::Tree::GraphType::vertex_property_type& src_p, Garnet::IVGraph::vertex_property_type& dst_p) {
			dst_p.label = src_p.label;
		});
		break;
	}
	watch.stop();
	trace.write("convertIVTreesToIVGraph", "IVTrees to IVGraph", "Time", watch.get());

	if ( isLocationTrace() ) {
		std::cout << boost::format("\n[%s] IVTrees to IVGraph %.6fs") % __FUNCTION__ % watch.get();
		std::cout << boost::format("\n[%s]   %u vertices.") % __FUNCTION__ % num_vertices(iv);
		std::cout << boost::format("\n[%s]   %u edges.") % __FUNCTION__ % num_edges(iv);
		std::cout << std::flush;
	}
}

