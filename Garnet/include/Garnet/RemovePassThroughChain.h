#ifndef GARNET__REMOVE_PASS_THROUGH_CHAIN__H__
#define GARNET__REMOVE_PASS_THROUGH_CHAIN__H__

#include <iostream>
#include <stdint.h>
#include <vector>
//#include <numeric>
#include <iterator>

#include <boost/format.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

#include <Common/Text.h>

#include <Garnet/TreeUtilities.h>

namespace Garnet {

template <typename Graph, typename Iterator>
void removePassThroughChains(Graph& g, Iterator first, Iterator last)
{
	typedef typename Graph::vertex_descriptor  Vertex;
	typedef typename Graph::edge_descriptor    Edge;

	Graph g_new;
	std::map<Vertex, Vertex> u2v; // <K,V>=<g vertex, g_new vertex>

	// 1. Copy all verteices from g to g_new if a vertex is not "Move" with color=-1.
	auto g_vertices = boost::vertices(g);
	std::for_each(g_vertices.first, g_vertices.second, [&] (Vertex u) {
		if ( g[u].color != -1 || !Text::equals(g[u].label, "Move") ) {
			Vertex v = boost::add_vertex(g_new);
			g_new[v] = g[u];
			u2v[u] = v;
		}
	});
	
	// 2. Connect g_new edges.
	std::for_each(g_vertices.first, g_vertices.second, [&] (Vertex u) {
		if ( u2v.find(u) != u2v.end() ) {
			auto edges = boost::out_edges(u, g);
			std::for_each(edges.first, edges.second, [&] (Edge e) {
				Vertex t = e.m_target;
				while ( u2v.find(t) == u2v.end() ) {
					t = boost::out_edges(t, g).first->m_target;
				}
				boost::add_edge(u2v[u], u2v[t], g_new);
			});
		}
	});
	
	// 3. Swap.
	g.swap(g_new);
}

template <typename Graph>
void removePassThroughChains(Graph& g)
{
	typedef typename Graph::vertex_descriptor  Vertex;

	std::vector<Vertex> roots;
	getRoots(g, std::back_inserter(roots));
	
	removePassThroughChains(g, roots.begin(), roots.end());
}

}//end of namespace Garnet

#endif//GARNET__REMOVE_PASS_THROUGH_CHAIN__H__
