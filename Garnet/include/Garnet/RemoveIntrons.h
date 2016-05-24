#ifndef GARNET__REMOVE_INTRONS__H__
#define GARNET__REMOVE_INTRONS__H__

#include <stack>
#include <set>
#include <map>
#include <algorithm>

#include <boost/graph/adjacency_list.hpp>

namespace Garnet {

/**
 *  Remove introns.
 *    Remove all vertices which is NOT connected with specified root vertices.
 *    [first, last)  A list of the root vertices.
 */
template <typename Graph, typename Iterator>
void removeIntrons(Graph& g, Iterator first, Iterator last)
{
	typedef typename Graph::vertex_descriptor  Vertex;
	typedef typename Graph::edge_descriptor    Edge;

	// Data:
	//   q:      Vertex stack which is waiting to be processed.
	//   mark_v: Important vertices. A vertex is an intron if it's not in mark_v.
	//   mark_e: Important edges. An edge is an intron if it's not in mark_e.
	std::stack<Vertex> q;
	std::set<Vertex> mark_v;
	//std::set<Edge>   mark_e;

	// 1. Move roots into stack.
	for (auto it = first; it != last; it++) {
		q.push(*it);
	}

	// 2. Mark vertices and edges connected with roots import.
	while ( !q.empty() ) {
		Vertex g_u = q.top();
		q.pop();

		// Mark it is an important vertex.
		mark_v.insert(g_u);
		
		// Mark edges from this vertex are introns and push child vertices.
		auto edges = out_edges(g_u, g);
		std::for_each(edges.first, edges.second, [&] (const Edge& g_e) {
			// Mark it is an important edge.
			//mark_e.insert(g_e);
			q.push(g_e.m_target);
		});
	}
	
	// 3. Make a new Graph by copying V and E from the orignal except introns.
	Graph g_new;
	std::map<Vertex, Vertex> v2v; // <K,V>=<g vertex, g_new vertex>
	// Copy important vertices from g to g_new.
	std::for_each(mark_v.begin(), mark_v.end(), [&] (const Vertex& g_u) {
		Vertex g_new_u = boost::add_vertex(g_new); // add a vertex to g_new
		g_new[g_new_u] = g[g_u];             // copy properties
		v2v[g_u] = g_new_u;                  // remember mapping
	});
	// Connect vertieces of g_new.
	std::for_each(mark_v.begin(), mark_v.end(), [&] (const Vertex& g_u) {
		auto edges = boost::out_edges(g_u, g);
		std::for_each(edges.first, edges.second, [&] (const Edge& g_e) {
			boost::add_edge(v2v[g_e.m_source], v2v[g_e.m_target], g_new);
		});
	});
	
	// 4. Swap.
	g.swap(g_new);
}

}//end of namespace Garnet

#endif//GARNET__REMOVE_INTRONS__H__
