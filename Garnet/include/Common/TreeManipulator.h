/*********************************************************
 *  Garnet
 *  Author: Munehiro Doi.
 *********************************************************/

#ifndef GARNET__TREE_MANIPULATOR__H__
#define GARNET__TREE_MANIPULATOR__H__

#include <string>
#include <utility>
#include <vector>
#include <set>
#include <stack>

namespace TreeManipulator
{

/**
 *  Get the first identifier from a tree.
 *  If there is a tree like (A(B(C)(D))), the first identifier is A.
 *  Or if the tree is (Img(Src)(Dest)), the first identifier is Img.
 *
 *  @return Identifier as string.
 *
 *  @param[in]  tree : Source tree.
 */
static
std::string getIdentifier(const std::string& tree)
{
    if ( tree.empty() ) return "";
    
    std::string ret;
    unsigned int left = 0, right;
    
    // Scan until '(' appears.
    while ( tree[left] != '(' && !(left >= tree.size()) ) left++;
    left++;
    
    // Scan until a letter except for alphabet, digit, etc. appears.
    right = left;
    while ( (isalnum(tree[right]) || tree[right] == '_' || tree[right] == '.') && !(right >= tree.size()) ) right++;
    
    // 
    if ( right != left )
    {
        ret = tree.substr(left, right - left);
    }
    
    return ret;
}

/**
 *  Retrieve subtrees of "tree" which depth are equal to depth.
 *  ex.) If "tree" is (A(B(C)(D))), the whole tree for depth 0, (B(C)(D)) for depth 1, (C) and (D) for depth 2.
 *
 *  @return Pairs of start position of a subtree and its length.
 
 *  @param[in]  tree  : Source tree.
 *  @param[in]  depth : Subtree Depth. (default: 1)
 */
static
std::vector<std::pair<int, std::string> >  getSubTrees(const std::string& tree, const int depth = 1)
{
	std::stack<unsigned int> posStack;
    std::vector<std::pair<int, std::string> > ret;

	for (unsigned int i = 0; i < tree.size(); i++) {
		if ( tree[i] == '(' ) {
			posStack.push(i);
		}
		else if ( tree[i] == ')' ) {
			unsigned int pos = posStack.top();
			posStack.pop();

			if ( posStack.size() == (unsigned int)depth ) {
				ret.push_back(std::pair<int, std::string>(pos, tree.substr(pos, i - pos + 1)));
			}
		}
	}
    return ret;
}

/**
 *  Return a maximum depth of a tree.
 *  ex.) (A) -> 1. (A(B)(C)) -> 2. (A(B(D))(C)) -> 3.
 */
static
unsigned int getMaxDepth(const std::string& tree)
{
    unsigned int maxDepth = 0;
    unsigned int curDepth = 0;
    
    for (unsigned int i = 0; i < tree.size(); i++) {
        if      ( tree[i] == '(' ) curDepth++;
        else if ( tree[i] == ')' ) curDepth--;
        
        if ( curDepth > maxDepth ) maxDepth = curDepth;
    }
    
    return maxDepth;
}


/**
 *  Return the number of nodes in a source tree.
 *  ex.) (A) -> 1. (A(B)(C)) -> 3. (A(B(D))(C)) -> 4.
 */
static
unsigned int getNodeCount(const std::string& tree)
{
    unsigned int nodeCount = 0;
    
    for (unsigned int i = 0; i < tree.size(); i++)
    {
        if ( tree[i] == '(' ) nodeCount++;
    }
    
    return nodeCount;
}

/**
 *  Return all sub trees.
 */
static
std::vector<std::string>  getAllSubTrees(const std::string& tree)
{
	std::vector<std::string> ret;

	std::stack<unsigned int> pos;
	for (unsigned int i = 0; i < tree.size(); i++)
	{
		if ( tree[i] == '(' )
		{
			pos.push(i);
		}
		else if ( tree[i] == ')' )
		{
			unsigned int left = pos.top();
			pos.pop();
			ret.push_back(tree.substr(left, i - left + 1));
		}
	}

	return ret;
}

/**
 *  Return all leaves.
 */
static
std::set<std::string>  getAllLeaves(const std::string& tree)
{
	std::set<std::string> ret;

	unsigned int left = 0, right;
	bool isInside = true;
	for (unsigned int i = 1; i < tree.size(); i++)
	{
		if ( tree[i] == '(' )
		{
			left = i;
			isInside = true;
		}
		else if ( tree[i] == ')' )
		{
			right = i;
			if ( isInside )
			{
				ret.insert(tree.substr(left, right - left + 1));
				isInside = false;
			}
		}
	}
	
	return ret;
}

/**
 *  Retrieve subtrees of "tree" which depth have the specified size.
 *  ex.) If "tree" is (A(B(C)(D))), size=1 are (C) and (D), size=2 is (B(C)(D)) and size=3 is (A(B(C)(D))).
 *
 *  @return Pairs of start position of a subtree and its length.
 *
 *  @param[in]  tree : Source tree.
 *  @param[in]  size : Subtree Size
 */
static
std::vector<std::string>  getSubTreesWithSize(const std::string& tree, const unsigned int size)
{
	std::vector<std::string> ret = getAllSubTrees(tree);
	std::vector<std::string>::iterator it = ret.begin();
	while ( it != ret.end() ) {
		if ( getMaxDepth(*it) != size ) {
			it = ret.erase(it);
		}
		else {
			it++;
		}
	}
	
	return ret;
}

/**
 *
 */
static
std::string  replaceAll(const std::string& src, const std::string& current, const std::string& replaceTo)
{
	std::string ret = src;
	std::string::size_type pos = 0;
	while ( (pos = ret.find(current, pos)) != std::string::npos ) {
		ret = ret.replace(pos, current.length(), replaceTo);
	}
	return ret;
}

} // end of namespace TreeManipulator

#endif//GARNET__TREE_MANIPULATOR__H__
