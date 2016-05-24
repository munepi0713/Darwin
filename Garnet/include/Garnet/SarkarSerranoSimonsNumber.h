#ifndef GARNET__SETHI_ULLMAN_NUMBER__H__
#define GARNET__SETHI_ULLMAN_NUMBER__H__

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/graph/depth_first_search.hpp>

/**
 * [1] Vivek Sarkar, et al., "Register-Sensitive Selection, Duplication,
 *     and Sequencing of Instruction.", ICS '01 Sorrent, Italy, ACM 2001.
 */
template <typename Graph, typename Reg>
void sarkar_serrano_simons_number(const Graph& g, Reg& reg)
{
	typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
	typedef typename boost::graph_traits<Graph>::edge_descriptor   Edge;

	// Assign modified Sethi-Ullman numbers for each node in Topological order
	// instead of recursive processing. (Step 2 of Figure 5 Sequencing Algorithm)
	std::vector<Vertex> nodes;
	boost::topological_sort(g, back_inserter(nodes));	
	for_each(nodes.cbegin(), nodes.cend(), [&g, &reg] (const Vertex& u) {
		auto k = boost::out_degree(u, g);
		if ( k == 0 ) {
			reg[u] = 0; // In Garnet, leaf is not a constant but a load op for constant value.
		}
		else {
			// sort children by its reg number.
			std::vector<Vertex> children;
			auto edges = boost::out_edges(u, g);
			std::for_each(edges.first, edges.second, [&children] (Edge e) {
				children.push_back(e.m_target);
			});
			std::sort(children.begin(), children.end(), [&reg] (Vertex u, Vertex v) {
				return reg[u] < reg[v];
			});
			// find max(reg[child] + k - i) and set it as parents' reg.
			auto i = 0u, r = 0u;
			std::for_each(children.begin(), children.end(), [&] (Vertex u) {
				auto temp = reg[u] + children.size() - i;
				if ( temp > r ) r = temp;
				i++;
			});
			reg[u] = r;
		}
	});
}

#endif//GARNET__SETHI_ULLMAN_NUMBER__H__
