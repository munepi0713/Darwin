#ifndef GARNET__TREE_UTILITIES__H__
#define GARNET__TREE_UTILITIES__H__

#include <algorithm>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

namespace Garnet {

namespace impl {

	class dfs_depth_visitor : public boost::default_dfs_visitor
	{
	public :
		dfs_depth_visitor(uint32_t& maxDepth) : _max_depth(maxDepth), _current_depth(0) {}
		dfs_depth_visitor(const dfs_depth_visitor& x) : _max_depth(x._max_depth), _current_depth(x._current_depth) {}
		template <typename Vertex, typename Graph>
		void  discover_vertex(Vertex u, const Graph& g) {
			++_current_depth;
			if ( _current_depth > _max_depth ) _max_depth = _current_depth;
		}
		template <typename Vertex, typename Graph>
		void  finish_vertex(Vertex u, const Graph& g) {
			--_current_depth;
		}
	private :
		uint32_t&  _max_depth;
		uint32_t   _current_depth;
	};

}//end of namespace impl

// Find root vertices.
// A root vertex is no input edges.
// It assumes vertex type is an integer type.
template <typename Graph, typename Inserter>
void getRoots(const Graph& g, Inserter inserter)
{
	typedef boost::graph_traits<typename Graph>::edge_descriptor   Edge;

	std::vector<bool> mark(num_vertices(g), false);
	auto erange = edges(g);
	std::for_each(erange.first, erange.second, [&mark] (const Edge& e) {
		mark[e.m_target] = true;
	});
	for (auto i = 0u; i < mark.size(); i++) {
		if ( !mark[i] ) *inserter++ = i;
	}
}

template <typename Graph>
int getMaxOutDegree(const Graph& g)
{
	typedef boost::graph_traits<typename Graph>::vertex_descriptor  Vertex;
	auto maxDegree = 0u;
    foreach(Vertex u : boost::vertices(g)) {
        auto degree = boost::out_degree(u, g);
        if (degree > maxDegree) maxDegree = degree;
    }
    return maxDegree;
}

template <typename Graph>
int getMaxDegree(const Graph& l) {
    typedef boost::graph_traits<typename Graph>::vertex_descriptor  Vertex;
    auto maxDegree = 0u;
    foreach(Vertex u : boost::vertices(g)) {
        auto degree = boost::degree(u, g);
        if (degree > maxDegree) maxDegree = degree;
    }
    return maxDegree;
}

template <typename Graph>
uint32_t   getDepth(const Graph& g)
{
	typedef boost::graph_traits<typename Graph>::vertex_descriptor Vertex;
	uint32_t  depth = 0;
	std::vector<Vertex> nodes;
	boost::topological_sort(g, std::back_inserter(nodes));
	boost::depth_first_search(g,
		boost::visitor(impl::dfs_depth_visitor(depth)).
		root_vertex(nodes.back()));
	return depth;
}

}//end of namespace Garnet

#endif//GARNET__TREE_UTILITIES__H__
