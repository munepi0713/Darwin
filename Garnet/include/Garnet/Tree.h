/*********************************************************
 *  Garnet
 *  Author: Munehiro Doi.
 *********************************************************/
#ifndef GARNET__TREE__H__
#define GARNET__TREE__H__

#include <string>
#include <utility>
#include <vector>
#include <set>
#include <stack>
#include <iostream>
#include <array>

#include <boost/graph/adjacency_list.hpp>

namespace Garnet
{

// Utilities

/**
 *  Make S-Expression of the graph from the vertex u.
 */
template <typename String, typename Graph, typename Vertex, typename Formatter>
String  getSExpression(const Vertex& u, const Graph& g, const Formatter& formatter)
{
	String ret;
	ret += "(";
	ret += formatter(u, g);

	typename Graph::adjacency_iterator  it, end;
	for (boost::tie(it, end) = boost::adjacent_vertices(u, g); it != end; it++) {
		ret += getSExpression<String>(*it, g, formatter);
	}
	ret += ")";
	return ret;
}

/**
 *  Make S-Expression of the graph from the vertex u.
 */
template <typename String, typename Graph, typename Vertex, typename It, typename Formatter>
String  getSExpression(const Vertex& u, const It& termFirst, const It& termLast, const Graph& g, const Formatter& formatter)
{
	if ( std::binary_search(termFirst, termLast, u) ) return "(?)";

	String ret;
	ret += "(";
	ret += formatter(u, g);

	typename Graph::adjacency_iterator  it, end;
	for (boost::tie(it, end) = boost::adjacent_vertices(u, g); it != end; it++) {
		ret += getSExpression<String>(*it, termFirst, termLast, g, formatter);
	}
	ret += ")";
	return ret;
}

/**
 *  Make S-Expression of the graph from the vertex u.
 */
template <typename String, typename Graph, typename Vertex>
String  getSExpression(const Vertex& u, const Graph& g)
{
#if 1
	return getSExpression<String>(u, g, [](const boost::graph_traits<Graph>::vertex_descriptor& u, const Graph& g) {
		return g[u].label;
	});
#else
	String ret;
	ret += "(";
	ret += g[u].label;

	typename Graph::adjacency_iterator  it, end;
	for (boost::tie(it, end) = boost::adjacent_vertices(u, g); it != end; it++) {
		ret += getSExpression<String>(*it, g);
	}
	ret += ")";
	return ret;
#endif
}

/**
 *  Make S-Expression of the graph from the vertex u to the vertex v.
 */
template <typename String, typename Graph, typename Vertex>
String  getSExpression(const Vertex& u, const Vertex& v, const Graph& g)
{
#if 1
	std::array<Vertex, 1> a = { v };
	return getSExpression<String>(u, a.begin(), a.end(), g, [](const boost::graph_traits<Graph>::vertex_descriptor& u, const Graph& g) {
		return g[u].label;
	});
#else
	if ( u == v ) return "(?)";

	String ret;
	ret += "(";
	ret += g[u].label;

	typename Graph::adjacency_iterator  it, end;
	for (boost::tie(it, end) = boost::adjacent_vertices(u, g); it != end; it++) {
		ret += getSExpression<String>(*it, v, g);
	}
	ret += ")";
	return ret;
#endif
}

/**
 *  Make S-Expression of the graph from the vertex u to the vertices v.
 */
template <typename String, typename Graph, typename Vertex>
String  getSExpression(const Vertex& u, const std::vector<Vertex>& v, const Graph& g)
{
#if 1
	return getSExpression<String>(u, v.begin(), v.end(), g, [](const boost::graph_traits<Graph>::vertex_descriptor& u, const Graph& g) {
		return g[u].label;
	});
#else
	if ( std::binary_search(v.begin(), v.end(), u) ) return "(?)";

	String ret;
	ret += "(";
	ret += g[u].label;

	typename Graph::adjacency_iterator  it, end;
	for (boost::tie(it, end) = boost::adjacent_vertices(u, g); it != end; it++) {
		ret += getSExpression<String>(*it, v, g);
	}
	ret += ")";
	return ret;
#endif
}

namespace detail
{
	
struct VProp
{
    int16_t     index;
    std::string label;
    int16_t     color;
    int16_t     depth;

    VProp() : index(-1), color(-1), depth(-1) {}
    VProp(const VProp& a) : index(a.index), label(a.label), color(a.color), depth(a.depth) {}
};

template <typename G>
class ConstVAccessor {
public :
	typedef G  GraphType;
	typedef typename boost::graph_traits<GraphType>::vertex_descriptor VertexDesc;
	typedef typename GraphType::vertex_bundled  VertexProp;

	ConstVAccessor() : _g(0), _desc(0) {};
    ConstVAccessor(const GraphType* g, const VertexDesc& desc) : _g(g), _desc(desc) {};
	ConstVAccessor(const ConstVAccessor& a) : _g(a._g), _desc(a._desc) {};

	const VertexProp* operator->() const { return &(*_g)[_desc]; }
	operator VertexDesc () const { return _desc; }

	ConstVAccessor& operator=(const ConstVAccessor& a) { if ( this != &a ) { _g = a._g; _desc = a._desc; } return *this; }
protected :
	const GraphType*   _g;
	VertexDesc         _desc;
};

template <typename G>
class VAccessor : public ConstVAccessor<G> {
public :
	typedef ConstVAccessor<G>  MyBase;
	typedef typename MyBase::GraphType  GraphType;
	typedef typename MyBase::VertexDesc VertexDesc;
	typedef typename MyBase::VertexProp VertexProp;

	VAccessor() {};
    VAccessor(GraphType* g, VertexDesc desc) : MyBase(g, desc) {};

    VertexProp* operator->() { return &(*const_cast<GraphType*>(MyBase::_g))[MyBase::_desc]; }
	//operator VertexDesc& () { return const_cast<VertexDesc>(_desc); }
private :
};

typedef boost::adjacency_list<
		boost::listS,
		boost::vecS,
		boost::directedS,
		VProp,
		boost::no_property,
		boost::no_property
	> GraphType;
typedef boost::graph_traits<GraphType>::vertex_descriptor VertexDesc;
typedef boost::graph_traits<GraphType>::vertex_descriptor EdgeDesc;
typedef uint32_t                  ValueType;
typedef std::string               StringType;
typedef detail::ConstVAccessor<GraphType>  ConstVertexType;
typedef detail::VAccessor<GraphType>       VertexType;

}//end of namespace detail

/**
 *  The class represents Tree (Single rooted DAG).
 *  based on boost::adjacency_list.
 */
class  Tree
{
public :
	typedef detail::GraphType  GraphType;
	typedef detail::ValueType  ValueType;
	typedef detail::StringType  StringType;
	typedef detail::ConstVertexType  ConstVertexType;
	typedef detail::VertexType  VertexType;
	typedef detail::VertexDesc  VertexDesc;
	typedef detail::VProp       VertexProp;
	typedef detail::EdgeDesc  EdgeDesc;
	
	Tree() : _isBuilt(false) {}
	/** Create Tree object from S-Expression */
	Tree(const std::string& sexp);
	/** Copy constructor */
	Tree(const Tree& t) : _g(t._g), _isBuilt(false), _sexp(t._sexp) {}
	/** Construct from GraphType */
	Tree(const GraphType& g) : _g(g), _isBuilt(false) { rebuild(); }
    
    /** */
    GraphType&  getGraph() { return _g; }
    const GraphType & getGraph() const { return _g; }

	void resetGraph(const std::string& sexp);

	operator const GraphType & () const {
		return _g;
	}
	
	/** Return the number of vertices in this graph. */
	ValueType   getNumVertices() const;
	/** Return the number of leaves in this graph. */
	ValueType   getNumLeaves() const;
	/** Return the maximum depth of this DAG. */
	ValueType   getDepth() const;
	/** Return the depth of the specified vertex. */
	ValueType   getDepth(const VertexDesc& u) const;
	/** Return a vertex of the specified position. */
	VertexType getVertex(const ValueType& pos);
	ConstVertexType getVertex(const ValueType& pos) const;
	/** Return data of the root vertex. */
	VertexType getRoot();
	ConstVertexType getRoot() const;
	/** Return a S expression of this tree. */
	StringType  getSExpression() const;
	/** Return a S expression of this tree from vertex u. */
	StringType  getSExpression(const VertexDesc& u) const;
	/** Return a S expression of this tree from vertex u, 
	    and removed under vertex v and replace it to (?). */
	StringType  getSExpression(const VertexDesc& u, const VertexDesc& v) const;
	/** Return a S expression of this tree from vertex u, 
	    and removed under any of v and replace it to (?). */
	StringType  getSExpression(const VertexDesc& u, const std::vector<VertexDesc>& v) const;
	/** Return all leaves.*/
	std::vector<ConstVertexType> getAllLeaves() const;
	std::vector<VertexType> getAllLeaves();
	/** Return a subtree from the specified vertex. */
	Tree getSubTree(const VertexType& u) const;
	Tree getSubTree(const ConstVertexType& u) const;
	/**
	 *  Return subtrees of "tree" which depth is in specified range.
	 *  ex.) If "tree" is (A(B(C)(D))),
	 *         min=0, max=0, returns the whole tree,
	 *         min=1, max=1, returns subtrees with depth=1, (B(C)(D)),
	 *         min=2, max=2, returns subtrees with depth=1, (C) and (D),
	 *         min=1, max=2, returns subtrees with depth=[1,2], (B(C)(D)), (C) and (D).
	 *
	 *  @return Pairs of a subtree and its root node.
	 *
	 *  @param[in]  tree  : Source tree.
	 *  @param[in]  depth : Subtree Depth. (default: 1)
	 */
    #if 0
	std::vector<std::pair<Tree, VertexType> > getSubTrees(ValueType minDepth = 1, ValueType maxDepth = 0);
    #endif
	std::vector<std::pair<Tree, ConstVertexType> > getSubTrees(ValueType minDepth = 1, ValueType maxDepth = 0) const;
	/** Return all subtrees */
    #if 0
	std::vector<std::pair<Tree, VertexType> > getAllSubTrees();
    #endif
	std::vector<std::pair<Tree, ConstVertexType> > getAllSubTrees() const;
	/**
	 *  Replace a subtree with an another tree.
	 *  @param[in]  u       A node to be replaced.
	 *  @param[in]  newtree A tree to replace the subtree topped by u.
	 */
	void  replace(const VertexType& u, const Tree& newtree);
	void  replace(const ConstVertexType& u, const Tree& newtree);
	bool  replace(const std::string& label, const Tree& newtree);
	bool  replace(const std::string& label, const Tree& newtree, Tree& oldSubTree);
	/**
	 *  Replace a subtree with an another tree.
	 *  If newtree has arguments denoted by (_1), (_2), ... or (_9), 
	 *  subtrees of u will be used as subtrees of newtree.
	 *  ex.1) Replacing u C in (B(C)) by newtree (X) will result to (B(X)).
	 *  ex.2) Replaceng u C in (B(C)) by newtree (X(A)) will result to (B(X(A))).
	 *  ex.3) Replaceng u B in (B(D)) by newtree (X(_1)(A)) will result to (X(D)(A))
	 *  ex.3) Replaceng u B in (B(C)(D)) by newtree (X(_1)(_2)) will result to (X(C)(D))
	 *  ex.3) Replaceng u B in (B(C)(D)) by newtree (X(_2)(_1)) will result to (X(D)(C))
	 *  ex.4) Replaceng u B in (B(D)) by newtree (X(_1)(A(_1))) will result to (X(D)(A(D)))
	 *  @param[in]  u       A node to be replaced.
	 *  @param[in]  newtree A tree to replace the subtree topped by u.
	*/
	void  replaceWithArgs(const VertexType& u, const Tree& newtree);
	void  replaceWithArgs(const ConstVertexType& u, const Tree& newtree);
	/**
	 *  Replace the first subtree matching to the pattern to newTree.
	 */
	void  replaceFirst(const Tree& pattern, const Tree& newtree);
	/**
	 *  Replace subtrees matching to the pattern to newTree.
	 */
	void  replaceAll(const Tree& pattern, const Tree& newTree);
	uint32_t  replaceAll(const std::string& label, const Garnet::Tree& newtree);
	/**
	 *  Find subtrees which has same structure as the pattern's.
	 *  @return  Vertices of subtrees matches the pattern.
	 */
	std::vector<VertexType>  findFirstOf(const Tree& pattern);
	std::vector<ConstVertexType>  findFirstOf(const Tree& pattern) const;
	std::vector<VertexType>  find(const Tree& pattern);
	std::vector<ConstVertexType>  find(const Tree& pattern) const;
	/** Print the tree as a S-Expresson string */
	void put(std::ostream& os) const {
		os << getSExpression();
	}

protected :
	GraphType           _g;
	mutable bool        _isBuilt;
	mutable ValueType   _numLeaves;
	mutable VertexDesc  _root;
	mutable StringType  _sexp;

	/** Rebuild all static information for caching purpose */
	void rebuild() const;
};

typedef std::vector<Tree>  IVTreeList;

void swapSubTrees(Tree& treeA, uint32_t posA, Tree& treeB, uint32_t posB);


} // end of namespace Garnet

inline
std::ostream& operator << (std::ostream& os, const Garnet::Tree& g) {
	g.put(os);
	return os;
}

template <typename SumType>
SumType check_sum_cast(const Garnet::Tree& g) {
	SumType sum = 0;
	const std::string& sexp = g.getSExpression();
	for (auto it = sexp.cbegin(); it != sexp.cend(); it++) {
		sum += *it;
	}
	return sum;
}

#endif//GARNET__TREE__H__

