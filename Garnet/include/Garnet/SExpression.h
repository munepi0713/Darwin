#ifndef GARNET__SEXPRESSION__H__
#define GARNET__SEXPRESSION__H__

#include <vector>
#include <algorithm>
#include <string>
#include <stack>

#include <boost/graph/adjacency_list.hpp>

#include <Garnet/TreeUtilities.h>

namespace Garnet {

template <typename Graph, typename Vertex, typename Map>
void getSExpressions(const Graph& g, const Vertex& u, Map& map)
{
	typedef typename Map::mapped_type String;
	
	String ret;
	ret += "(";
	ret += g[u].label;
	auto a_vertices = boost::adjacent_vertices(u, g);
	for_each(a_vertices.first, a_vertices.second, [&] (const Vertex& v) {
		getSExpressions(g, v, map);
		ret += map[v];
	});
	ret += ")";
	map[u] = ret;
}

template <typename Graph, typename Map>
void getSExpressions(const Graph& g, Map& map)
{
	typedef boost::graph_traits<typename Graph>::vertex_descriptor Vertex;

	std::vector<Vertex> roots;
	getRoots(g, std::back_inserter(roots));
	getSExpressions(g, roots[0], map);
}

template <typename Graph, typename String>
Graph createGraphFromSExpression(const String& sexp)
{
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
	typedef typename String::value_type Char;
	
	std::stack<Vertex> s;
    Graph g;
    Vertex v, vc;
    String token = "";
    int16_t depth = -1;
	for_each(sexp.begin(), sexp.end(), [&] (Char c) {
        switch ( c ) {
        case '(' : 
        case ' ' : 
        case ')' : 
            if ( !token.empty() ) {
                v = boost::add_vertex(g);
                g[v].label = token;
                s.push(v);
                token = "";
            }
			if ( c == ')' ) {
				vc = s.top();
				s.pop();
				if ( !s.empty() ) {
					v = s.top();
					boost::add_edge(v, vc, g);
				}
				depth--;
			}
			else if ( c == '(' ) {
				depth++;
			}
            break;
        default :
            token.push_back(c);
        }
    });
    return g;
}

template <typename Graph>
Graph createGraphFromSExpression(const char* sexp_)
{
	std::string sexp(sexp_);
	return createGraphFromSExpression<Graph>(sexp);
}

} // end of namespace Garnet

#endif//GARNET__SEXPRESSION__H__
