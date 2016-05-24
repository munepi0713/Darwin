#ifndef GARNET___PPE_GRAPH__H__
#define GARNET___PPE_GRAPH__H__

#include <string>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>

#include <Common/ConfigLoader.h>

namespace Garnet
{

namespace ppegraph_impl
{
struct VProp {
	std::string label;
	int         color; // This is used for an inbound edge of this vertex.
	uint32_t    type;
	//int32_t     tree;

	VProp() : label(""), color(-1), type(0xfff)
	//	, tree(-1) 
	{}
	VProp(const VProp& x) : label(x.label), color(x.color), type(x.type)
	//, tree(x.tree) 
	{}
};

}//end of namespace ppegraph_impl

class PPEGraph : public boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::directedS,
        ppegraph_impl::VProp // VertexProperty
    >, public ConfigLoader
{
public :
	typedef boost::graph_traits<PPEGraph>::vertex_descriptor Vertex;
	typedef boost::graph_traits<PPEGraph>::edge_descriptor   Edge;
	typedef PPEGraph::vertex_property_type                   VProp;

	void  load(std::istream& ins, void* data = 0);
};

}//end of namespace Garnet

#endif//GARNET___PPE_GRAPH__H__
