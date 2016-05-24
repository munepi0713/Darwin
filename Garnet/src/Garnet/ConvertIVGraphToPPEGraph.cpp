
#include <algorithm>

#include <boost/format.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

#include <Common/GetTime.h>
#include <Common/Text.h>

#include <Garnet/Garnet.h>
#include <Garnet/Dictionary.h>

void Garnet::convertIVGraphToPPEGraph(Garnet::IVGraph& iv, Garnet::PPEGraph& pg)
{
	ConfigStore conf;
	TraceStore trace;

	conf.write("/convertIVGraphToPPEGraph/Remove Introns/ShouldRemoveIntrons", true);

	convertIVGraphToPPEGraph(iv, pg, conf, trace);
}

void Garnet::convertIVGraphToPPEGraph(Garnet::IVGraph& iv, Garnet::PPEGraph& pg, const ConfigStore& conf, TraceStore& trace)
{
	typedef PPEGraph::vertex_descriptor  PVertex;
	typedef PPEGraph::edge_descriptor    PEdge;

	StopWatch watch;

	// 1. Crack IV node into a set of PPE node.
	for (auto vpair = vertices(iv); vpair.first != vpair.second; vpair.first++) {
		auto u_iv = *vpair.first;
		PPEGraph::vertex_descriptor u, v, w;
		if ( Text::startsWith(iv[u_iv].label, "_IVStore") ) {
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
		else if ( Text::startsWith(iv[u_iv].label, "_VAStore") ) {
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
		else if ( Text::startsWith(iv[u_iv].label, "_Src") ) {
			// 1.2 Source Terminal node.
			//     Add I, V, and A nodes into PPE graph.
			uint32_t reg = strtoul(iv[u_iv].label.c_str() + strlen("_Src"), 0, 0);
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
		else if ( Text::startsWith(iv[u_iv].label, "_Load") ) {
			// 1.2 Source Terminal node.
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
					PPEGraph::vertex_descriptor v = MAKE_PP_INDEX(out_edges(u, iv).first->m_target, (ppType)); \
					while ( Text::equals(pg[v].label, "Move") && out_degree(v, pg) > 0 ) { \
						v = out_edges(v, pg).first->m_target;\
					} \
					add_edge(MAKE_PP_INDEX(u, (ppType)), v, pg);\
				}

		// 2.2 Add edges.
		if ( Text::startsWith(iv[u].label, "_IVStore") ) {
			// 2.2.1 Add edges for Sink.
			//       Sink is cracked two Move operations with preassigned
			//       Result register number.
			//       No A edge for 'Sink'.
			ADD_SINK_EDGE(0); // I
			ADD_SINK_EDGE(1); // V
		}
		else if ( Text::startsWith(iv[u].label, "_VAStore") ) {
			// 2.2.2 Add edges for VASink.
			//       Sink is cracked two Move operations with preassigned
			//       Result register number.
			//       No I edge for VASink.
			ADD_SINK_EDGE(1); // V
			ADD_SINK_EDGE(2); // A
		}
		else if ( Text::startsWith(iv[u].label, "_Src") || Text::startsWith(iv[u].label, "_Load") ){
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
					IVGraph::vertex_descriptor ipos = (out_edges(u, iv).first + dict.pp[t].input[i] / NUM_PP_PER_FILTER)->m_target;
					int                        type = dict.pp[t].input[i] % NUM_PP_PER_FILTER;
					// Skip "Move" nodes in PPE graph.
					// If terminal node is found, end search and connect to it.
					while ( Text::equals(pg[MAKE_PP_INDEX(ipos, type)].label, "Move") && pg[MAKE_PP_INDEX(ipos, type)].color == -1 ) {
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
	if ( conf.read<bool>("/convertIVGraphToPPEGraph/Remove Introns/ShouldRemoveIntrons") ) {
		// 3.1 Find sink roots.
		std::stack<PVertex> q;    // Non-intron vertices.
		std::set<PVertex> mark_v; // A vertex is an intron if it is not in mark_v.
		std::set<PEdge>   mark_e; // An edge is an intron if it is not in mark_e.
		auto pg_vertices = vertices(pg);
		auto pg_edges = edges(pg);
		for (auto it = pg_vertices.first; it != pg_vertices.second; it++) {
			if ( IS_RES(pg[*it].type) && pg[*it].color != -1 ) {
				// move instruction for sink.
				mark_v.insert(*it);
				q.push(*it);
			}
		}
		// 3.2 Mark vertices and edges directly or indirectly connected to sink roots.
		while ( !q.empty() ) {
			PVertex pg_u = q.top();
			q.pop();

			// Mark the vertex as a non-intron.
			mark_v.insert(pg_u);
		
			// Mark edges from this vertex as non-introns.
			auto edges = out_edges(pg_u, pg);
			for_each(edges.first, edges.second, [&] (const PEdge& pg_e) {
				// Mark it is an important edge.
				mark_e.insert(pg_e);
				q.push(pg_e.m_target);
			});
		}
		// 3.3 Make a new PPEGraph by copying V and E from the orignal except introns.
		PPEGraph pg_new;
		std::map<PVertex, PVertex> mapper; // <K,V>=<pg vertex, pg_new vertex>
		for_each(mark_v.begin(), mark_v.end(), [&] (const PVertex& pg_u) {
			PVertex pg_new_u = add_vertex(pg_new);
			pg_new[pg_new_u].label = pg[pg_u].label;
			pg_new[pg_new_u].type  = pg[pg_u].type;
			pg_new[pg_new_u].color = pg[pg_u].color;
			mapper[pg_u] = pg_new_u;
		});
		for_each(mark_v.begin(), mark_v.end(), [&] (const PVertex& pg_u) {
			auto edges = out_edges(pg_u, pg);
			for_each(edges.first, edges.second, [&] (const PEdge& pg_e) {
				add_edge(mapper[pg_e.m_source], mapper[pg_e.m_target], pg_new);
			});
		});
		// 3.4 Swap.
		pg.swap(pg_new);
	}

	trace.write("convertIVGraphToPPEGraph", "IVGraph to PPEGraph", "PPEGraph", pg);
	
	watch.stop();

	if ( isLocationTrace() ) {
		std::cout << boost::format("\n[" __FUNCTION__ "] IVGraph to PPEGraph %.6fs") % watch.get();
		std::cout << boost::format("\n[" __FUNCTION__ "]   # of vertices: %6u") % num_vertices(pg);
		std::cout << boost::format("\n[" __FUNCTION__ "]   # of edges:    %6u") % num_edges(pg);
		std::cout << std::flush;
	}

	if ( isProcessTrace() ) {
		print_graph(pg, get(&PPEGraph::vertex_property_type::label, pg));
		std::cout << "\n-----" << std::flush;
	}
}
