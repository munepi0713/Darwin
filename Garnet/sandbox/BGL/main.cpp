#pragma warning(disable: 4819)

#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <string>
#include <stack>
#include <queue>
#include <utility>
#include <boost/format.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/graph/mcgregor_common_subgraphs.hpp>

#include <Garnet/Tree.h>
#include <Garnet/PPEGraph.h>
#include <Common/GetTime.h>

struct VProp
{
    typedef int  type;
    type         index;
    unsigned int color;
    std::string  label;
};

struct EProp
{
    int color;
    int weight;
};


typedef boost::adjacency_list<
        boost::listS,        // OutEdgeList
        boost::vecS,         // VertexList
        boost::directedS,    // Directed
        VProp,               // VertexProperties
        EProp,                // EdgeProperties
        boost::no_property,
        boost::listS
    > MyGraph;
typedef boost::graph_traits<MyGraph>::edge_descriptor     Edge;
typedef boost::graph_traits<MyGraph>::vertex_descriptor   Vertex;

enum { S, A, B, C, D, E, F, Z, N };
const std::string Names = "SABCDEFZ";

// Create graph.
MyGraph make_graph()
{
    MyGraph g;
    
    Vertex vS = boost::add_vertex(g);
    Vertex vA = boost::add_vertex(g);
    Vertex vB = boost::add_vertex(g);
    Vertex vC = boost::add_vertex(g);
    Vertex vD = boost::add_vertex(g);
    Vertex vE = boost::add_vertex(g);
    Vertex vF = boost::add_vertex(g);
    Vertex vX = boost::add_vertex(g);
    Vertex vY = boost::add_vertex(g);
    Vertex vZ = boost::add_vertex(g);
    
    g[vS].label = "S";
    g[vA].label = "HOGE";
    g[vB].label = "B";
    g[vC].label = "C";
    g[vD].label = "HOGE";
    g[vE].label = "E";
    g[vF].label = "F";
    g[vX].label = "X";
    g[vY].label = "Y";
    g[vZ].label = "Z";

    bool tmp;
    Edge e[11];
    boost::tie(e[0], tmp) = boost::add_edge(vS, vA, g);
    boost::tie(e[1], tmp) = boost::add_edge(vA, vB, g);
    boost::tie(e[2], tmp) = boost::add_edge(vB, vC, g);
    boost::tie(e[3], tmp) = boost::add_edge(vB, vE, g);
    boost::tie(e[4], tmp) = boost::add_edge(vB, vD, g);
//    boost::tie(e[5], tmp) = boost::add_edge(vC, vF, g);
    boost::tie(e[6], tmp) = boost::add_edge(vD, vF, g);
    boost::tie(e[7], tmp) = boost::add_edge(vE, vX, g);
    boost::tie(e[8], tmp) = boost::add_edge(vD, vY, g);
    boost::tie(e[9], tmp) = boost::add_edge(vE, vZ, g);
//    boost::tie(e[10], tmp) = boost::add_edge(vF, vZ, g);

    g[e[0]].weight = 3;
    g[e[1]].weight = 1;
    g[e[2]].weight = 2;
    g[e[3]].weight = 3;
    g[e[4]].weight = 7;
//    g[e[5]].weight = 12;
    g[e[6]].weight = 2;
    g[e[7]].weight = 11;
    g[e[8]].weight = 3;
    g[e[9]].weight = 2;
//    g[e[10]].weight = 2;
    
    return g;
}

MyGraph  StoGraph(const std::string& sexp)
{
    std::stack<Vertex>  s;
    MyGraph g;
    Vertex v, vc;
    //v = boost::add_vertex(g);
    //g[v].label = "Root";
    //s.push(v);
    std::string token = "";
    for (std::string::const_iterator it = sexp.begin(); it != sexp.end(); it++) {
        switch ( *it ) {
        case '(' : 
        case ' ' : 
        case ')' : 
            if ( !token.empty() ) {
                v = boost::add_vertex(g);
                g[v].label = token;
                s.push(v);
                token = "";
            }
            break;
        default :
            token.push_back(*it);
            continue;
        }
        
        if ( *it == ')' ) {
            vc = s.top();
            s.pop();
            if ( !s.empty() ) {
                v = s.top();
                boost::add_edge(v, vc, g);
            }
        }
    }
    
    return g;
}

Vertex  getRoot(const MyGraph& g) {
	std::vector<Vertex> verts;
	boost::topological_sort(g, std::back_inserter(verts));
    return *verts.rbegin();
}

/** Return a S expression of this tree. */
std::string  getSExpression(const Vertex& u, const MyGraph& g) {
    std::string ret;
    ret += "(";
    ret += g[u].label;

    MyGraph::adjacency_iterator  it, end;
    for (boost::tie(it, end) = boost::adjacent_vertices(u, g); it != end; it++) {
        ret += getSExpression(*it, g);
    }
    ret += ")";
    return ret;
}
std::string  getSExpression(const MyGraph& g) {
    return boost::num_vertices(g) == 0 ? "" : getSExpression(getRoot(g), g);
}


class dfs_visitor : public boost::default_dfs_visitor
{
public :
    dfs_visitor(uint32_t& maxDepth)
     : _max_depth(maxDepth), _current_depth(0) {}
    dfs_visitor(const dfs_visitor& vis) 
     : _max_depth(vis._max_depth), _current_depth(vis._current_depth) {}
    
#if 0
    template <typename E, typename G>
    void  tree_edge(E e, const G& g) {
        std::cout << "tree_edge " << _current_depth << std::endl;
    }
    
    template <typename E, typename G>
    void  back_edge(E e, const G& g) {
        std::cout << "back_edge" << std::endl;
    }
    
    template <typename E, typename G>
    void  examine_edge(E e, const G& g) {
        std::cout << "examine_edge" << std::endl;
    }
    
    template <typename E, typename G>
    void  forward_or_cross_edge(E e, const G& g) {
        std::cout << "forward_or_cross_edge" << std::endl;
    }
    
    template <typename V, typename G>
    void  start_vertex(V u, const G& g) {
        std::cout << "start_vertex" << std::endl;
    }
#endif
    template <typename V, typename G>
    void  finish_vertex(V u, const G& g) {
        //std::cout << "finish_vertex" << std::endl;
        --_current_depth;
    }
    
    template <typename V, typename G>
    void  discover_vertex(V u, const G& g) {
        //std::cout << "discover_vertex " << _current_depth << std::endl;
        ++_current_depth;
        if ( _current_depth > _max_depth ) _max_depth = _current_depth;
    }
private :
    uint32_t&  _max_depth;
    uint32_t   _current_depth;
};

template <typename MyGraph>
class dfs_subtree_visitor : public boost::default_dfs_visitor
{
public :
    dfs_subtree_visitor(std::vector<MyGraph>& subtrees, uint32_t depth = 1)
     : _subtrees(subtrees), _currentDepth(0), _depth(depth) {}
    dfs_subtree_visitor(const dfs_subtree_visitor& vis) 
     : _subtrees(vis._subtrees), _currentDepth(vis._currentDepth), _depth(vis._depth) {}
    
#if 0
    template <typename E, typename G>
    void  tree_edge(E e, const G& g) {
        std::cout << "tree_edge " << std::endl;
    }
    
    template <typename E, typename G>
    void  back_edge(E e, const G& g) {
        std::cout << "back_edge" << std::endl;
    }
    
    template <typename E, typename G>
    void  examine_edge(E e, const G& g) {
        std::cout << "examine_edge" << std::endl;
    }
    
    template <typename E, typename G>
    void  forward_or_cross_edge(E e, const G& g) {
        std::cout << "forward_or_cross_edge" << std::endl;
    }
    
    template <typename V, typename G>
    void  start_vertex(V u, const G& g) {
        std::cout << "start_vertex" << std::endl;
    }
#endif
    template <typename V, typename G>
    void  finish_vertex(V u, const G& g) {
        std::cout << "finish_vertex " << _currentDepth << std::endl;
        _currentDepth--;
        if ( _currentDepth >= _depth ) {
            G gt;
            Vertex ut;
            ut = boost::add_vertex(gt);
            gt[ut].label = g[u].label;
            boost::graph_traits<G>::adjacency_iterator it;
            boost::graph_traits<G>::adjacency_iterator end;
            std::stack<Vertex> children;
            for (boost::tie(it, end) = boost::adjacent_vertices(u, g); it != end; it++) {
                G gs = _stack.top();
                _stack.pop();
                uint32_t pos = boost::num_vertices(gt);
                boost::copy_graph(gs, gt);
                children.push(boost::vertex(pos, gt));
            }
            while ( !children.empty() ) {
                boost::add_edge(ut, children.top(), gt);
                children.pop();
            }
            _stack.push(gt);
            _subtrees.push_back(gt);
        }
    }
    #if 1
    template <typename V, typename G>
    void  discover_vertex(V u, const G& g) {
        std::cout << "discover_vertex " << _currentDepth << std::endl;
        _currentDepth++;
    }
    #endif
private :
    std::vector<MyGraph>&  _subtrees;
    std::stack<MyGraph>    _stack;
    uint32_t             _currentDepth;
    uint32_t             _depth;
};

std::vector<MyGraph>  getSubTrees(const MyGraph& g, uint32_t depth = 1)
{
    std::vector<MyGraph>  ret;
    boost::depth_first_search(g, boost::visitor(dfs_subtree_visitor<MyGraph>(ret, depth)));
    return ret;
}


template <typename MyGraph>
class dfs_replace_visitor : public boost::default_dfs_visitor
{
public :
    dfs_replace_visitor(const Vertex& u, MyGraph& g1, const MyGraph& g2)
     : _u(u), _g1(g1), _g2(g2), _active(true) {}
    dfs_replace_visitor(const dfs_replace_visitor& vis) 
     : _u(vis._u), _g1(vis._g1), _g2(vis._g2), _stack(vis._stack), _active(vis._active) {}

    template <typename V, typename G>
    void  finish_vertex(V u, const G& g) {
        std::cout << "finish_vertex " << g[u].label << " " << _active << std::endl;
        if ( _active ) {
            Vertex u2, v2;
            u2 = _stack.top();
            _stack.pop();
            if ( !_stack.empty() ) {
                v2 = _stack.top();
                boost::add_edge(v2, u2, _g1);
            }
        }
        
        if ( u == _u ) {
            _active = true;
        }
    }
    template <typename V, typename G>
    void  discover_vertex(V u, const G& g) {
        std::cout << "discover_vertex " << g[u].label << " " << _active << std::endl;
        if ( u == _u ) {
            if ( boost::num_vertices(_g2) > 0 ) {
                MyGraph::vertices_size_type pos = boost::num_vertices(_g1);
                boost::copy_graph(_g2, _g1);
                if ( !_stack.empty() ) {
                    Vertex v2 = _stack.top();
                    boost::add_edge(v2, boost::vertex(pos, _g1), _g1);
                }
            }

            _active = false;
        }
        
        if ( _active ) {
            Vertex v = boost::add_vertex(_g1);
            _g1[v] = g[u];
            _stack.push(v);
        }
    }

private :
    std::stack<Vertex> _stack;
    const Vertex&  _u;
    MyGraph& _g1;
    const MyGraph& _g2;
    bool _active;
};

void  replace(const Vertex& u, MyGraph& g1, const MyGraph& g2)
{
    MyGraph gt;
    boost::depth_first_search(g1, boost::visitor(dfs_replace_visitor<MyGraph>(u, gt, g2)));
    boost::swap(g1, gt);
}

void  remove(const Vertex& u, MyGraph& g1)
{
    #if 0
    MyGraph g2;
    boost::depth_first_search(g1, boost::visitor(dfs_remove_visitor<MyGraph>(u, g2)));
    boost::swap(g1, g2);
    #else
    MyGraph gt;
    boost::depth_first_search(g1, boost::visitor(dfs_replace_visitor<MyGraph>(u, gt, MyGraph())));
    boost::swap(g1, gt);
    #endif
}

template <typename MyGraph>
class dfs_test_visitor : public boost::default_dfs_visitor
{
public :
    template <typename E, typename G>
    void  tree_edge(E e, const G& g) {
        std::cout << "tree_edge " << g[boost::source(e, g)].label << "->" << g[boost::target(e, g)].label << std::endl;
    }
    
    template <typename E, typename G>
    void  back_edge(E e, const G& g) {
        std::cout << "back_edge " << g[boost::source(e, g)].label << "->" << g[boost::target(e, g)].label << std::endl;
    }
    
    template <typename E, typename G>
    void  examine_edge(E e, const G& g) {
        std::cout << "examine_edge " << g[boost::source(e, g)].label << "->" << g[boost::target(e, g)].label << std::endl;
    }
    
    template <typename E, typename G>
    void  forward_or_cross_edge(E e, const G& g) {
        std::cout << "forward_or_cross_edge " << g[boost::source(e, g)].label << "->" << g[boost::target(e, g)].label << std::endl;
    }
    
    template <typename V, typename G>
    void  start_vertex(V u, const G& g) {
        std::cout << "start_vertex " << g[u].label << std::endl;
    }
    template <typename V, typename G>
    void  finish_vertex(V u, const G& g) {
        std::cout << "finish_vertex " <<  g[u].label << std::endl;
    }
    template <typename V, typename G>
    void  discover_vertex(V u, const G& g) {
        std::cout << "discover_vertex " << g[u].label << std::endl;
    }
private :
};


struct LineV {
    boost::graph_traits<MyGraph>::edge_descriptor  e;
    std::string label;
    uint32_t    color;
};
typedef boost::adjacency_list<
        boost::listS,        // OutEdgeList
        boost::vecS,         // VertexList
        boost::undirectedS,  // Directed
        LineV
    > LineGraph;

template <typename MyGraph, typename LineGraph>
class dfs_convert_visitor : public boost::default_dfs_visitor
{
public :
    typedef typename boost::graph_traits<MyGraph>::edge_descriptor   Edge;

    typedef typename boost::graph_traits<LineGraph>::vertex_descriptor LVertex;
    typedef typename boost::graph_traits<LineGraph>::edge_descriptor   LEdge;
    typedef typename boost::graph_traits<LineGraph>::out_edge_iterator LOutEdgeIterator;
    
    dfs_convert_visitor(LineGraph& l) : _l(l) {}
    dfs_convert_visitor(const dfs_convert_visitor& vis) : _l(vis._l), _v(vis._v) {}

#if 0
    template <typename E, typename G>
    void  tree_edge(E e, const G& g) {
        std::cout << "tree_edge " << g[boost::source(e, g)].label << "->" << g[boost::target(e, g)].label << std::endl;
    }
    
    template <typename E, typename G>
    void  back_edge(E e, const G& g) {
        std::cout << "back_edge " << g[boost::source(e, g)].label << "->" << g[boost::target(e, g)].label << std::endl;
    }
#endif
    template <typename E, typename G>
    void  examine_edge(E e, const G& g) {
        //std::cout << "examine_edge " << g[boost::source(e, g)].label << "->" << g[boost::target(e, g)].label << std::endl;
        LVertex u;
        if ( _v.find(e) != _v.end() ) {
            u = _v[e];
        }
        else {
            u = boost::add_vertex(_l);
            _l[u].e = e;
            _l[u].label = g[boost::source(e, g)].label + "->" + g[boost::target(e, g)].label;
            _v[e] = u;
        }
        std::vector<LVertex> vlist;
        vlist.push_back(u);
        boost::graph_traits<G>::out_edge_iterator it, end;
        for (boost::tie(it, end) = boost::out_edges(boost::target(e, g), g); it != end; it++) {
            E& f = *it;
            LVertex v;
            if ( _v.find(f) != _v.end() ) {
                v = _v[f];
            }
            else {
                v = boost::add_vertex(_l);
                _l[v].e = f;
                _l[v].label = g[boost::source(f, g)].label + "->" + g[boost::target(f, g)].label;
                _v[f] = v;
            }
            vlist.push_back(v);
        }
        for (std::vector<LVertex>::iterator i = vlist.begin(); i != vlist.end(); i++) {
            std::vector<LVertex>::iterator j = i;
            for (j++; j != vlist.end(); j++) {
                //std::cout << ".";
                boost::add_edge(*i, *j, _l);
            }
        }
    }
#if 0
    template <typename E, typename G>
    void  forward_or_cross_edge(E e, const G& g) {
        std::cout << "forward_or_cross_edge " << g[boost::source(e, g)].label << "->" << g[boost::target(e, g)].label << std::endl;
    }
    
    template <typename V, typename G>
    void  start_vertex(V u, const G& g) {
        std::cout << "start_vertex " << g[u].label << std::endl;
    }
    template <typename V, typename G>
    void  finish_vertex(V u, const G& g) {
        std::cout << "finish_vertex " <<  g[u].label << std::endl;
    }
    template <typename V, typename G>
    void  discover_vertex(V u, const G& g) {
        std::cout << "discover_vertex " << g[u].label << std::endl;
    }
#endif
private :
    LineGraph& _l;
    std::map<Edge, LVertex> _v;
};

template <typename G, typename L>
void  convertToLineGraph(const G& g, L& _l) {
    typedef typename boost::graph_traits<MyGraph>::edge_descriptor   Edge;

    typedef typename boost::graph_traits<LineGraph>::vertex_descriptor LVertex;
    typedef typename boost::graph_traits<LineGraph>::edge_descriptor   LEdge;
    typedef typename boost::graph_traits<LineGraph>::out_edge_iterator LOutEdgeIterator;

    std::map<Edge, LVertex> _v;
    boost::graph_traits<G>::edge_iterator eit, eend;
    for (boost::tie(eit, eend) = boost::edges(g); eit != eend; eit++) {
        boost::graph_traits<G>::edge_descriptor e = *eit;
            
        LVertex u;
        if ( _v.find(e) != _v.end() ) {
            u = _v[e];
        }
        else {
            u = boost::add_vertex(_l);
            _l[u].e = e;
            _l[u].label = g[boost::source(e, g)].label + "->" + g[boost::target(e, g)].label;
            _v[e] = u;
        }
        std::vector<LVertex> vlist;
        vlist.push_back(u);
        boost::graph_traits<G>::out_edge_iterator it, end;
        for (boost::tie(it, end) = boost::out_edges(boost::target(e, g), g); it != end; it++) {
            Edge& f = *it;
            LVertex v;
            if ( _v.find(f) != _v.end() ) {
                v = _v[f];
            }
            else {
                v = boost::add_vertex(_l);
                _l[v].e = f;
                _l[v].label = g[boost::source(f, g)].label + "->" + g[boost::target(f, g)].label;
                _v[f] = v;
            }
            vlist.push_back(v);
        }
        for (std::vector<LVertex>::iterator i = vlist.begin(); i != vlist.end(); i++) {
            std::vector<LVertex>::iterator j = i;
            for (j++; j != vlist.end(); j++) {
                //std::cout << ".";
                boost::add_edge(*i, *j, _l);
            }
        }
    }
}

template <typename G, typename L, typename First, typename Last>
void  convertToLineGraph(const G& g, L& _l, const First& efirst, const Last& elast) {
	typedef G  MyGraph;
	typedef L  LineGraph;

    typedef typename boost::graph_traits<MyGraph>::vertex_descriptor   Vertex;
    typedef typename boost::graph_traits<MyGraph>::edge_descriptor     Edge;

    typedef typename boost::graph_traits<LineGraph>::vertex_descriptor LVertex;
    typedef typename boost::graph_traits<LineGraph>::edge_descriptor   LEdge;
    typedef typename boost::graph_traits<LineGraph>::out_edge_iterator LOutEdgeIterator;

	// 1. Add vertices corresponding to edges of the original graph.
    std::map<Edge, LVertex> _v;
    for (auto erange = boost::edges(g); erange.first != erange.second; erange.first++) {
        const boost::graph_traits<G>::edge_descriptor e = *erange.first;
		//std::cout << "E" << e << std::endl;
        
		// 1.1 Add a line vertex.
        LVertex u = boost::add_vertex(_l);
        _l[u].e = e;
		#if defined(_DEBUG)
        _l[u].label = (boost::format("%s(%d)->%s(%d)") 
				% g[boost::source(e, g)].label
				% g[boost::source(e, g)].color
				% g[boost::target(e, g)].label
				% g[boost::target(e, g)].color).str();
		#endif
		_v[e] = u;
	}

	// 2. Add edges for all combinations of vertices A and Bs.
	//    A is a vertex corresponding one of an original edge.
	//    Bs are vertices corresponding original edges which source
	//    is the A's corresponding edge.
    for (auto erange = boost::edges(g); erange.first != erange.second; erange.first++) {
        const boost::graph_traits<G>::edge_descriptor e = *erange.first;
		//std::cout << "E" << e << std::endl;

		// 2.1 Pick up vertices corresponding original edges
		//     which share a same vertex.
		std::vector<LVertex> vlist;
		// 2.1.1 A edge.
        vlist.push_back(_v[e]);
		// 2.1.2 B edges.
        for (auto oerange = boost::out_edges(boost::target(e, g), g); oerange.first != oerange.second; oerange.first++) {
            vlist.push_back(_v[*oerange.first]);
        }

		// 2.2 Add edges for all combinations of vertices picked up in 2.1.
        for (auto i = vlist.begin(); i != vlist.end(); i++) {
            for (auto j = i + 1; j != vlist.end(); j++) {
                boost::add_edge(*i, *j, _l);
            }
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
		#if 1//defined(_DEBUG)
        l[u].label = (boost::format("%s(%d)->%s(%d)") 
				% g[boost::source(e, g)].label
				% g[boost::source(e, g)].color
				% g[boost::target(e, g)].label
				% g[boost::target(e, g)].color).str();
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

template <typename GraphFirst,
          typename GraphSecond>
struct print_callback {

  print_callback(const GraphFirst& graph1,
                 const GraphSecond& graph2) :
    m_graph1(graph1), m_graph2(graph2) { }

template <typename CorrespondenceMapFirstToSecond,
          typename CorrespondenceMapSecondToFirst>
  bool operator()(CorrespondenceMapFirstToSecond correspondence_map_1_to_2,
                  CorrespondenceMapSecondToFirst correspondence_map_2_to_1,
typename boost::graph_traits<GraphFirst>::vertices_size_type subgraph_size) {

    // Print out correspondences between vertices
    BGL_FORALL_VERTICES_T(vertex1, m_graph1, GraphFirst) {

      // Skip unmapped vertices
      if (get(correspondence_map_1_to_2, vertex1) != boost::graph_traits<GraphSecond>::null_vertex()) {
        auto vertex2 = get(correspondence_map_1_to_2, vertex1);
        std::cout << "[" << vertex1 << "]" << m_graph1[vertex1].label
            << " <-> [" 
            << vertex2 << "]" << m_graph2[vertex2].label << std::endl;
      }

    }

    std::cout << "---" << std::endl;

    return (true);
  }

  private:
    const GraphFirst& m_graph1;
    const GraphSecond& m_graph2;

};


template <typename Graph>
class dfs_dfdfs_visitor : public boost::default_dfs_visitor
{
public :
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    typedef typename std::list<std::pair<Vertex, int32_t> > Container;

    dfs_dfdfs_visitor(Container* list)
     : _list(list), _current_depth(-1) { _pos = _list->begin(); }
    dfs_dfdfs_visitor(const dfs_dfdfs_visitor& x) 
     : _list(x._list), _pos(x._pos), _current_depth(x._current_depth) {}
    
    template <typename G>
    void show(const G& g) {
        Container::iterator it = _list->begin();
        while ( it != _pos ) {
            std::cout << boost::format("%u[%u] ") % g[it->first].label % it->second;
            it++;
        }
        std::cout << "*[*] ";
        while ( it != _list->end() ) {
            std::cout << boost::format("%u[%u] ") % g[it->first].label % it->second;
            it++;
        }
        std::cout << std::endl;
    }
    
    template <typename V, typename G>
    void  finish_vertex(V u, const G& g) {
        //
        std::cout << "- current: " << g[u].label << "[" << _current_depth << "]" << std::endl;
        if ( _pos != _list->end() && _current_depth < _pos->second ) {
            do {
                show(g);
                _pos++;
            }
            while ( _pos != _list->end() && _current_depth > _pos->second );
        }
        else if ( _pos != _list->begin()  && _pos->second <= _current_depth ) {
            do {
                show(g);
                _pos--;
            }
            while ( _pos != _list->begin() && _pos->second <= _current_depth );
        }
        show(g);
        _pos = _list->insert(_pos, std::make_pair(u, _current_depth));
        std::cout << "inserted." << std::endl;
        
        //
        --_current_depth;
    }
    
    template <typename V, typename G>
    void  discover_vertex(V u, const G& g) {
            std::cout << "+ current: " << _current_depth << ", pos: " << (_pos == _list->end() ? -1 : _pos->second) << std::endl;
        ++_current_depth;
    }
    
private :
    int32_t                      _current_depth;
    typename Container*          _list;
    typename Container::iterator _pos;
};

bool replace(Garnet::Tree& g, const std::string& label, const Garnet::Tree& newTree) {
	Garnet::Tree gSub;
	bool result = g.replace(label, newTree, gSub);
	if (result) {
		auto subtrees = gSub.getSubTrees(1, 2);
		if (subtrees.size() > 0) while (replace(g, "_1", subtrees[0].first));
		if (subtrees.size() > 1) while (replace(g, "_2", subtrees[1].first));
		if (subtrees.size() > 2) while (replace(g, "_3", subtrees[2].first));
		if (subtrees.size() > 3) while (replace(g, "_4", subtrees[3].first));
		return true;
	}
	else {
		return false;
	}
};

void testSubgraphSwap()
{
	{
		Garnet::Tree g1("(A(B(C)(D))(C))");
		Garnet::Tree g2("(E(F)(G))");

		g1.replace("C", g2);
		
		std::cout << "-----\n";
		std::cout << g1 << std::endl;
	}
	{
		Garnet::Tree g1("(A(B(C)(D))(C))");
		Garnet::Tree g2("(E(F)(G))");

		g1.replaceAll("C", g2);
		
		std::cout << "-----\n";
		std::cout << g1 << std::endl;
	}
	{
		Garnet::Tree g1("(A(B(C)(D(E(X(Y)(Z)))))(C))");
		Garnet::Tree g2("(F(_1)(G))");

		g1.replaceAll("D", g2);
		
		std::cout << "-----\n";
		std::cout << g1 << std::endl;
	}
	{
		Garnet::Tree g1("(A(B(C)(D(E(X(Y)(Z)))))(C))");
		Garnet::Tree g2("(F(_1)(G))");
		Garnet::Tree g3;

		g1.replace("D", g2, g3);
		
		std::cout << "-----\n";
		std::cout << g1 << std::endl;
		std::cout << g3 << std::endl;
	}
	{
		Garnet::Tree g1("(A(B(C)(D(E(X(Y)(Z)))(L)))(C))");
		Garnet::Tree g2("(F(_1)(D(_1)))");
		Garnet::Tree g3;

		//g1.replace("D", g2, g3);
		
		std::cout << "-----\n";
		//std::cout << g1 << std::endl;
		//std::cout << g3 << std::endl;
		/*
		auto subtrees = g3.getSubTrees(1, 2);
		for (auto it = subtrees.begin(); it != subtrees.end(); it++) {
			std::cout << it->first << std::endl;
		}
		*/
		
		//std::cout << "--\n";
		while (replace(g1, "D", g2)) {
			std::cout << g1 << std::endl;
		}
	}
}


int main(int argc, const char* argv[])
{

	testSubgraphSwap();

#if 0
    //const MyGraph g = make_graph();
    const MyGraph g = argc >= 2 ? StoGraph(argv[1]) : make_graph();

	// Show graph
    boost::print_graph(g, get(&VProp::label, g));
//    boost::print_graph(g, Names.c_str());

#if 0
	// Save as a graphviz's dot file.
    std::ofstream file("test.dot");
    boost::write_graphviz(file, g, boost::make_label_writer(get(&VProp::label, g)));
    file.close();
#endif

	// Get root node.
	std::vector<Vertex> verts;
	boost::topological_sort(g, std::back_inserter(verts));
	std::cout << "First: " << g[verts[0]].label << std::endl;
	std::cout << "Last:  " << g[verts[verts.size() - 1]].label << std::endl;
    
    std::cout << "-----" << std::endl;
    // Get S Expression
//    std::string sexp = getSExpression(verts[verts.size() - 1], g);
    std::string sexp = getSExpression(getRoot(g), g);
    std::cout << sexp << std::endl;
    
    std::cout << "-----" << std::endl;
    uint32_t  maxDepth = 0;
    dfs_visitor  vis(maxDepth);
    boost::depth_first_search(g, boost::visitor(vis));
    std::cout << "MaxDepth: " << maxDepth << std::endl;
	
    std::cout << "-----" << std::endl;
	std::vector<MyGraph> subtrees;
    #if 1
    dfs_subtree_visitor<MyGraph>  sbvis(subtrees, 0);
    boost::depth_first_search(g, boost::visitor(sbvis));
    #else
    subtrees = getSubTrees(g);
    #endif
    for (auto it = subtrees.begin(); it != subtrees.end(); it++) {
        #if 0
        boost::print_graph(*it, get(&VProp::label, *it));
        std::cout << std::endl;
        #else
        std::cout << getSExpression(getRoot(*it), *it) << std::endl;
        #endif
    }
    
    std::cout << "-----" << std::endl;
    subtrees = getSubTrees(g, 1);
    for (auto it = subtrees.begin(); it != subtrees.end(); it++) {
        #if 0
        boost::print_graph(*it, get(&VProp::label, *it));
        std::cout << std::endl;
        #else
        std::cout << getSExpression(getRoot(*it), *it) << std::endl;
        #endif
    }
    
    std::cout << "-----" << std::endl;
    MyGraph g2;
    g2 = g;
    Vertex root_g2 = getRoot(g2);
    Vertex newroot_g2 = boost::add_vertex(g2);
    g2[newroot_g2].label = "RootG2";
    boost::add_edge(newroot_g2, root_g2, g2);
    boost::print_graph(g2, get(&VProp::label, g2));
    std::cout << getSExpression(getRoot(g2), g2) << std::endl;
    std::cout << "-----" << std::endl;
    boost::print_graph(g, get(&VProp::label, g));

    std::cout << "-----" << std::endl;
    {
        MyGraph gr = g;
        std::cout << getSExpression(gr) << std::endl;
        remove(boost::vertex(2, gr), gr);
        //remove(getRoot(gr), gr);
        std::cout << getSExpression(gr) << std::endl;
    }

    std::cout << "-----" << std::endl;
    {
        MyGraph gr = g;
        MyGraph gs = g;
        std::cout << getSExpression(gr) << std::endl;
        gs = StoGraph("(1(2(3)(4))(5(6(7))(8(9))))");
        replace(boost::vertex(4, gr), gr, gs);
        std::cout << getSExpression(gr) << std::endl;
        replace(boost::vertex(2, gr), gr, gs);
        std::cout << getSExpression(gr) << std::endl;
        replace(boost::vertex(6, gr), gr, gs);
        std::cout << getSExpression(gr) << std::endl;
    }
    #if 0
    {
        boost::graph_traits<MyGraph>::edge_iterator  edges_begin, edges_end;
        boost::tie(edges_begin, edges_end) = boost::edges(g);

        typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS> Graph;
        typedef boost::graph_traits<Graph>::vertices_size_type  vertices_size_type;
        typedef boost::property_map<Graph, boost::vertex_index_t>::const_type vertex_index_map;

        Graph g2(edges_begin, edges_end, boost::num_edges(g));

        std::vector<vertices_size_type>  color_vec(boost::num_vertices(g2));
        boost::iterator_property_map<vertices_size_type*, vertex_index_map> 
                color(&color_vec.front(), get(boost::vertex_index, g2));

        boost::graph_traits<MyGraph>::vertices_size_type num_colors;
        num_colors = boost::sequential_vertex_coloring(g2, color);

        std::cout << num_colors << " colors." << std::endl;
        //boost::print_graph(g2, &color_vec.front());
    }
    #endif
    {
        MyGraph g2 = g;
        boost::graph_traits<MyGraph>::vertices_size_type num_colors;
        num_colors = boost::sequential_vertex_coloring(g2, get(&VProp::color, g2));
        std::cout << num_colors << " colors." << std::endl;
        boost::print_graph(g2, get(&VProp::color, g2));
    }
    {
        using namespace boost;
		 typedef adjacency_list<listS, vecS, undirectedS> Graph;
		  typedef graph_traits<Graph>::vertex_descriptor vertex_descriptor;
		  typedef graph_traits<Graph>::vertices_size_type vertices_size_type;
		  typedef property_map<Graph, vertex_index_t>::const_type vertex_index_map;

		  typedef std::pair<int, int> Edge;
		  enum nodes {A, B, C, D, E, n};
		  Edge edge_array[] = { Edge(A, C), Edge(B, B), Edge(B, D), Edge(B, E), 
								Edge(C, B), Edge(C, D), Edge(D, E), Edge(E, A), 
								Edge(E, B) };
		  int m = sizeof(edge_array) / sizeof(Edge);
		  Graph g(edge_array, edge_array + m, n);

		  // Test with the normal order
		  std::vector<vertices_size_type> color_vec(num_vertices(g));
		  iterator_property_map<vertices_size_type*, vertex_index_map>
			color(&color_vec.front(), get(vertex_index, g));
		  vertices_size_type num_colors = sequential_vertex_coloring(g, color);
				std::cout << num_colors << " colors." << std::endl;
  }
    
    std::cout << "-----" << std::endl;
    boost::depth_first_search(g, boost::visitor(dfs_test_visitor<MyGraph>()));

    std::cout << "-----" << std::endl;
    {
        StopWatch watch;
        LineGraph l;
        boost::depth_first_search(g, boost::visitor(dfs_convert_visitor<MyGraph, LineGraph>(l)));
        std::cout << watch.lap() << " sec" << std::endl;
        boost::print_graph(l, get(&LineV::label, l));

        boost::graph_traits<LineGraph>::vertices_size_type num_colors;
        num_colors = boost::sequential_vertex_coloring(l, get(&LineV::color, l));
        std::cout << num_colors << " colors." << std::endl;
        boost::print_graph(l, get(&LineV::color, l));
    }
    
    std::cout << "-----" << std::endl;
    {// This method is 20% faster than the method with depth_first_search.
        StopWatch watch;
        LineGraph l;
        convertToLineGraph(g, l);
        std::cout << watch.lap() << " sec" << std::endl;
        boost::print_graph(l, get(&LineV::label, l));
    }
    
    std::cout << "-----" << std::endl;
    {
        using namespace boost;
        
        MyGraph gs = StoGraph("(E(X)(Z))");

        print_callback<MyGraph, MyGraph> my_callback(g, gs);
        
        mcgregor_common_subgraphs(g, gs, true, my_callback);
        std::cout << std::endl;

        // Print out all connected common subgraphs between graph1 and graph2.
        mcgregor_common_subgraphs(g, gs, true, my_callback,
          vertices_equivalent(make_property_map_equivalent(get(&VProp::label, g), get(&VProp::label, gs))));
        std::cout << std::endl;

        mcgregor_common_subgraphs_maximum(g, gs, true, my_callback,
          vertices_equivalent(make_property_map_equivalent(get(&VProp::label, g), get(&VProp::label, gs))));
        std::cout << std::endl;

        mcgregor_common_subgraphs_maximum_unique(g, gs, true, my_callback,
          vertices_equivalent(make_property_map_equivalent(get(&VProp::label, g), get(&VProp::label, gs))));
        std::cout << std::endl;
    }
    
    std::cout << "-----" << std::endl;
    {
        using namespace boost;
        
        MyGraph g1 = StoGraph("(A(E(X)(Z))(F(E(X)(Z))))");
        MyGraph g2 = StoGraph("(E(X)(Z))");

        print_callback<MyGraph, MyGraph> my_callback(g1, g2);
        
        mcgregor_common_subgraphs(g1, g2, true, my_callback);
        std::cout << std::endl;

        // Print out all connected common subgraphs between graph1 and graph2.
        mcgregor_common_subgraphs(g1, g2, true, my_callback,
          vertices_equivalent(make_property_map_equivalent(get(&VProp::label, g1), get(&VProp::label, g2))));
        std::cout << std::endl;

        mcgregor_common_subgraphs_maximum(g1, g2, true, my_callback,
          vertices_equivalent(make_property_map_equivalent(get(&VProp::label, g1), get(&VProp::label, g2))));
        std::cout << std::endl;

        mcgregor_common_subgraphs_maximum_unique(g1, g2, true, my_callback,
          vertices_equivalent(make_property_map_equivalent(get(&VProp::label, g1), get(&VProp::label, g2))));
        std::cout << std::endl;
    }
    
    std::cout << "-----" << std::endl;
    {
        using namespace boost;
        
        MyGraph g1 = StoGraph("(E(X)(Z))");
        MyGraph g2 = StoGraph("(A(E(X)(Z))(F(E(X)(Z))))");

        print_callback<MyGraph, MyGraph> my_callback(g1, g2);
        
        #if 0
        mcgregor_common_subgraphs(g1, g2, true, my_callback);
        std::cout << std::endl;

        // Print out all connected common subgraphs between graph1 and graph2.
        mcgregor_common_subgraphs(g1, g2, true, my_callback,
          vertices_equivalent(make_property_map_equivalent(get(&VProp::label, g1), get(&VProp::label, g2))));
        std::cout << std::endl;

        mcgregor_common_subgraphs_maximum(g1, g2, true, my_callback,
          vertices_equivalent(make_property_map_equivalent(get(&VProp::label, g1), get(&VProp::label, g2))));
        std::cout << std::endl;
        #endif

        mcgregor_common_subgraphs_maximum_unique(g1, g2, true, my_callback,
          vertices_equivalent(make_property_map_equivalent(get(&VProp::label, g1), get(&VProp::label, g2))));
        std::cout << std::endl;
    }
    
    std::cout << "-----" << std::endl;
    {
        using namespace boost;
        
        MyGraph g1 = StoGraph("(E(X)(Z))");
        MyGraph g2 = StoGraph("(A(E(Z)(X))(F(E(X)(Z))))");

        print_callback<MyGraph, MyGraph> my_callback(g1, g2);

        mcgregor_common_subgraphs_maximum_unique(g1, g2, true, my_callback,
          vertices_equivalent(make_property_map_equivalent(get(&VProp::label, g1), get(&VProp::label, g2))));
        std::cout << std::endl;
    }

    std::cout << "=====" << std::endl;
    {
        Garnet::Tree  tree("(S(HOGE(B(C)(E(X)(Z))(HOGE(F)(Y)))))");
        std::cout << tree << std::endl;
        std::cout << tree.getNumVertices() << std::endl;
        std::cout << tree.getDepth() << std::endl;
        Garnet::Tree::VertexType root = tree.getRoot();
        std::cout << root->label << std::endl;
        Garnet::Tree::VertexType root2;
        root2 = root;
        std::cout << root2->label << std::endl;
        Garnet::Tree::ConstVertexType croot = tree.getRoot();
        std::cout << croot->label << std::endl;
        Garnet::Tree::ConstVertexType croot2;
        croot2 = croot;
        std::cout << croot2->label << std::endl;
        croot2 = root;
        std::cout << croot2->label << std::endl;
        std::vector<Garnet::Tree::VertexType> vertices;
        vertices = tree.getAllLeaves();
        std::cout << vertices.size() << " leaves : " << std::flush;
        for (auto it = vertices.begin(); it != vertices.end(); it++) {
            std::cout << (*it)->label << " ";
        }
        std::cout << std::endl;
        //std::vector<Garnet::Tree::ConstVertexType> cvertices;
        //cvertices = tree.getAllLeaves();
        //for (auto it = vertices.begin(); it != vertices.end(); it++) {
        //    std::cout << it->label << " ";
        //}
        //std::cout << std::endl;
        #if 0
        std::vector<std::pair<Garnet::Tree, Garnet::Tree::VertexType> >  subtrees;
        subtrees = tree.getAllSubTrees();
        for (auto it = subtrees.begin(); it != subtrees.end(); it++) {
            std::cout << it->first << " ";
        }
        std::cout << std::endl;
//        std::vector<std::pair<Garnet::Tree, Garnet::Tree::ConstVertexType> >  csubtrees;
//        csubtrees = tree.getAllSubTrees();
//        for (auto it = csubtrees.begin(); it != csubtrees.end(); it++) {
//            std::cout << it->first << " ";
//        }
//        std::cout << std::endl;
        subtrees = tree.getSubTrees(2, 4);
        for (auto it = subtrees.begin(); it != subtrees.end(); it++) {
            std::cout << it->first << " ";
        }
        std::cout << std::endl;
        {
            Garnet::Tree gr = tree;
            Garnet::Tree gs = "(1(2(3)(4))(5(6(7))(8(9))))";
            gr.replace(gr.getVertex(4), gs);
            std::cout << gr << std::endl;
        }
        #endif
    }

    std::cout << "=====" << std::endl;
    {
        const Garnet::Tree  tree("(S(HOGE(B(C)(E(X)(Z))(HOGE(F)(Y)))))");
        std::cout << tree << std::endl;
        std::cout << tree.getNumVertices() << std::endl;
        std::cout << tree.getDepth() << std::endl;
        //Garnet::Tree::VertexType root = tree.getRoot();
        //std::cout << root->label << std::endl;
        //Garnet::Tree::VertexType root2;
        //root2 = root;
        //std::cout << root2->label << std::endl;
        Garnet::Tree::ConstVertexType croot = tree.getRoot();
        std::cout << croot->label << std::endl;
        Garnet::Tree::ConstVertexType croot2;
        croot2 = croot;
        std::cout << croot2->label << std::endl;
        //croot2 = root;
        //std::cout << croot2->label << std::endl;
        //std::vector<Garnet::Tree::VertexType> vertices;
        //vertices = tree.getAllLeaves();
        //for (auto it = vertices.begin(); it != vertices.end(); it++) {
        //    std::cout << it->label << " ";
        //}
        //std::cout << std::endl;
        std::vector<Garnet::Tree::ConstVertexType> cvertices;
        std::cout << cvertices.size() << " leaves : " << std::flush;
        cvertices = tree.getAllLeaves();
        for (auto it = cvertices.begin(); it != cvertices.end(); it++) {
            std::cout <<(*it)->label << " ";
        }
        std::cout << std::endl;
//        std::vector<std::pair<Garnet::Tree, Garnet::Tree::VertexType> >  subtrees;
//        subtrees = tree.getAllSubTrees();
//        for (auto it = subtrees.begin(); it != subtrees.end(); it++) {
//            std::cout << it->first << " ";
//        }
//        std::cout << std::endl;
        std::vector<std::pair<Garnet::Tree, Garnet::Tree::ConstVertexType> >  csubtrees;
        csubtrees = tree.getAllSubTrees();
        for (auto it = csubtrees.begin(); it != csubtrees.end(); it++) {
            std::cout << it->first << " ";
        }
        std::cout << std::endl;
        csubtrees = tree.getSubTrees(2, 4);
        for (auto it = csubtrees.begin(); it != csubtrees.end(); it++) {
            std::cout << it->first << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "-----" << std::endl;
    {
        Garnet::Tree g("(S(A(E(Z)(X))(F(E(X)(Z))))(B(E(Z)(X))(F(E(X)(Z)))))");
        Garnet::Tree a("(E(X)(Z))");
        std::vector<Garnet::Tree::VertexType> m = g.find(a);
        for (auto it = m.begin(); it != m.end(); it++) {
            std::cout << g.getSExpression(*it) << std::endl;
        }
        std::cout << std::endl;
    }
    
    std::cout << "-----" << std::endl;
    {
        Garnet::Tree g("(S(A(E(Z)(X))(F(E(X)(Z))))(B(E(Z)(X))(F(E(X)(Z)))))");
        Garnet::Tree a("(E(X)(Z))");
        std::vector<Garnet::Tree::VertexType> m = g.findFirstOf(a);
        for (auto it = m.begin(); it != m.end(); it++) {
            std::cout << g.getSExpression(*it) << std::endl;
        }
        std::cout << std::endl;
    }
    
    std::cout << "-----" << std::endl;
    {
        Garnet::Tree g("(S(A(E(Z)(X))(F(E(X)(Z))))(B(E(Z)(X))(F(E(X)(Z)))))");
        Garnet::Tree a("(E(X)(Z))");
        Garnet::Tree b("(L(M)(N(O)(P)))");
        std::vector<Garnet::Tree::VertexType> m = g.findFirstOf(a);
        for (auto it = m.begin(); it != m.end(); it++) {
            std::cout << g.getSExpression(*it) << std::endl;
        }
        g.replace(m.front(), b);
        std::cout << g << std::endl;
    }
    
    std::cout << "-----" << std::endl;
    {
        Garnet::Tree g("(S(A(E(Z)(X))(F(E(X)(Z))))(B(E(Z)(X))(F(E(X)(Z)))))");
        Garnet::Tree a("(E(X)(Z))");
        Garnet::Tree b("(L(M)(N(O)(P)))");
        g.replaceAll(a, b);
        std::cout << g << std::endl;
    }
#endif

#if 0    
    std::cout << "----- topological + depth sort -----" << std::endl;
    {
        using namespace boost;
        
        Garnet::Tree g("(S(A(B(0))(C(D(1))(E(F(2))(G(3))))))");
        std::cout << g << std::endl;
        #if 0
        std::vector<Garnet::Tree::GraphType::vertex_descriptor> ulist;
        boost::topological_sort(g.getGraph(), std::back_inserter(ulist));
        for (auto it = ulist.cbegin(); it != ulist.cend(); it++) {
            std::cout << boost::format("\n[%u] %s") % *it % g.getGraph()[*it].label;
        }
        #endif
        
        
        struct {
            typedef Garnet::Tree::GraphType                    Graph;
            typedef Garnet::Tree::GraphType::vertex_descriptor Vertex;
            uint32_t func(Vertex u, const Graph& g, uint32_t depth) {
                uint32_t maxDepth = depth;
                for (auto it = out_edges(u, g); it.first != it.second; it.first++) {
                    uint32_t subDepth = func(target(*it.first, g), g, depth + 1);
                    if ( subDepth > maxDepth ) maxDepth = subDepth;
                }
                _depth[u] = maxDepth;
                return maxDepth;
            }
            std::map<Vertex, uint32_t> _depth;
        } hoge;
        for (auto it = vertices(g.getGraph()); it.first != it.second; it.first++) {
            if ( hoge._depth.find(*it.first) == hoge._depth.end() ) {
                hoge.func(*it.first, g.getGraph(), 0);
            }
        }
        for (auto it = vertices(g.getGraph()); it.first != it.second; it.first++) {
            std::cout << boost::format(" %s(%u)") % g.getGraph()[*it.first].label % hoge._depth[*it.first];
        }
        std::cout << std::endl;

        struct {
            typedef Garnet::Tree::GraphType                    Graph;
            typedef Garnet::Tree::GraphType::vertex_descriptor Vertex;
            typedef Garnet::Tree::GraphType::edge_descriptor   Edge;
            
            struct comp {
                const Graph& _g;
                const std::map<Vertex, uint32_t>& _depth;
                comp(const Graph& g, const std::map<Vertex, uint32_t>& depth) 
                    : _g(g), _depth(depth) {}
                bool operator()(const Edge& x, const Edge& y) const {
                    return _depth.at(target(x, _g)) > _depth.at(target(y, _g));
                }
            };
            
            void func(Vertex u, const Graph& g, const std::map<Vertex, uint32_t>& depth) {
                std::vector<Edge> e;
                for (auto it = out_edges(u, g); it.first != it.second; it.first++) {
                    e.push_back(*it.first);
                }
                std::sort(e.begin(), e.end(), comp(g, depth));
                for (auto it = e.begin(); it != e.end(); it++) {
                    func(target(*it, g), g, depth);
                }
                _result.push_back(u);
                _visit[u] = 1;
            }
            std::vector<Vertex> _result;
            std::vector<int8_t> _visit;
        } fuga;
        fuga._visit.resize(num_vertices(g.getGraph()), 0);
        for (auto it = vertices(g.getGraph()); it.first != it.second; it.first++) {
            if ( fuga._visit[*it.first] == 0 ) {
                fuga.func(*it.first, g.getGraph(), hoge._depth);
            }
        }
        for (auto it = fuga._result.begin(); it != fuga._result.end(); it++) {
            std::cout << boost::format(" %s(%u)") % g.getGraph()[*it].label % hoge._depth[*it];
        }

        {
            LineGraph l;
        	std::vector<Garnet::Tree::GraphType::vertex_descriptor> nodes;
        	boost::topological_sort(g.getGraph(), std::back_inserter(nodes));
        	convertToLineGraph(g.getGraph(), nodes.begin(), nodes.end(), l);
            boost::graph_traits<LineGraph>::vertices_size_type num_colors;
            num_colors = boost::sequential_vertex_coloring(l, get(&LineGraph::vertex_property_type::color, l));

    		int32_t maxDegree = 0;
    		for (auto it = vertices(l); it.first != it.second; it.first++) {
    			int32_t degree = boost::degree(*it.first, l);
    			if ( maxDegree < degree ) maxDegree = degree;
    		}
            
            std::cout << "\n----- topo_sort -----";
    		std::cout << "\n# of vertices:   " << boost::num_vertices(l);
    		std::cout << "\n# of edges:      " << boost::num_edges(l);
    	    std::cout << "\n# of colors:     " << num_colors;
    		std::cout << "\nmax # of degree: " << maxDegree;
            std::cout << std::endl;
            print_graph(l, get(&LineV::label, l));
        }

        {
            LineGraph l;
        	convertToLineGraph(g.getGraph(), fuga._result.begin(), fuga._result.end(), l);
            boost::graph_traits<LineGraph>::vertices_size_type num_colors;
            num_colors = boost::sequential_vertex_coloring(l, get(&LineGraph::vertex_property_type::color, l));

    		int32_t maxDegree = 0;
    		for (auto it = vertices(l); it.first != it.second; it.first++) {
    			int32_t degree = boost::degree(*it.first, l);
    			if ( maxDegree < degree ) maxDegree = degree;
    		}
            
            std::cout << "\n----- dfdfs -----";
    		std::cout << "\n# of vertices:   " << boost::num_vertices(l);
    		std::cout << "\n# of edges:      " << boost::num_edges(l);
    		std::cout << "\n# of colors:     " << num_colors;
    		std::cout << "\nmax # of degree: " << maxDegree;
            std::cout << std::endl;
            print_graph(l, get(&LineV::label, l));
        }

        //boost::depth_first_search(g.getGraph(), boost::visitor(dfs_test_visitor<Garnet::Tree::GraphType>()));
        
        #if 0
        dfs_dfdfs_visitor<Garnet::Tree::GraphType>::Container cont;
        dfs_dfdfs_visitor<Garnet::Tree::GraphType> visitor(&cont);
        boost::depth_first_search(g.getGraph(), boost::visitor(visitor));
        for (auto it = cont.cbegin(); it != cont.cend(); it++) {
            std::cout << boost::format(" [%u] %s (%u)") % it->first % g.getGraph()[it->first].label % it->second;
        }
        std::cout << std::endl;
        #endif
    }
#endif

    return 0;
}
//(S(HOGE(B(C)(E(X)(Z))(HOGE(F)(Y)))))
