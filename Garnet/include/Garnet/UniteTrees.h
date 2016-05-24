#ifndef GARNET__UNITE_TREES__H__
#define GARNET__UNITE_TREES__H__

#include <algorithm>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

#include <Garnet/SExpression.h>

namespace Garnet {

template <typename SrcGraph, typename Iterator, typename DstGraph, typename Copier>
void uniteTreesToDag(Iterator first, Iterator last, DstGraph& dst_g, Copier copy)
{
	typedef boost::graph_traits<typename SrcGraph>::vertex_descriptor SrcVertex;
	typedef boost::graph_traits<typename SrcGraph>::edge_descriptor   SrcEdge;
	typedef boost::graph_traits<typename DstGraph>::vertex_descriptor DstVertex;
	typedef boost::graph_traits<typename DstGraph>::edge_descriptor   DstEdge;

	std::map<std::string, DstVertex> dag;

	std::for_each(first, last, [&] (const SrcGraph& src_g) {
		std::map<SrcVertex, std::string> sexp;
		getSExpressions(src_g, sexp);
	
		std::map<SrcVertex, DstVertex> v2v;

		// 1. Get a vertex list as topological order.
		std::deque<SrcVertex> vertices;
		boost::topological_sort(src_g, std::back_inserter(vertices));
		
		// 2. Copy vertices/edges from the tree to the dag.
		while ( !vertices.empty() ) {
			SrcVertex u = vertices.front();
			vertices.pop_front();
			
			auto it = dag.find(sexp[u]);
			if ( it != dag.end() ) {
				//std::cout << " --> Existig Structure" << std::endl;
				// dst_g has a subtree as same form as a subtree topped u in the index-th tree.
				// Modify edge destination to v instead of u.
				DstVertex v = it->second;
				v2v[u] = v;
			}
			else {
				//std::cout << " --> New Structure" << std::endl;
				// Add new vertex w to dst_g corresponding to u.
				DstVertex w = boost::add_vertex(dst_g);
				copy(src_g[u], dst_g[w]);

				// Record relation between u and w.
				v2v[u] = w;
				
				// Add edges between w and dst_g vertices corresponding u's children.
				auto edges = boost::out_edges(u, src_g);
				std::for_each(edges.first, edges.second, [&] (const SrcEdge& e) {
					auto ch = e.m_target;
					auto c  = v2v[ch];
					assert(dag[sexp[ch]] == c);
					boost::add_edge(w, c, dst_g);
				});
				dag[sexp[u]] = w;
			}
		}
	});
}

template <typename SrcGraph, typename Iterator, typename DstGraph>
void uniteTreesToDag(Iterator first, Iterator last, DstGraph& dst_g)
{
	typedef typename SrcGraph::vertex_property_type SrcProp;
	typedef typename DstGraph::vertex_property_type DstProp;
	
	uint32_t index;
	uniteTreesToDag<SrcGraph>(first, last, dst_g, [&index] (const SrcProp& src_p, DstProp& dst_p) {
		dst_p.label = (Text::equals(src_p.label, "Sink") || Text::equals(src_p.label, "VASink"))
				? (boost::format("%s%d") % src_p.label % index).str()
				: src_p.label;
		index++;
	});
}


template <typename SrcGraph, typename Iterator, typename DstGraph, typename Copier>
void uniteTreesToForest(Iterator first, Iterator last, DstGraph& dst_g, Copier copy)
{
	typedef boost::graph_traits<typename SrcGraph>::vertex_descriptor SrcVertex;
	typedef boost::graph_traits<typename SrcGraph>::edge_descriptor   SrcEdge;
	typedef boost::graph_traits<typename DstGraph>::vertex_descriptor DstVertex;
	typedef boost::graph_traits<typename DstGraph>::edge_descriptor   DstEdge;

	// Just move all trees into single container. (Multiple DAGs in the iv as a result.)
	std::for_each(first, last, [&] (const SrcGraph& src_g) {
		std::map<SrcVertex, DstVertex> v2v;

		// 1. Copy vertices.
		for (auto vit = vertices(src_g); vit.first != vit.second; vit.first++) {
			SrcVertex v = *vit.first;
			DstVertex u = add_vertex(dst_g);

			v2v[v] = u;

#if 1
			copy(src_g[v], dst_g[u]);
#else
			if ( Text::equals(src_g[v].label, "Sink") || Text::equals(src_g[v].label, "VASink") ) {
				dst_g[u].label = (boost::format("%s%d") % src_g[*vit.first].label % index).str();
			}
			else {
				dst_g[u].label = src_g[v].label;
			}
#endif
		}

		// 2. Copy edges.
		for (auto eit = edges(src_g); eit.first != eit.second; eit.first++) {
			add_edge(v2v[(*eit.first).m_source], v2v[(*eit.first).m_target], dst_g);
		}
	});
}

}//end of namespace Garnet


#endif//GARNET__UNITE_TREES__H__
