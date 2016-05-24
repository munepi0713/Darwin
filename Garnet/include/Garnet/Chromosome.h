#ifndef GARNET__CHROMOSOME__H__
#define GARNET__CHROMOSOME__H__

#include <vector>
#include <string>
#include <memory>

#include <Garnet/Tree.h>

namespace Garnet
{

/**
 *  Chromosome
 *    Represents a filter-tree.
 */
struct Chromosome : public Tree {
	class Intermediate
	{
	public :
		virtual ~Intermediate() = 0;
	};
	typedef std::shared_ptr<Intermediate> IntermediatePtr;

	typedef uint16_t history_t;

	Chromosome() : Tree() {
		cross[0] = std::numeric_limits<history_t>::max();
		cross[1] = std::numeric_limits<history_t>::max();
		mutate.clear();
	}
	Chromosome(const Chromosome& x) : Tree(x) { 
		cross[0]     = x.cross[0];
		cross[1]     = x.cross[1];
		mutate       = x.mutate;
		intermediate = x.intermediate;
	}
	Chromosome(const Tree& x) : Tree(x) { 
		cross[0] = std::numeric_limits<history_t>::max();
		cross[1] = std::numeric_limits<history_t>::max();
		mutate.clear();
		intermediate.reset();
	}
	//std::vector<double>  score; /// The score as a result of evaluating this tree.

	// History member
	history_t               cross[2]; // Crossover points.
	std::vector<history_t>  mutate;   // Mutation point.
	//std::string             igraph;
	//std::string             vgraph;
	//std::string             agraph;
	IntermediatePtr         intermediate; // Intermediate result. ex. For IVTree Evaluator, it contains igraph, vgraph, agraph and ivpair.
	template <typename T>
	const std::shared_ptr<T>  getIntermediate() const {
		return std::dynamic_pointer_cast<T>(intermediate);
	}
	template <typename T>
	std::shared_ptr<T>  getIntermediate() {
		if (!intermediate) {
			intermediate = std::shared_ptr<T>(new T);
		}
		return std::dynamic_pointer_cast<T>(intermediate);
	}
};
typedef std::vector<Chromosome> ChromosomeList;

}//end of namespace Garnet

#endif//GARNET__CHROMOSOME__H__
