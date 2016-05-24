

#include <boost/format.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/graph/depth_first_search.hpp>

#include <Common/GetTime.h>
#include <Common/Text.h>
#include <Common/PrintRange.h>

#include <Garnet/Garnet.h>
#include <Garnet/Coloring.h>
#include <Garnet/TopologicalSort.h>
#include <Garnet/Dictionary.h>
#include <Garnet/PPEGraphProcessor.h>

Garnet::PPEGraphProcessor::PPEGraphProcessor(const std::string& type)
{
	_engine = PicturePerfect::PicturePerfectEngine::createInstance(type);

	setInformation<TopoSortMethod>(TOPOSORT_METHOD, TOPOSORT_DEFAULT);
}

Garnet::PPEGraphProcessor::~PPEGraphProcessor()
{
}

const std::string& Garnet::PPEGraphProcessor::generateScript(Garnet::PPEGraph& pg)
{
	StopWatch watch[3];

	// 1. Determine execution order by topological sort.
	watch[0].reset();
	std::vector<PPEGraph::vertex_descriptor> nodes;
	switch (getInformation<TopoSortMethod>(TOPOSORT_METHOD)) {
	case TOPOSORT_BGL:
		topological_sort(1, pg, std::back_inserter(nodes)); // BGL-standard topo sort
		break;
	case TOPOSORT_SARKER:
		topological_sort(-1, pg, std::back_inserter(nodes)); // Sarker-Serrano-Simmons (Sethi-Ulman)
		break;
	}
	watch[0].stop();

#if 0
	{
		// Show to which tree each vertex is belonged.
		std::cout << "\n[" __FUNCTION__ "] " << " #v:" << nodes.size() << std::flush;
		for (auto it = nodes.begin(); it != nodes.end(); it++) {
			std::cout << " " << pg[*it].tree;
		}
		std::cout << std::endl;
	}
#endif

	// 2. Color edges to determine register numbers.
	//    It is executed as a vertex coloring for a line graph,
	//    which is converted from the PPE graph.
	watch[1].reset();
#if 1
	// 2.1 Convert the PPE graph to a line graph.
	//auto l = dag::make_line_graph(pg);
	dag::line_graph_traits<Garnet::PPEGraph>::line_graph_type l;
	// 2.2 Perform coloring and reflect colors.
	auto num_colors = dag::edge_coloring(pg, nodes.cbegin(), nodes.cend(), l);
	{
		int32_t maxDegree = 0;
		for (auto it = vertices(l); it.first != it.second; it.first++) {
			int32_t degree = boost::degree(*it.first, l);
			if (maxDegree < degree) maxDegree = degree;
		}

#if 0
		std::cout << "\n[" __FUNCTION__ "] Dependence (Line) Graph ";
		std::cout << "\n[" __FUNCTION__ "]   # of vertices:   " << boost::num_vertices(l);
		std::cout << "\n[" __FUNCTION__ "]   # of edges:      " << boost::num_edges(l);
		std::cout << "\n[" __FUNCTION__ "]   max # of degree: " << maxDegree;
		std::cout << "\n[" __FUNCTION__ "]   # of colors:     " << num_colors;
		std::cout << std::flush;
#endif
	}
#if 0
	// 2.3 Reflect the coloring result to the PPE graph.
	auto range = vertices(l);
	reflectColors(pg, range.first, range.second, l);
	if (isProcessTrace()) {
#if defined(_DEBUG)
		std::cout << "\n-----" << std::flush;
		print_graph(l, get(&dag::line_graph_traits<Garnet::PPEGraph>::line_graph_type::vertex_property_type::label, l));
#endif
		std::cout << "\n-----" << std::flush;
		print_graph(l, get(&dag::line_graph_traits<Garnet::PPEGraph>::line_graph_type::vertex_property_type::color, l));
	}
#endif
#else
	// 2.1 Convert the PPE graph to a line graph.
	LineGraph l;
	convertToLineGraph(pg, nodes.begin(), nodes.end(), l);
	{
		int32_t maxDegree = 0;
		for (auto it = vertices(l); it.first != it.second; it.first++) {
			int32_t degree = boost::degree(*it.first, l);
			if (maxDegree < degree) maxDegree = degree;
		}

		std::cout << "\n[" __FUNCTION__ "] Dependence (Line) Graph ";
		std::cout << "\n[" __FUNCTION__ "]   # of vertices:   " << boost::num_vertices(l);
		std::cout << "\n[" __FUNCTION__ "]   # of edges:      " << boost::num_edges(l);
		std::cout << "\n[" __FUNCTION__ "]   max # of degree: " << maxDegree;
		std::cout << std::flush;
	}
	// 2.2 Perform coloring.
	boost::graph_traits<LineGraph>::vertices_size_type num_colors;
	num_colors = boost::sequential_vertex_coloring(l, get(&LineGraph::vertex_property_type::color, l));
	{//if ( false ) {
		std::cout << "\n[" __FUNCTION__ "]   # of colors:     " << num_colors;
		std::cout << std::flush;
	}
	// 2.3 Reflect the coloring result to the PPE graph.
	auto range = vertices(l);
	reflectColors(pg, range.first, range.second, l);
	if (isProcessTrace()) {
#if defined(_DEBUG)
		std::cout << "\n-----" << std::flush;
		print_graph(l, get(&LineGraph::vertex_property_type::label, l));
#endif
		std::cout << "\n-----" << std::flush;
		print_graph(l, get(&LineGraph::vertex_property_type::color, l));
	}
	// 2.4 Release line graph.
	l.clear();
#endif
	watch[1].stop();

	// 3. Print instrucitons in the topological order.
	//    The color of input edges corresponds to output register.
	//    The colors of output edges correnspond to input registers.
	std::stringstream script;
	watch[2].reset();
	if (isProcessTrace()) {
		std::cout << "\n-----" << std::flush;
	}
	for (auto it = nodes.begin(); it != nodes.end(); it++) {
		const PPEGraph::vertex_property_type& f = pg[*it];
		auto range = boost::out_edges(*it, pg);

		if (isProcessTrace()) {
			std::cout << (boost::format("%s[%u]\n") % f.label % f.color).str();
		}

		// 3.1 Intron removal.
		if (f.color == -1) {
			// Node without color is an intron.
			continue;
		}

		if (IS_SRC(f.type)) {
			// Source node is an intron.
			continue;
		}
#if 0
		if (range.first == range.second && (f.type & 0xF00) != 0x000) {
			// Node with no inputs is an intron.
			// except for color is -2, which is a special terminal. (i.e. Zero)
			continue;
		}
#endif
		// 3.3 Output instruction.
		char buf[12] = " 0x";
		// 3.3.1 Output opcode.
		script << f.label;
		// 3.3.2 Output output registers.
		//       Output register is expressed as inbound edge,
		//       however the color is copied to the vertex's color
		//       because PPEGraph is unidirected graph.
		itoa((f.type << 24) | f.color, buf + 3, 16);
		script << buf;
		// 3.3.3 Output input registers.
		//       Input registers are expressed as outboud edges.
		for (; range.first != range.second; range.first++) {
			const PPEGraph::vertex_property_type& f2 = pg[boost::target(*range.first, pg)];
			itoa((f2.type << 24) | f2.color, buf + 3, 16);
			script << buf;
		}

		// 3.4 Finish.
		script << "\n";
	}
	script << "End" << std::endl;
	if (isProcessTrace()) {
		std::cout << std::flush;
	}
	watch[2].stop();

	// TRACE
	if (isLocationTrace()) {
		std::cout << boost::format("\n[%s] Topological Sort .. %.6f[sec]") % __FUNCTION__ % watch[0].get();
		std::cout << boost::format("\n[%s] Edge Coloring ..... %.6f[sec]") % __FUNCTION__ % watch[1].get();
		std::cout << boost::format("\n[%s] Output Script ..... %.6f[sec]") % __FUNCTION__ % watch[2].get();
		std::cout << std::flush;
	}

	// Return result.
	return _script = script.str();
}
