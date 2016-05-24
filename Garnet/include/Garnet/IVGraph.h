#ifndef GARNET__IV_GRAPH__H__
#define GARNET__IV_GRAPH__H__

#include <string>

#include <boost/graph/adjacency_list.hpp>

#include <Common/ConfigLoader.h>

namespace Garnet
{

namespace ivgraph_impl
{

struct VProp {
	std::string label;
	int32_t     tree;
	int         color;

	VProp() : tree(-1), color(-1) {}
	VProp(const VProp& u) : label(u.label), tree(u.tree), color(u.color) {}
};

}//end of namespace ivgraph_impl

class IVGraph : public boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::directedS,
        ivgraph_impl::VProp // VertexProperty
    >, public ConfigLoader
{
public :
	typedef boost::graph_traits<IVGraph>::vertex_descriptor Vertex;
	typedef boost::graph_traits<IVGraph>::edge_descriptor   Edge;
	typedef IVGraph::vertex_property_type                   VProp;

	void  load(std::istream& ins, void* data = 0);
};

}//end of namespace Garnet

#endif//GARNET__PPE_GRAPH__H__
