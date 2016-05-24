#ifndef GARNET__DEFAULT_TREE_BUILDER__H__
#define GARNET__DEFAULT_TREE_BUILDER__H__

#include <Garnet/TreeBuilder.h>

namespace Garnet
{

class DefaultTreeBuilder : public TreeBuilder
{
public :
	DefaultTreeBuilder(const BuildingBlocks& buildingBlocks);
	virtual ~DefaultTreeBuilder();

	virtual Tree generate(uint32_t maxDepth, uint32_t minSize);

private :
};

}//end of namespace Garnet

#endif//GARNET__TREE_BUILDER__H__
