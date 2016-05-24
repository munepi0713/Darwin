#ifndef GARNET__COLORING__H__
#define GARNET__COLORING__H__

#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/graph/depth_first_search.hpp>


#define GARNET_DEBUG

namespace dag {

template <typename G>
struct VProp {
    std::vector<typename boost::graph_traits<G>::edge_descriptor> e;
    int16_t  color;
#if defined(GARNET_DEBUG)
    std::string label;
#endif
};

template <typename G, typename L, typename It>
void  reflectColors(G& g, It first, It last, const L& l) {
    typedef typename boost::graph_traits<G>::edge_descriptor   GEdge;

	for (auto it = first; it != last; it++) {
		auto l_prop = l[*it];
		for_each(l_prop.e.begin(), l_prop.e.end(), [&] (GEdge g_e) {
			auto& targetColor = g[boost::target(g_e, g)].color;
			if ( targetColor == -1 ) {
				targetColor = l_prop.color;
			}
		});
	}
}

template <typename G, typename Iterator, typename L>
void  convertToLineGraph(const G& g, const Iterator& vfirst, const Iterator& vlast, L& l) {
    typedef typename boost::graph_traits<G>::vertex_descriptor GVertex;
    typedef typename boost::graph_traits<G>::edge_descriptor   GEdge;

    typedef typename boost::graph_traits<L>::vertex_descriptor LVertex;
    typedef typename boost::graph_traits<L>::edge_descriptor   LEdge;

	std::map<GVertex, std::vector<GEdge> > g_edgegroups;
    std::map<GEdge, LVertex>               g2l; // G edge to L vertex.
	std::map<GVertex, GEdge>               g_v2e; // G edge directed to G vertex.
	
	auto g_edges    = boost::edges(g);
	auto g_vertices = boost::vertices(g);
	
	// 1. Group edges in G by those target vertices. (A target is a vertex near leaf.)
	std::for_each(g_edges.first, g_edges.second, [&] (const GEdge& e) {
		auto target = boost::target(e, g);
		g_edgegroups[target].push_back(e);
		g_v2e.insert(std::make_pair(target, e));
	});
	
	// 2. Add vertices into L, corresponding to G edge groups.
	for (auto it = g_edgegroups.begin(); it != g_edgegroups.end(); it++) {
		GVertex g_u = it->first;
		LVertex l_u = boost::add_vertex(l);
		l[l_u].e = it->second;
		
		//
		#if defined(GARNET_DEBUG)
		std::vector<std::string> temp;
		for_each(it->second.begin(), it->second.end(), [&temp, &g] (const GEdge& e) {
			temp.push_back((boost::format("%s->%s") 
				% g[boost::source(e, g)].label
				% g[boost::target(e, g)].label).str());
		});
        l[l_u].label = std::string("(") + Text::join(temp.begin(), temp.end(), ",") + std::string(")");
		#endif

		for_each(it->second.begin(), it->second.end(), [&g2l, l_u] (const GEdge& e) {
			g2l[e] = l_u;
		});
	}
	
	// 3. In the topological order based on Sethi-Ullman numbering,
	//    scan all vertices in G to make edges in L which corresponding to 
	//    dependencies at the timing that a vertex in G is executed.
	std::map<LVertex, uint32_t> live_regs; // <K,V>=<reg# (i.e. group#), reference count>
    std::set<std::pair<LVertex, LVertex> > processed;
	std::for_each(vfirst, vlast, [&] (GVertex g_u) {
		std::set<LVertex> dependencies;
		
		// 3.1 Add all live registers into dependencies. 
		std::for_each(live_regs.begin(), live_regs.end(), [&dependencies] (const std::pair<LVertex, uint32_t>& x) {
			dependencies.insert(x.first);
		});
		
		// 3.2 Add source registers (i.e. output edges from the vertex)  into dependencies.
		auto g_outedges = boost::out_edges(g_u, g);
		std::for_each(g_outedges.first, g_outedges.second, [&dependencies, &g2l, &live_regs] (const GEdge& g_e) {
			LVertex l_u = g2l[g_e];
			dependencies.insert(l_u);
			if ( --live_regs[l_u] == 0 ) {
				live_regs.erase(l_u);
			}
		});
		
		// 3.3 Add result registers (i.e. input edges to the vertex) into dependencies,
		//     and set its reference count as same number as the number of the group members.
		auto g_inedges = g_v2e.equal_range(g_u);
		std::for_each(g_inedges.first, g_inedges.second, [&dependencies, &g2l, &live_regs, &g_edgegroups, g_u] (const std::pair<GVertex, GEdge>& g_e) {
			LVertex l_u = g2l[g_e.second];
			dependencies.insert(l_u);
			live_regs.insert(std::make_pair(l_u, static_cast<uint32_t>(g_edgegroups[g_u].size())));
		});

		// 3.4 Make edges in L for all combinations of vertices in the dependencies.
		auto deprange = std::make_pair(dependencies.begin(), dependencies.end());
        for (auto i = deprange.first; i != deprange.second; i++) {
			auto j = i;
			std::advance(j, 1);
            for (; j != deprange.second; j++) {
                if ( processed.find(std::make_pair(*i, *j)) == processed.end() ) {
                    boost::add_edge(*i, *j, l);
                    processed.insert(std::make_pair(*i, *j));
                }
            }
        }

		#if 0
		if ( Garnet::getTraceLevel() >= 2 ) {
			std::cout << print_range(remains.begin(), remains.end(), ", ") << std::endl;
		}
		#endif
	});
}

template <typename G>
class line_graph_traits
{
public :
	typedef boost::adjacency_list<
			boost::listS,        // OutEdgeList
			boost::vecS,         // VertexList
			boost::undirectedS,  // Directed
			VProp<G>             // VertexProperty
		> line_graph_type;
};

template <typename G>
typename line_graph_traits<G>::line_graph_type make_line_graph(const G& g)
{
	line_graph_traits<G>::line_graph_type l;
	return l;
}

template <typename G, typename Iterator, typename L>
uint32_t  edge_coloring(G& g, const Iterator& vfirst, const Iterator& vlast, L& l)
{
	// 1. Convert g to a line graph (dependency graph).
	convertToLineGraph(g, vfirst, vlast, l);
	// 2. Perform vertex coloring to l.
	auto num_colors = boost::sequential_vertex_coloring(l, get(&typename L::vertex_property_type::color, l));
	// 3. Reflect the colors from l to g.
	auto l_vertices = boost::vertices(l);
	reflectColors(g, l_vertices.first, l_vertices.second, l);
	
	return static_cast<uint32_t>(num_colors);
}

}//end of namespace dag

namespace network {
template <typename G>
struct VProp {
    typename boost::graph_traits<G>::edge_descriptor  e;
    int16_t  color;
#if defined(GARNET_DEBUG)
    std::string label;
#endif
};

template <typename G, typename L, typename It>
void  reflectColors(G& g, It first, It last, const L& l) {
    typedef typename boost::graph_traits<G>::edge_descriptor   GEdge;

	for (auto it = first; it != last; it++) {
		auto l_prop = l[*it];
		auto& targetColor = g[boost::target(l_prop.e, g)].color;
		if ( targetColor == -1 ) {
			targetColor = l_prop.color;
		}
	}
}

template <typename G, typename Iterator, typename L>
void  convertToLineGraph(const G& g, const Iterator& vfirst, const Iterator& vlast, L& l) {
    typedef typename boost::graph_traits<G>::vertex_descriptor GVertex;
    typedef typename boost::graph_traits<G>::edge_descriptor   GEdge;

    typedef typename boost::graph_traits<L>::vertex_descriptor LVertex;
    typedef typename boost::graph_traits<L>::edge_descriptor   LEdge;
    typedef typename boost::graph_traits<L>::out_edge_iterator LOutEdgeIterator;

    std::map<GEdge, LVertex>      g2l; // G edge to L vertex.
	std::multimap<GVertex, GEdge> v2e;   // G edge directed to G vertex.

	// 1. Add vertices corresponding to edges of the original graph.
    for (auto erange = boost::edges(g); erange.first != erange.second; erange.first++) {
        const boost::graph_traits<G>::edge_descriptor e = *erange.first;
		//std::cout << "E" << e << std::endl;
        
		// 1.1 Add a line vertex.
        LVertex u = boost::add_vertex(l);
        l[u].e = e;
		#if defined(GARNET_DEBUG)
		#if 1
        l[u].label = (boost::format("%s->%s") 
				% g[boost::source(e, g)].label
				% g[boost::target(e, g)].label).str();
		#else
        l[u].label = (boost::format("%s(%d)->%s(%d)") 
				% g[boost::source(e, g)].label
				% g[boost::source(e, g)].color
				% g[boost::target(e, g)].label
				% g[boost::target(e, g)].color).str();
		#endif
		#endif
		g2l[e] = u;
		v2e.insert(std::make_pair(boost::target(e, g), e));
	}

	// 2. Add edges for all combinations of vertices A and Bs.
	std::set<GEdge> remains;
    std::set<std::pair<LVertex, LVertex> > processed;
	for (auto it = vfirst; it != vlast; it++) {
		std::set<LVertex> vlist;

		// 2.1 Add all edges in the stack, which are preserved
		//     for later use.
		for (auto jt = remains.begin(); jt != remains.end(); jt++) {
			vlist.insert(g2l[*jt]);
		}

		// 2.2 Add output edges from the vertex.
		for (auto range = boost::out_edges(*it, g); range.first != range.second; range.first++) {
			vlist.insert(g2l[*range.first]);
			remains.erase(*range.first);
		}

		// 2.3 Add input edges to the vertex.
		for (auto range = v2e.equal_range(*it); range.first != range.second; range.first++) {
			vlist.insert(g2l[range.first->second]);
			remains.insert(range.first->second);
		}

		// 2.2 Add edges for all combinations of vertices picked up in 2.1.
        for (auto i = vlist.begin(); i != vlist.end(); i++) {
			auto j = i;
			std::advance(j, 1);
            for (; j != vlist.end(); j++) {
                if ( processed.find(std::make_pair(*i, *j)) == processed.end() ) {
                    boost::add_edge(*i, *j, l);
                    processed.insert(std::make_pair(*i, *j));
                }
            }
        }

		#if 0
		if ( Garnet::getTraceLevel() >= 2 ) {
			std::cout << print_range(remains.begin(), remains.end(), ", ") << std::endl;
		}
		#endif
	}
}

template <typename G>
class line_graph_traits
{
public :
	typedef boost::adjacency_list<
			boost::listS,        // OutEdgeList
			boost::vecS,         // VertexList
			boost::undirectedS,  // Directed
			VProp<G>        // VertexProperty
		> line_graph_type;
};

template <typename G>
typename line_graph_traits<G>::line_graph_type make_line_graph(const G& g)
{
	line_graph_traits<G>::line_graph_type l;
	return l;
}

/**
 *
 *  [vfirst, vlast) contains all vertieces of g, and it is sorted by topological order.
 *  Sethi-Ullman order or Sarkar order are better to reduce edges for pseudo dependencies.
 *
 *  @param[in] vfirst  first iterator of the vertex array sorted by topological order.
 *  @param[in] vlast
 */
template <typename G, typename Iterator, typename L>
uint32_t  edge_coloring(G& g, const Iterator& vfirst, const Iterator& vlast, L& l)
{
	// 1. Convert g to a line graph (dependency graph).
	convertToLineGraph(g, vfirst, vlast, l);
	// 2. Perform vertex coloring to l.
	auto num_colors = boost::sequential_vertex_coloring(l, get(&typename L::vertex_property_type::color, l));
	// 3. Reflect the colors from l to g.
	auto l_vertices = boost::vertices(l);
	reflectColors(g, l_vertices.first, l_vertices.second, l);
	
	return static_cast<uint32_t>(num_colors);
}

}//end of namespace network



#endif//GARNET__COLORING__H__
