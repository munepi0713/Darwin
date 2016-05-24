#ifndef GARNET__TOPOLOGICAL_SORT__H__
#define GARNET__TOPOLOGICAL_SORT__H__

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/graph/depth_first_search.hpp>

#include <Garnet/SarkarSerranoSimonsNumber.h>
#include <Garnet/TreeUtilities.h>

//#define _DEBUG

/**
 *  Determine an execution order based on Sethi-Ullman numbering.
 *  It is similar to DFS, but this algorithm search a deeper subtree first
 *  to reduce live registers.
 */
template <typename Graph, typename Inserter, typename Reg>
void deep_subtree_first_sort(const Graph& g, Inserter inserter, Reg& reg)
{
	using namespace std;
	using namespace boost;

	typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
	typedef typename graph_traits<Graph>::edge_descriptor   Edge;
	
	//map<Vertex, uint32_t> reg;
	// Sethi-Ullman order
	sarkar_serrano_simons_number(g, reg);
	
	// Serialize all nodes in DAG on depth-first basis.
	// Visiting is processed from a child tree which has the greater reg number to one has the less.
	stack<Vertex> source;
	set<Vertex> visited;
	set<Vertex> done;
	auto pred = [&reg] (const Vertex& u, const Vertex& v) {
		return reg[u] < reg[v];
	};
	auto push = [&source] (const Vertex& u) {
		source.push(u);
	};
	// Push root vertices to stack.
	// Sort them by SSS number in ascending order.
	{
		vector<Vertex> roots;
		getRoots(g, back_inserter(roots));		
		sort(roots.begin(), roots.end(), pred);
		for_each(roots.cbegin(), roots.cend(), push);
	}
	// Process all child nodes recursively.
	while ( !source.empty() ) {
		auto u = source.top();
		
		if ( done.find(u) != done.end() ) {
			source.pop();
		}
		else {
			visited.insert(u);
			
			//
			vector<Vertex> children;
			auto erange = out_edges(u, g);
			for_each(erange.first, erange.second, [&children, &visited] (const Edge& e) {
				if ( visited.find(e.m_target) == visited.end() ) {
					children.push_back(e.m_target);
				}
			});
			if ( children.empty() ) {
				source.pop();
				done.insert(u);
				*inserter++ = u;
			}
			else {
				sort(children.begin(), children.end(), pred);
				for_each(children.cbegin(), children.cend(), push);
			}
		}
	}
}

template <typename Graph, typename Inserter>
void deep_subtree_first_sort(const Graph& g, Inserter inserter)
{
	typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;

	std::map<Vertex, uint32_t> reg;
	deep_subtree_first_sort(g, inserter, reg);
}

template <typename Graph, typename Inserter>
void topological_sort(uint32_t method, const Graph& g, Inserter inserter)
{
	typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
	//typedef typename graph_traits<Graph>::edge_descriptor   Edge;
	
	switch ( method ) {
	default :
	case 0 :
		// Schedule based on the Sethi-Ullman order.
		{
			std::map<Vertex, uint32_t> reg;
			deep_subtree_first_sort(g, inserter, reg);
		}
		break;
	case 1 :
		// Schedule based on the topological order.
		boost::topological_sort(g, inserter);
		break;
	}
}

template <typename Graph, typename Inserter, typename Reg>
void topological_sort(uint32_t method, const Graph& g, Inserter& inserter, Reg& reg)
{
	switch ( method ) {
	default :
	case 0 :
		// Schedule based on the Sethi-Ullman order.
		deep_subtree_first_sort(g, inserter, reg);
		break;
	case 1 :
		// Schedule based on the topological order.
		boost::topological_sort(g, inserter);
		break;
	}
}

#endif//GARNET__TOPOLOGICAL_SORT__H__
