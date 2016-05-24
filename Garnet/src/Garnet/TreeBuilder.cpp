#pragma warning(disable: 4819)

#define NOMINMAX

#include <limits>

#include <Garnet/RandomGenerator.h>
#include <Garnet/TreeBuilder.h>

Garnet::TreeBuilder::TreeBuilder(const Garnet::BuildingBlocks& buildingBlocks)
{
	// Given building blocks are stored as 'Tree' instances.

	// 1. Convert building blocks from string type to Tree.
	for (Garnet::BuildingBlocks::const_iterator it = buildingBlocks.begin(); it != buildingBlocks.end(); it++) {
		Tree t(*it);
		_trees.push_back(t);
	}

	// 2. Sort by ary.
	std::sort(_trees.begin(), _trees.end(), [] (const Tree& x, const Tree& y) {
		uint32_t numInputsX = x.find(Tree("(?)")).size();
		uint32_t numInputsY = y.find(Tree("(?)")).size();
		return numInputsX < numInputsY;
	});

	// 3. Check range of n-ary operators.
	uint32_t n = 0;
	_indices.push_back(_trees.cbegin());
	for (auto it = _trees.cbegin(); it != _trees.cend(); it++) {
		uint32_t numInputs = it->find(Tree("(?)")).size();
		if (n == numInputs) continue;
		while (n < numInputs) {
			_indices.push_back(it);
			n++;
		}
	}
	_indices.push_back(_trees.cend()); // centinel

	// There must be one leaf at least.
	if (_indices.size() == 1) {
		throw "There must be one leaf at least.";
	}
}

Garnet::TreeBuilder::~TreeBuilder()
{
}

Garnet::Tree Garnet::TreeBuilder::generateBySize(uint32_t maxDepth, uint32_t minSize)
{
	if (maxDepth == 0) maxDepth = std::numeric_limits<int>::max();

	// Generate initial tree.
	Tree ret;
	do {
		uint32_t index;

		// 1. Select a root node from non-leaf nodes.
		auto range = getInternalNodes();
		index = randInteger(static_cast<int>(range.second - range.first));
		ret = range.first[index];

		// 2. Grow tree.
		//  Find a free position and replace it to one of buiding blocks.
		//  Process positions one by one, because replacing operation breaks current iterator.
		Tree pattern("(?)");
		std::vector<Garnet::Tree::VertexType> m;
		while (!(m = ret.findFirstOf(pattern)).empty()) {
			Tree::VertexType& u = m.front();
			if (ret.getDepth(u) > maxDepth) {
				// Current depth exceeds limit.
				//   Insert a leaf to the current free position.
				//   Random index is selected from subtrees with 0 inputs (=leaves).
				range = getLeaves();
			}
			else {
				// Current depth is under limit.
				// Replace the current free positions to a subtree.
				range = getNodes();
			}
			index = randInteger(static_cast<int>(range.second - range.first));
			ret.replace(u, range.first[index]);
		}
	} while (ret.getNumVertices() < minSize);

	return ret;
}

Garnet::Tree Garnet::TreeBuilder::generateByGrow(uint32_t maxDepth)
{
	if (maxDepth == 0) {
		throw "GROW initializer needs maxDepth.";
	}

	// Generate initial tree.
	Tree ret;
	uint32_t index;

	// 1. Select a root node from non-leaf nodes.
	auto range = getInternalNodes();
	index = randInteger(static_cast<int>(range.second - range.first));
	ret = range.first[index];

	// 2. Grow tree.
	//  Find a free position and replace it to one of buiding blocks.
	//  Process positions one by one, because replacing operation breaks current iterator.
	Tree pattern("(?)");
	std::vector<Garnet::Tree::VertexType> m;
	while (!(m = ret.findFirstOf(pattern)).empty()) {
		Tree::VertexType& u = m.front();
		if (ret.getDepth(u) >= maxDepth) {
			// Current depth reaches the depth limit.
			//   Insert a leaf to the current free position.
			range = getLeaves();
		}
		else {
			// Current depth is under limit.
			// Replace the current free positions to a subtree.
			range = getNodes();
		}
		index = randInteger(static_cast<int>(range.second - range.first));
		ret.replace(u, range.first[index]);
	}

	return ret;
}

Garnet::Tree Garnet::TreeBuilder::generateByFull(uint32_t maxDepth)
{
	if (maxDepth == 0) {
		throw "FULL initializer needs maxDepth.";
	}

	// Generate initial tree.
	Tree ret;
	uint32_t index;

	// 1. Select a root node from non-leaf nodes.
	auto range = getInternalNodes();
	index = randInteger(static_cast<int>(range.second - range.first));
	ret = range.first[index];

	// 2. Grow tree.
	//  Find a free position and replace it to one of buiding blocks.
	//  Process positions one by one, because replacing operation breaks current iterator.
	Tree pattern("(?)");
	std::vector<Garnet::Tree::VertexType> m;
	while (!(m = ret.findFirstOf(pattern)).empty()) {
		Tree::VertexType& u = m.front();
		if (ret.getDepth(u) >= maxDepth) {
			// Current depth reaches the depth limit.
			//   Insert a leaf to the current free position.
			range = getLeaves();
		}
		else {
			// Current depth is under limit.
			//   Insert an internal node to the current free position.
			range = getInternalNodes();
		}
		index = randInteger(static_cast<int>(range.second - range.first));
		ret.replace(u, range.first[index]);
	}

	return ret;
}

Garnet::TreeBuilderPtr  Garnet::TreeBuilder::create(const BuildingBlockList& buildingBlocks)
{
	return TreeBuilderPtr(new Garnet::TreeBuilder(buildingBlocks));
}
