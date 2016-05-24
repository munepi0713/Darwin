
#include <deque>
#include <algorithm>

#include <boost/format.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/topological_sort.hpp>

#include <Common/GetTime.h>
#include <Common/Text.h>
#include <Garnet/ThreadPool.h>

#include <Garnet/Coloring.h>
#include <Garnet/Dictionary.h>
#include <Garnet/Garnet.h>
#include <Garnet/IVGraphProcessor.h>
#include <Garnet/TopologicalSort.h>
#include <Garnet/TraceLevel.h>
#include <Garnet/UniteTrees.h>

namespace {


Garnet::IVGraph convertToIVGraph(const Garnet::IVTreeList& trees, Garnet::IVGraphProcessor::GroupMethod method)
{
	Garnet::IVGraph iv;

	StopWatch watch;
	switch (method) {
	case Garnet::IVGraphProcessor::GROUP_AS_FOREST:
		Garnet::uniteTreesToForest<Garnet::Tree::GraphType>(trees.begin(), trees.end(), iv, [](const Garnet::Tree::GraphType::vertex_property_type& src_p, Garnet::IVGraph::vertex_property_type& dst_p) {
			dst_p.label = src_p.label;
		});
		break;
	case Garnet::IVGraphProcessor::GROUP_AS_DAG:
	default:
		Garnet::uniteTreesToDag<Garnet::Tree::GraphType>(trees.begin(), trees.end(), iv, [](const Garnet::Tree::GraphType::vertex_property_type& src_p, Garnet::IVGraph::vertex_property_type& dst_p) {
			dst_p.label = src_p.label;
		});
		break;
	}
	watch.stop();

	ILOG("IVTrees to IVGraph %.6fs", watch.get());
    ILOG("  %u vertices.", boost::num_vertices(iv));
    ILOG("  %u edges.", boost::num_edges(iv));

	return iv;
}

Garnet::PPEGraph convertIVGraphToPPEGraph(const Garnet::IVGraph& iv)
{
	typedef Garnet::PPEGraph::vertex_descriptor  PVertex;
	typedef Garnet::PPEGraph::edge_descriptor    PEdge;

	Garnet::PPEGraph pg;

	StopWatch watch;

	// 1. Crack IV node into a set of PPE node.
	for (auto vpair = vertices(iv); vpair.first != vpair.second; vpair.first++) {
		auto u_iv = *vpair.first;
		Garnet::PPEGraph::vertex_descriptor u, v, w;
		if (Text::startsWith(iv[u_iv].label, "_IVStore")) {
			// 1.1 Root node.
			//     Add I, V and A nodes into PPE graph.
			uint32_t reg = strtoul(iv[u_iv].label.c_str() + strlen("_IVStore"), 0, 0);
			u = boost::add_vertex(pg); // I
			v = boost::add_vertex(pg); // V
			w = boost::add_vertex(pg); // A
			pg[u].label = "Move";
			pg[v].label = "Move";
			pg[w].label = "Move";
			pg[u].type = _RI1B;
			pg[v].type = _RV1D;
			pg[w].type = _RA1D;
			pg[u].color = reg;
			pg[v].color = reg;
			pg[w].color = -1;     // A node is intended to be an intron.
		}
		else if (Text::startsWith(iv[u_iv].label, "_VAStore")) {
			// 1.1 Root node.
			//     Add I, V and A nodes into PPE graph.
			uint32_t reg = strtoul(iv[u_iv].label.c_str() + strlen("_VAStore"), 0, 0);
			u = boost::add_vertex(pg);
			v = boost::add_vertex(pg);
			w = boost::add_vertex(pg);
			pg[u].label = "Move";
			pg[v].label = "Move";
			pg[w].label = "Move";
			pg[u].type = _RI1B;
			pg[v].type = _RV1D;
			pg[w].type = _RA1D;
			pg[u].color = -1;     // I node is intendd to be an intron.
			pg[v].color = reg;
			pg[w].color = reg;
		}
		else if (Text::startsWith(iv[u_iv].label, "_LoadColor")) {
			// 1.2 Source Terminal node. (I3B)
			//     Add I, V, and A nodes into PPE graph.
			uint32_t reg = strtoul(iv[u_iv].label.c_str() + strlen("_LoadColor"), 0, 0);
			u = boost::add_vertex(pg);
			v = boost::add_vertex(pg);
			w = boost::add_vertex(pg);
			pg[u].label = "Move";
			pg[v].label = "Move";
			pg[w].label = "Move";
			pg[u].type = _SI3B;
			pg[v].type = _SV1D;
			pg[w].type = _SA1D;
			pg[u].color = reg;
			pg[v].color = reg;
			pg[w].color = reg;
		}
		else if (Text::startsWith(iv[u_iv].label, "_Load")) {
			// 1.2 Source Terminal node. (I1B)
			//     Add I, V, and A nodes into PPE graph.
			uint32_t reg = strtoul(iv[u_iv].label.c_str() + strlen("_Load"), 0, 0);
			u = boost::add_vertex(pg);
			v = boost::add_vertex(pg);
			w = boost::add_vertex(pg);
			pg[u].label = "Move";
			pg[v].label = "Move";
			pg[w].label = "Move";
			pg[u].type = _SI1B;
			pg[v].type = _SV1D;
			pg[w].type = _SA1D;
			pg[u].color = reg;
			pg[v].color = reg;
			pg[w].color = reg;
		}
		else {
			// 1.2.1 Find an entry with this vertex's name from the dictionary.
			const Garnet::IVInstruction& dict = Garnet::Dictionary::findByIVName(iv[u_iv].label);

			// 1.2.2 Intermediate node or non-source terminal node.
			//       Add I, V, and A nodes into PPE graph.
			u = boost::add_vertex(pg);
			v = boost::add_vertex(pg);
			w = boost::add_vertex(pg);
			pg[u].label = dict.pp[0].label;
			pg[v].label = dict.pp[1].label;
			pg[w].label = dict.pp[2].label;
			pg[u].type = dict.pp[0].outputType;
			pg[v].type = dict.pp[1].outputType;
			pg[w].type = dict.pp[2].outputType;
			pg[u].color = -1;
			pg[v].color = -1;
			pg[w].color = -1;
		}

		//pg[u].tree = iv[u_iv].tree;
		//pg[v].tree = iv[u_iv].tree;
		//pg[w].tree = iv[u_iv].tree;
	}
	//std::cout << "\n" << __LINE__ << std::endl;

	// 2. Connect
	//    PPEGraph and IVGraph use vector as vertex and edge list storage,
	//    so these descriptors are integer values.
	//    A IV filter with index x corresponds to PPE filters index x*3+0, x*3+1 and x*3+2. 
	for (auto vpair = vertices(iv); vpair.first != vpair.second; vpair.first++) {
		auto u = *vpair.first;

	#define ADD_SINK_EDGE(ppType) {\
		Garnet::PPEGraph::vertex_descriptor v = MAKE_PP_INDEX(out_edges(u, iv).first->m_target, (ppType)); \
		while (Text::equals(pg[v].label, "Move") && out_degree(v, pg) > 0) {\
		v = out_edges(v, pg).first->m_target; \
		} \
		add_edge(MAKE_PP_INDEX(u, (ppType)), v, pg); \
		}

		// 2.2 Add edges.
		if (Text::startsWith(iv[u].label, "_IVStore")) {
			// 2.2.1 Add edges for Sink.
			//       Sink is cracked two Move operations with preassigned
			//       Result register number.
			//       No A edge for 'Sink'.
			ADD_SINK_EDGE(0); // I
			ADD_SINK_EDGE(1); // V
		}
		else if (Text::startsWith(iv[u].label, "_VAStore")) {
			// 2.2.2 Add edges for VASink.
			//       Sink is cracked two Move operations with preassigned
			//       Result register number.
			//       No I edge for VASink.
			ADD_SINK_EDGE(1); // V
			ADD_SINK_EDGE(2); // A
		}
		else if (Text::startsWith(iv[u].label, "_LoadColor") || Text::startsWith(iv[u].label, "_Load")){
			// Source vertices does not need connect any edges.
			continue;
		}
		else {
			// 2.2.5.1 Find an entry with this vertex's name from the dictionary.
			const Garnet::IVInstruction& dict = Garnet::Dictionary::findByIVName(iv[*vpair.first].label);

			// 2.2.5.2 Add edges for filters by following dictionary.
			for (auto t = 0; t < NUM_PP_PER_FILTER; t++) {
				// t=0 -> I, t=1 -> V, t=2 -> A.
				// Add edges for inputs.
				for (auto i = 0u; dict.pp[t].input[i] != _NULL; i++) {
					Garnet::IVGraph::vertex_descriptor ipos = (out_edges(u, iv).first + dict.pp[t].input[i] / NUM_PP_PER_FILTER)->m_target;
					int                        type = dict.pp[t].input[i] % NUM_PP_PER_FILTER;
					// Skip "Move" nodes in PPE graph.
					// If terminal node is found, end search and connect to it.
					while (Text::equals(pg[MAKE_PP_INDEX(ipos, type)].label, "Move") && pg[MAKE_PP_INDEX(ipos, type)].color == -1) {
						const Garnet::IVInstruction* dictCur = &Garnet::Dictionary::findByIVName(iv[ipos].label);
						ipos = (out_edges(ipos, iv).first + dictCur->pp[type].input[0] / NUM_PP_PER_FILTER)->m_target;
						type = dictCur->pp[type].input[0] % NUM_PP_PER_FILTER;
					}

					// Add an edge.
					add_edge(MAKE_PP_INDEX(u, t), MAKE_PP_INDEX(ipos, type), pg);
				}
			}
		}

	#undef ADD_SINK_EDGE
	}

	// 3. Remove introns.
	//      Remove all vertices under non-Sink roots.
	// 3.1 Find sink roots.
	std::stack<PVertex> q;    // Non-intron vertices.
	std::set<PVertex> mark_v; // A vertex is an intron if it is not in mark_v.
	std::set<PEdge>   mark_e; // An edge is an intron if it is not in mark_e.
	auto pg_vertices = vertices(pg);
	auto pg_edges = edges(pg);
	for (auto it = pg_vertices.first; it != pg_vertices.second; it++) {
		if (IS_RES(pg[*it].type) && pg[*it].color != -1) {
			// move instruction for sink.
			mark_v.insert(*it);
			q.push(*it);
		}
	}
	// 3.2 Mark vertices and edges directly or indirectly connected to sink roots.
	while (!q.empty()) {
		PVertex pg_u = q.top();
		q.pop();

		// Mark the vertex as a non-intron.
		mark_v.insert(pg_u);

		// Mark edges from this vertex as non-introns.
		auto edges = out_edges(pg_u, pg);
		for_each(edges.first, edges.second, [&](const PEdge& pg_e) {
			// Mark it is an important edge.
			mark_e.insert(pg_e);
			q.push(pg_e.m_target);
		});
	}
	// 3.3 Make a new PPEGraph by copying V and E from the orignal except introns.
	Garnet::PPEGraph pg_new;
	std::map<PVertex, PVertex> mapper; // <K,V>=<pg vertex, pg_new vertex>
	for_each(mark_v.begin(), mark_v.end(), [&](const PVertex& pg_u) {
		PVertex pg_new_u = add_vertex(pg_new);
		pg_new[pg_new_u].label = pg[pg_u].label;
		pg_new[pg_new_u].type = pg[pg_u].type;
		pg_new[pg_new_u].color = pg[pg_u].color;
		mapper[pg_u] = pg_new_u;
	});
	for_each(mark_v.begin(), mark_v.end(), [&](const PVertex& pg_u) {
		auto edges = out_edges(pg_u, pg);
		for_each(edges.first, edges.second, [&](const PEdge& pg_e) {
			add_edge(mapper[pg_e.m_source], mapper[pg_e.m_target], pg_new);
		});
	});
	// 3.4 Swap.
	pg.swap(pg_new);

	//trace.write("convertIVGraphToPPEGraph", "IVGraph to PPEGraph", "PPEGraph", pg);

	watch.stop();

	if (Garnet::isLocationTrace()) {
		std::cout << boost::format("\n[" __FUNCTION__ "] IVGraph to PPEGraph %.6fs") % watch.get();
		std::cout << boost::format("\n[" __FUNCTION__ "]   # of vertices: %6u") % num_vertices(pg);
		std::cout << boost::format("\n[" __FUNCTION__ "]   # of edges:    %6u") % num_edges(pg);
		std::cout << std::flush;
	}

	if (Garnet::isProcessTrace()) {
		print_graph(pg, get(&Garnet::PPEGraph::vertex_property_type::label, pg));
		std::cout << "\n-----" << std::flush;
	}

	return pg;
}

std::string generateScript(Garnet::PPEGraph& pg, Garnet::IVGraphProcessor::TopoSortMethod method, InfoContainer* info)
{
	StopWatch watch[3];

	// 1. Determine execution order by topological sort.
	watch[0].reset();
	std::vector<Garnet::PPEGraph::vertex_descriptor> nodes;
	switch (method) {
	case Garnet::IVGraphProcessor::TOPOSORT_BGL:
		topological_sort(1, pg, std::back_inserter(nodes)); // BGL-standard topo sort
		break;
	case Garnet::IVGraphProcessor::TOPOSORT_SARKAR:
		topological_sort(-1, pg, std::back_inserter(nodes)); // Sarker-Serrano-Simmons (Sethi-Ulman)
		break;
	}
	watch[0].stop();

    // 2. Color edges to determine register numbers.
	//    It is executed as a vertex coloring for a line graph,
	//    which is converted from the PPE graph.
	watch[1].reset();
	// 2.1 Convert the PPE graph to a line graph.
	//auto l = dag::make_line_graph(pg);
	dag::line_graph_traits<Garnet::PPEGraph>::line_graph_type l;
	// 2.2 Perform coloring and reflect colors.
	auto num_colors = dag::edge_coloring(pg, nodes.cbegin(), nodes.cend(), l);
    if (info) info->setInformation<int>(0, getMaxDegree(l));
	watch[1].stop();

	// 3. Print instrucitons in the topological order.
	//    The color of input edges corresponds to output register.
	//    The colors of output edges correnspond to input registers.
	std::stringstream script;
	watch[2].reset();
	if (Garnet::isProcessTrace()) {
		std::cout << "\n-----" << std::flush;
	}
	for (auto it = nodes.begin(); it != nodes.end(); it++) {
		const Garnet::PPEGraph::vertex_property_type& f = pg[*it];
		auto range = boost::out_edges(*it, pg);

		if (Garnet::isProcessTrace()) {
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
			const Garnet::PPEGraph::vertex_property_type& f2 = pg[boost::target(*range.first, pg)];
			itoa((f2.type << 24) | f2.color, buf + 3, 16);
			script << buf;
		}

		// 3.4 Finish.
		script << "\n";
	}
	script << "End" << std::endl;
	if (Garnet::isProcessTrace()) {
		std::cout << std::flush;
	}
	watch[2].stop();

	// TRACE
	ILOG("  Topological Sort .. %.6f[sec]", watch[0].get());
	ILOG("  Edge Coloring ..... %.6f[sec]", watch[1].get());
	ILOG("  Output Script ..... %.6f[sec]", watch[2].get());

	// Return result.
	return script.str();
}

}// end of local namespace

Garnet::IVGraphProcessor::IVGraphProcessor()
{
}

Garnet::IVGraphProcessor::~IVGraphProcessor()
{

}

Garnet::IVTreeList Garnet::IVGraphProcessor::resolveAlias(const IVTreeList& trees, const AliasDictionary& dictionary, InfoContainer& info) {
    Garnet::IVTreeList resultTrees;
    double lap = WATCH_DO([&]() {
        for (Tree tree : trees) {
            resultTrees.push_back(AliasResolver::resolve(tree, dictionary));
        }
    });
    info.setInformation<double>(INFO_COMPILE_TIME, lap);
    return resultTrees;
}

Garnet::IVTreeList Garnet::IVGraphProcessor::resolveStatement(const IVTreeList& trees, const StatementDictionary& dictionary, InfoContainer& info) {
    Garnet::IVTreeList resultTrees;
    double lap = WATCH_DO([&]() {
        for (Tree tree : trees) {
            resultTrees.push_back(StatementResolver::resolve(tree, dictionary));
        }
    });
    info.setInformation<double>(INFO_COMPILE_TIME, lap);
    return resultTrees;
}

Garnet::IVGraph Garnet::IVGraphProcessor::convertIVTreesToIVGraph(const IVTreeList& trees, GroupMethod method, InfoContainer& info) {
    Garnet::IVGraph ivg;
    double lap = WATCH_DO([&]() {
        ivg = convertToIVGraph(trees, method);
    });
    info.setInformation<double>(INFO_COMPILE_TIME, lap);
    return ivg;
}

Garnet::PPEGraph Garnet::IVGraphProcessor::convertIVGraphToPPEGraph(const IVGraph& ivg, InfoContainer& info) {
    Garnet::PPEGraph pg;
    double lap = WATCH_DO([&]() {
        ::convertIVGraphToPPEGraph(ivg);
    });
    info.setInformation<double>(INFO_COMPILE_TIME, lap);
    return pg;
}

Garnet::PPEGraph Garnet::IVGraphProcessor::removeIntrons(const PPEGraph& pg, InfoContainer& info) {
    return pg;
}

std::string Garnet::IVGraphProcessor::generateScript(const PPEGraph& pg, TopoSortMethod method, InfoContainer& info) {
    std::string script;
    double lap = WATCH_DO([&]() {
        std::string  script = generateScript(pg, method);
    });
    info.setInformation<double>(INFO_COMPILE_TIME, lap);
    return script;
}

PicturePerfect::ExecutablePtr Garnet::IVGraphProcessor::compile(PicturePerfect::CompilerPtr compiler, std::istream& ins, InfoContainer& info) {
    PicturePerfect::ExecutablePtr executable;
    double lap = WATCH_DO([&]() {
        executable = compiler->compile(ins);
    });
    info.setInformation<double>(INFO_COMPILE_TIME, lap);
    return executable;
}

void Garnet::IVGraphProcessor::execute(PicturePerfect::PicturePerfectEnginePtr ppe, const PicturePerfect::ExecutablePtr& executable, const PicturePerfect::ArchivePtr& source, PicturePerfect::ArchivePtr result, InfoContainer& info)
{
	double lap = WATCH_DO([&]() {
		ppe->execute(executable, source, result);
	});
    info.setInformation<double>(INFO_EXECUTE_TIME, lap);
}

std::vector<Garnet::IVTreeList> Garnet::IVGraphProcessor::makeIVTreeGroups(const IVTreeList& trees, int numVerticesLimit, InfoContainer& info) {
    std::vector<Garnet::IVTreeList> treeGroups;
    double lap = WATCH_DO([&]() {
        treeGroups.push_back(std::vector<Tree>());
        auto numVertices = 0u;
        for (auto it = trees.cbegin(); it != trees.cend(); it++) {
            auto i = it - trees.cbegin();

            if (numVertices >= numVerticesLimit) {
                treeGroups.push_back(std::vector<Tree>());
                numVertices = 0;
            }
            treeGroups.back().push_back(*it);
            numVertices += it->getNumVertices();
        }
        std::cout << ".";
    });
    info.setInformation<double>(INFO_EXECUTE_TIME, lap);
    return treeGroups;
}

#if 0
std::map<uint32_t, Garnet::IVGraphInfo> Garnet::IVGraphProcessor::getLastTreeInfo() const
{
	std::map<uint32_t, Garnet::IVGraphInfo> ret;

	// [Log] Save intermediate PPEGraph.
	for (auto it = _info.begin(); it != _info.end(); it++) {
		const auto& pg = it->pg;
		auto range = boost::vertices(pg);
		for (auto vt = range.first; vt != range.second; vt++) {
			auto& prop = pg[*vt];
			if (IS_RES(prop.type)) { // Is a sink node?
				std::string pgraph = Garnet::getSExpression<std::string>(*vt, pg, [](const Garnet::PPEGraph::vertex_descriptor& u, const Garnet::PPEGraph& g) -> const std::string{
				switch (g[u].type) {
				case _SI1B:
					return (boost::format("SI1B[%u]") % g[u].color).str();
				case _SI3B:
					return (boost::format("SI3B[%u]") % g[u].color).str();
				case _SV1D:
					return (boost::format("SV1D[%u]") % g[u].color).str();
				case _SA1D:
					return (boost::format("SA1D[%u]") % g[u].color).str();
				case _RI1B:
					return (boost::format("RI1B[%u]") % g[u].color).str();
				case _RI3B:
					return (boost::format("RI3B[%u]") % g[u].color).str();
				case _RV1D:
					return (boost::format("RV1D[%u]") % g[u].color).str();
				case _RA1D:
					return (boost::format("RA1D[%u]") % g[u].color).str();
				default:
					return g[u].label;
				}
			});

				auto& info = ret[prop.color];
				info.group = (it - _info.begin());
				switch (prop.type) {
				case _RI1B:
					info.igraph = pgraph;
					break;
				case _RV1D:
					info.vgraph = pgraph;
					break;
				case _RA1D:
					info.agraph = pgraph;
					break;
				}
			}
		}
	}

	return ret;
}
#endif
