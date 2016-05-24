/*********************************************************
 *  Garnet
 *  Author: Munehiro Doi.
 *********************************************************/

#ifdef __MSVC__
#pragma warning(disable: 4819)
#endif

#include <Garnet/Tree.h>

#include <string>
#include <utility>
#include <vector>
#include <set>
#include <stack>
#include <queue>
#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/copy.hpp>

#include <Garnet/TraceLevel.h>

namespace impl
{

//template <typename Graph>
class dfs_depth_visitor : public boost::default_dfs_visitor
{
public :
    dfs_depth_visitor(uint32_t& maxDepth) : _max_depth(maxDepth), _current_depth(0) {}
    dfs_depth_visitor(const dfs_depth_visitor& vis) : _max_depth(vis._max_depth), _current_depth(vis._current_depth) {}
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

template <typename G>
class dfs_replace_visitor : public boost::default_dfs_visitor
{
public :
	//typedef G                                    Graph;
	typedef typename boost::graph_traits<G>::vertex_descriptor Vertex;

    dfs_replace_visitor(const Vertex& u, G& g1, const G& g2)
     : _u(u), _g1(g1), _g2(g2), _active(true) {}
    dfs_replace_visitor(const dfs_replace_visitor& vis) 
     : _stack(vis._stack), _u(vis._u), _g1(vis._g1), _g2(vis._g2), _active(vis._active) {}
    template <typename Vertex, typename Graph>
    void  finish_vertex(Vertex u, const Graph& g) {
        //std::cout << "finish_vertex " << g[u].label << " " << _active << std::endl;
        if ( _active ) {
            Vertex u2, v2;
            u2 = _stack.top();
            _stack.pop();
            if ( !_stack.empty() ) {
                v2 = _stack.top();
                boost::add_edge(v2, u2, _g1);
            }
        }
		if (u == _u) {
			_active = true;
		}
    }
    template <typename Vertex, typename Graph>
    void  discover_vertex(Vertex u, const Graph& g) {
        //std::cout << "discover_vertex " << g[u].label << " " << _active << std::endl;
        if ( u == _u ) {
            if ( boost::num_vertices(_g2) > 0 ) {
                typename Graph::vertices_size_type pos = boost::num_vertices(_g1);
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
    G& _g1;
    const G& _g2;
    bool _active;
};

template <typename G>
class dfs_replace_and_save_visitor : public boost::default_dfs_visitor
{
public:
	//typedef G                                    Graph;
	typedef typename boost::graph_traits<G>::vertex_descriptor Vertex;

	dfs_replace_and_save_visitor(const Vertex& u, G& g1, const G& g2, G& g3)
		: _u(u), _g1(g1), _g2(g2), _g3(g3), _active(true) {}
	dfs_replace_and_save_visitor(const dfs_replace_and_save_visitor& vis)
		: _stack(vis._stack), _u(vis._u), _g1(vis._g1), _g2(vis._g2), _g3(vis._g3), _active(vis._active) {}
	template <typename Vertex, typename Graph>
	void  finish_vertex(Vertex u, const Graph& g) {
		//std::cout << "finish_vertex " << g[u].label << " " << _active << std::endl;
		if (_active) {
			Vertex u2, v2;
			u2 = _stack.top();
			_stack.pop();
			if (!_stack.empty()) {
				v2 = _stack.top();
				boost::add_edge(v2, u2, _g1);
			}
		}
		else {
			Vertex u2, v2;
			u2 = _stack2.top();
			_stack2.pop();
			if (!_stack2.empty()) {
				v2 = _stack2.top();
				boost::add_edge(v2, u2, _g3);
			}
		}
		if (u == _u) {
			_active = true;
		}
	}
	template <typename Vertex, typename Graph>
	void  discover_vertex(Vertex u, const Graph& g) {
		//std::cout << "discover_vertex " << g[u].label << " " << _active << std::endl;
		if (u == _u) {
			if (boost::num_vertices(_g2) > 0) {
				typename Graph::vertices_size_type pos = boost::num_vertices(_g1);
				boost::copy_graph(_g2, _g1);
				if (!_stack.empty()) {
					Vertex v2 = _stack.top();
					boost::add_edge(v2, boost::vertex(pos, _g1), _g1);
				}
			}
			_active = false;
		}
		if (_active) {
			Vertex v = boost::add_vertex(_g1);
			_g1[v] = g[u];
			_stack.push(v);
		}
		else {
			Vertex v = boost::add_vertex(_g3);
			_g3[v] = g[u];
			_stack2.push(v);
		}
	}

private:
	std::stack<Vertex> _stack;
	std::stack<Vertex> _stack2;
	const Vertex&  _u;
	G& _g1;
	const G& _g2;
	G& _g3;
	bool _active;
};
template <typename G>
class dfs_subtree_visitor : public boost::default_dfs_visitor
{
public :
	typedef typename boost::graph_traits<G>::vertex_descriptor Vertex;

    dfs_subtree_visitor(std::vector<std::pair<G, Vertex> >& subtrees, uint32_t minDepth, uint32_t maxDepth)
     : _subtrees(subtrees), _currentDepth(0), _minDepth(minDepth), _maxDepth(maxDepth) {}
    dfs_subtree_visitor(const dfs_subtree_visitor& vis) 
     : _subtrees(vis._subtrees), _currentDepth(vis._currentDepth), _minDepth(vis._minDepth), _maxDepth(vis._maxDepth) {}  
    template <typename Vertex, typename Graph>
    void  finish_vertex(Vertex u, const Graph& g) {
        //std::cout << "finish_vertex " << _currentDepth << std::endl;
        _currentDepth--;
        Graph gt;
        Vertex ut;
        ut = boost::add_vertex(gt);
        gt[ut].label = g[u].label;
        typename boost::graph_traits<Graph>::adjacency_iterator it;
        typename boost::graph_traits<Graph>::adjacency_iterator end;
        std::stack<Vertex> children;
        for (boost::tie(it, end) = boost::adjacent_vertices(u, g); it != end; it++) {
            Graph gs = _stack.top();
            _stack.pop();
            auto pos = boost::num_vertices(gt);
            boost::copy_graph(gs, gt);
            children.push(boost::vertex(pos, gt));
        }
        while ( !children.empty() ) {
            boost::add_edge(ut, children.top(), gt);
            children.pop();
        }
        _stack.push(gt);
        if ( _currentDepth >= _minDepth && _currentDepth < _maxDepth ) {
            _subtrees.push_back(std::make_pair(gt, ut));
        }
    }
    template <typename Vertex, typename Graph>
    void  discover_vertex(Vertex u, const Graph& g) {
        //std::cout << "discover_vertex " << _currentDepth << std::endl;
        _currentDepth++;
    }
private :
    std::vector<std::pair<G, Vertex> >&  _subtrees;
    std::stack<G>    _stack;
    uint32_t             _currentDepth;
    uint32_t             _minDepth;
    uint32_t             _maxDepth;
};
template <typename Graph>
Graph createGraphFromSExpression(const std::string& sexp)
{
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
	std::stack<Vertex> s;
    Graph g;
    int16_t depth = -1;
#if 0
	auto i = 0u;
	while (i < sexp.size()) {
	    Vertex v, vc;
		auto pos = sexp.find_first_of("() ", i);
		if (pos > i) {
			v = boost::add_vertex(g);
			g[v].label = sexp.substr(i, pos - i);
			//g[v].depth = depth;
			s.push(v);
			i += (pos - i);
		}

		auto ch = sexp[pos];
		if ( ch == ')' ) {
			vc = s.top();
			s.pop();
			if ( !s.empty() ) {
				v = s.top();
				boost::add_edge(v, vc, g);
			}
			depth--;
		}
		else if ( ch == '(' ) {
			depth++;
		}
		i++;
    }
#else
    Vertex v, vc;
	std::string token;
    for (std::string::const_iterator it = sexp.begin(); it != sexp.end(); it++) {
        switch ( *it ) {
        case '(' : 
        case ' ' : 
        case ')' : 
            if ( !token.empty() ) {
                v = boost::add_vertex(g);
                g[v].label = token;
                //g[v].depth = depth;
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
            depth--;
        }
        else if ( *it == '(' ) {
            depth++;
        }
    }
#endif
    return g;
}

template <typename V, typename G>
std::vector<V> getAllLeaves(G& g) {
	std::vector<V> ret;
	auto range = boost::vertices(g);
	for (auto it = range.first; it != range.second; it++) {
		if ( boost::out_degree(*it, g) == 0 ) {
            typename boost::graph_traits<G>::vertex_descriptor desc = *it;
			ret.push_back(V(&g, desc));
		}
	}
	return ret;
}

template <typename V, typename T, typename G, typename I>
std::vector<std::pair<T, V> > getSubTrees(const G& g, I minDepth, I maxDepth) {
	if ( maxDepth < minDepth ) maxDepth = minDepth;
    
    // Enumerate all subtrees as pairs of a root node of a subtree (2nd) and 
    // a copy of the subtree (1st).
	std::vector<std::pair<G, typename boost::graph_traits<G>::vertex_descriptor> > temp;
	boost::depth_first_search(g, boost::visitor(dfs_subtree_visitor<G>(temp, minDepth, maxDepth)));
    
    // Convert G to T.
	std::vector<std::pair<T, V> > ret;
	for (auto it = temp.begin(); it != temp.end(); it++) {
		ret.push_back(std::make_pair(T(it->first), V(&g, it->second)));
	}
	return ret;
}


template <typename VertexType, typename RetT, typename PatT>
std::vector<VertexType> find(RetT& tree, PatT& pattern, bool single = false)
{
	std::vector<VertexType> ret;
	typedef typename RetT::VertexDesc VertexDesc;
	typedef typename RetT::GraphType  GraphType;
	const GraphType& g = tree.getGraph();
	std::stack<VertexDesc> c;
	c.push(tree.getRoot());
	auto p = pattern.getRoot();
	while ( !c.empty() ) {
		VertexDesc u = c.top();
		//std::cout << g[u].label << std::endl;
		if ( g[u].label == p->label ) {
			std::string set = Garnet::getSExpression<std::string>(u, g);
			std::string sep = pattern.getSExpression(p);
			if ( set.compare(sep) == 0 ) {
				ret.push_back(VertexType(&const_cast<GraphType&>(g), u));
				if ( single ) return ret;
			}
		}
		c.pop();

		std::stack<VertexDesc> c2;
		typename GraphType::adjacency_iterator  it, end;
		for (boost::tie(it, end) = boost::adjacent_vertices(u, g); it != end; it++) {
			c2.push(*it);
		}
		while ( !c2.empty() ) { // gurantee for left-most child to be popped first.
			c.push(c2.top());
			c2.pop();
		}
	}
	return ret;
}

template <typename Vertex>
Garnet::Tree::GraphType replace(const Garnet::Tree::GraphType& g, const Vertex& u, const Garnet::Tree::GraphType& newtree)
{
	Garnet::Tree::GraphType gt;
	boost::depth_first_search(g, boost::visitor(impl::dfs_replace_visitor<Garnet::Tree::GraphType>(u, gt, newtree)));
	return gt;
}

template <typename Vertex>
Garnet::Tree::GraphType replace(const Garnet::Tree::GraphType& g, const Vertex& u, const Garnet::Tree::GraphType& newtree, Garnet::Tree::GraphType& oldsubtree)
{
	Garnet::Tree::GraphType gt;
	boost::depth_first_search(g, boost::visitor(impl::dfs_replace_and_save_visitor<Garnet::Tree::GraphType>(u, gt, newtree, oldsubtree)));
	return gt;
}

}// end of namespace impl

Garnet::Tree::Tree(const std::string& sexp)
{
	_g = impl::createGraphFromSExpression<GraphType>(sexp);
	_sexp = sexp;
	_isBuilt = false;
}

void Garnet::Tree::resetGraph(const std::string& sexp)
{
	_g = impl::createGraphFromSExpression<GraphType>(sexp);
	_sexp = sexp;
	_isBuilt = false;
}

void Garnet::Tree::rebuild() const
{
	// Get root vertex.
	std::vector<VertexDesc> verts;
	boost::topological_sort(_g, std::back_inserter(verts));
	_root = *verts.rbegin();

	// Count up leaves. (=nodes with no children)
	_numLeaves = 0;
	boost::graph_traits<GraphType>::vertex_iterator it, end;
	for (boost::tie(it, end) = boost::vertices(_g); it != end; it++) {
		if ( boost::out_degree(*it, _g) == 0 ) {
			_numLeaves++;
		}
	}

	//
	_isBuilt = true;
	_sexp = getSExpression();
}

Garnet::Tree::ValueType   Garnet::Tree::getNumVertices() const
{
	return (ValueType)boost::num_vertices(_g);
}

Garnet::Tree::ValueType   Garnet::Tree::getNumLeaves() const
{
	if ( !_isBuilt ) rebuild();
	return _numLeaves;
}

Garnet::Tree::ValueType   Garnet::Tree::getDepth() const
{
	uint32_t  depth = 0;
	boost::depth_first_search(_g, boost::visitor(impl::dfs_depth_visitor(depth)));
	return depth;
}

Garnet::Tree::ValueType   Garnet::Tree::getDepth(const VertexDesc& u) const
{
	// TODO
	uint32_t  depth = 0;
	boost::depth_first_search(_g, boost::visitor(impl::dfs_depth_visitor(depth)));
	return depth;
}

Garnet::Tree::VertexType Garnet::Tree::getVertex(const Garnet::Tree::ValueType& pos)
{
	return VertexType(&_g, boost::vertex(pos, _g));
}

Garnet::Tree::ConstVertexType Garnet::Tree::getVertex(const Garnet::Tree::ValueType& pos) const
{
	return ConstVertexType(&_g, boost::vertex(pos, _g));
}

Garnet::Tree::VertexType Garnet::Tree::getRoot()
{
	if ( !_isBuilt ) rebuild();
	return VertexType(&_g, _root);
}

Garnet::Tree::ConstVertexType Garnet::Tree::getRoot() const
{
	if ( !_isBuilt ) rebuild();
	return ConstVertexType(&_g, _root);
}

Garnet::Tree::StringType  Garnet::Tree::getSExpression() const
{
	_sexp = Garnet::getSExpression<StringType>(getRoot(), _g);
	return _sexp;
}

Garnet::Tree::StringType  Garnet::Tree::getSExpression(const Garnet::Tree::VertexDesc& u) const
{
	_sexp = Garnet::getSExpression<StringType>(u, _g);
	return _sexp;
}

Garnet::Tree::StringType  Garnet::Tree::getSExpression(const Garnet::Tree::VertexDesc& u, const Garnet::Tree::VertexDesc& v) const
{
	_sexp = Garnet::getSExpression<StringType>(u, v, _g);
	return _sexp;
}

Garnet::Tree::StringType  Garnet::Tree::getSExpression(const Garnet::Tree::VertexDesc& u, const std::vector<Garnet::Tree::VertexDesc>& v) const
{
	_sexp = Garnet::getSExpression<StringType>(u, v, _g);
	return _sexp;
}

std::vector<Garnet::Tree::ConstVertexType> Garnet::Tree::getAllLeaves() const
{
	return impl::getAllLeaves<ConstVertexType, const GraphType>(_g);
}

std::vector<Garnet::Tree::VertexType> Garnet::Tree::getAllLeaves()
{
	return impl::getAllLeaves<VertexType, GraphType>(_g);
}

#if 0
std::vector<std::pair<Garnet::Tree, Garnet::Tree::VertexType> > Garnet::Tree::getSubTrees(Garnet::Tree::ValueType minDepth, Garnet::Tree::ValueType maxDepth)
{
	return impl::getSubTrees<VertexType, Tree>(_g, minDepth, maxDepth);
}
#endif

std::vector<std::pair<Garnet::Tree, Garnet::Tree::ConstVertexType> > Garnet::Tree::getSubTrees(Garnet::Tree::ValueType minDepth, Garnet::Tree::ValueType maxDepth) const
{
	return impl::getSubTrees<ConstVertexType, Tree>(_g, minDepth, maxDepth);
}

#if 0
std::vector<std::pair<Garnet::Tree, Garnet::Tree::VertexType> > Garnet::Tree::getAllSubTrees()
{
	return impl::getSubTrees<VertexType, Tree>(_g, ValueType(0), std::numeric_limits<ValueType>::max());
}
#endif

std::vector<std::pair<Garnet::Tree, Garnet::Tree::ConstVertexType> > Garnet::Tree::getAllSubTrees() const
{
	return impl::getSubTrees<ConstVertexType, Tree>(_g, ValueType(0), std::numeric_limits<ValueType>::max());
}

void  Garnet::Tree::replace(const Garnet::Tree::VertexType& u, const Garnet::Tree& newtree)
{
    GraphType gt = impl::replace(_g, u, newtree.getGraph());
	boost::swap(_g, gt);
	_sexp = getSExpression();
}

void  Garnet::Tree::replace(const Garnet::Tree::ConstVertexType& u, const Garnet::Tree& newtree)
{
    GraphType gt = impl::replace(_g, u, newtree.getGraph());
	boost::swap(_g, gt);
	_sexp = getSExpression();
}

bool  Garnet::Tree::replace(const std::string& label, const Garnet::Tree& newtree)
{
	auto verts = boost::vertices(_g);
	for (auto it = verts.first; it != verts.second; it++) {
		if (Text::equals(_g[*it].label, label)) {
			const auto& u = *it;
			GraphType gt = impl::replace(_g, u, newtree.getGraph());
			boost::swap(_g, gt);
			_sexp = getSExpression();
			return true;
		}
	}
	return false;
}

bool  Garnet::Tree::replace(const std::string& label, const Garnet::Tree& newtree, Garnet::Tree& oldSubTree)
{
	auto verts = boost::vertices(_g);
	for (auto it = verts.first; it != verts.second; it++) {
		if (Text::equals(_g[*it].label, label)) {
			const auto& u = *it;
			GraphType gt = impl::replace(_g, u, newtree.getGraph(), oldSubTree.getGraph());
			boost::swap(_g, gt);
			_sexp = getSExpression();
			return true;
		}
	}
	return false;
}

void  Garnet::Tree::replaceFirst(const Garnet::Tree& pattern, const Garnet::Tree& newTree)
{
	StringType sret = getSExpression();
	StringType spat = pattern.getSExpression();
	StringType srep = newTree.getSExpression();
	StringType::size_type pos = 0;
	if ( (pos = sret.find(spat, pos)) != StringType::npos ) {
		sret.replace(pos, spat.size(), srep);
	}
	_g = impl::createGraphFromSExpression<GraphType>(sret);
	_sexp = getSExpression();
	_isBuilt = false;
}

void  Garnet::Tree::replaceAll(const Garnet::Tree& pattern, const Garnet::Tree& newTree)
{
	StringType sret = getSExpression();
	StringType spat = pattern.getSExpression();
	StringType srep = newTree.getSExpression();
	std::stack<StringType::size_type> positions;
	StringType::size_type pos = 0;
	while ( (pos = sret.find(spat, pos)) != StringType::npos ) {
		positions.push(pos);
		pos += spat.size();
	}
	while ( !positions.empty() ) {
		pos = positions.top();
		positions.pop();
		sret.replace(pos, spat.size(), srep);
	}
	_g = impl::createGraphFromSExpression<GraphType>(sret);
	_sexp = getSExpression();
	_isBuilt = false;
}

uint32_t  Garnet::Tree::replaceAll(const std::string& label, const Garnet::Tree& newtree)
{
	uint32_t count = 0;
	auto verts = boost::vertices(_g);
	for (auto it = verts.first; it != verts.second; it++) {
		if (Text::equals(_g[*it].label, label)) {
			const auto& u = *it;
			GraphType gt = impl::replace(_g, u, newtree.getGraph());
			boost::swap(_g, gt);
			verts = boost::vertices(_g);
		}
	}
	if (count > 0) {
		_sexp = getSExpression();
	}
	return count;
}

std::vector<Garnet::Tree::VertexType>  Garnet::Tree::findFirstOf(const Garnet::Tree& pattern)
{
	return impl::find<VertexType>(*this, pattern, true);
}

std::vector<Garnet::Tree::ConstVertexType>  Garnet::Tree::findFirstOf(const Garnet::Tree& pattern) const
{
	return impl::find<ConstVertexType>(*this, pattern, true);
}

std::vector<Garnet::Tree::VertexType>  Garnet::Tree::find(const Garnet::Tree& pattern)
{
	return impl::find<VertexType>(*this, pattern);
}

std::vector<Garnet::Tree::ConstVertexType>  Garnet::Tree::find(const Garnet::Tree& pattern) const
{
	return impl::find<ConstVertexType>(*this, pattern);
}


void Garnet::swapSubTrees(Garnet::Tree& treeA, uint32_t posA, Garnet::Tree& treeB, uint32_t posB) {
	std::string rootA = treeA.getSExpression(treeA.getRoot(), treeA.getVertex(posA));
	std::string rootB = treeB.getSExpression(treeB.getRoot(), treeB.getVertex(posB));
	std::string childA = treeA.getSExpression(treeA.getVertex(posA));
	std::string childB = treeB.getSExpression(treeB.getVertex(posB));

	if (Garnet::isProcessTrace()) {
		std::cout << boost::format("\n[%s] rootA:        %s") % __FUNCTION__ % rootA;
		std::cout << boost::format("\n[%s] rootB:        %s") % __FUNCTION__ % rootB;
		std::cout << boost::format("\n[%s] childA:       %s") % __FUNCTION__ % childA;
		std::cout << boost::format("\n[%s] childB:       %s") % __FUNCTION__ % childB;
	}

	rootA.replace(rootA.find("(?)"), 3, childB);
	rootB.replace(rootB.find("(?)"), 3, childA);

	if (Garnet::isProcessTrace()) {
		std::cout << boost::format("\n[%s] rootA+childB: %s") % __FUNCTION__ % rootA;
		std::cout << boost::format("\n[%s] rootB+childA: %s") % __FUNCTION__ % rootB;
	}

	// 3. Put trees.
	treeA.resetGraph(rootA);
	treeB.resetGraph(rootB);
}
