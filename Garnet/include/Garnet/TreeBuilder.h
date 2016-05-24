#ifndef GARNET__TREE_BUILDER__H__
#define GARNET__TREE_BUILDER__H__

#include <memory>
#include <string>

#include <Garnet/Configuration.h>
#include <Garnet/Tree.h>

namespace Garnet
{

class TreeBuilder;
typedef std::shared_ptr<TreeBuilder>  TreeBuilderPtr;

/**
 * Corresponds to a set of building blocks.
 */
class TreeBuilder
{
public :
	virtual ~TreeBuilder();

	/**
	 *  Generate a tree.
	 */
	virtual Tree generateBySize(uint32_t maxDepth = 0, uint32_t minSize = 2);
	virtual Tree generateByGrow(uint32_t maxDepth);
	virtual Tree generateByFull(uint32_t maxDepth);

	static TreeBuilderPtr  create(const BuildingBlockList& buildingBlocks);

protected :
	TreeBuilder(const Garnet::BuildingBlocks& buildingBlocks);

private :
	std::vector<Tree>  _trees;            // Trees (including leaves) as building blocks.

	/** _indices[k] is the first index of k-ary nodes. */
	std::vector<std::vector<Tree>::const_iterator> _indices;

	std::pair<std::vector<Tree>::const_iterator, std::vector<Tree>::const_iterator> getNodes(uint32_t minAry, uint32_t maxAry) const {
		return std::make_pair(_indices[minAry], _indices[maxAry]);
	}
	std::pair<std::vector<Tree>::const_iterator, std::vector<Tree>::const_iterator> getNodes() const {
		return getNodes(0, static_cast<uint32_t>(_indices.size() - 1));
	}
	std::pair<std::vector<Tree>::const_iterator, std::vector<Tree>::const_iterator> getLeaves() const {
		return getNodes(0, 1);
	}
	std::pair<std::vector<Tree>::const_iterator, std::vector<Tree>::const_iterator> getInternalNodes() const {
		return getNodes(1, static_cast<uint32_t>(_indices.size() - 1));
	}
	std::pair<std::vector<Tree>::const_iterator, std::vector<Tree>::const_iterator> getNaryNodes(uint32_t n) const {
		return getNodes(n, n + 1);
	}
};

}//end of namespace Garnet

#endif//GARNET__TREE_BUILDER__H__
