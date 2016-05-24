#ifndef GARNET__INDIVIDUAL__H__
#define GARNET__INDIVIDUAL__H__

#include <vector>
#include <string>

#include <Common/ConfigLoader.h>
#include <PicturePerfectEngine/Vector.h>
#include <Garnet/Chromosome.h>
#include <Garnet/TreeBuilder.h>

namespace Garnet
{

/**
 *  Individual
 *    Represents a filter-tree, which can consists of one or more trees.
 */
struct Individual
{
	typedef std::vector<PicturePerfect::Vector<double> > RawFitness;

	ChromosomeList       chromosome;  // n trees.
	std::vector<double>  fitness;     //< fitness vector. The size should be zero before evaluation has been done.
	int                  rank;        // rank.
	int                  preRank;     // rank in previous generation.
	RawFitness           rawFitness;  // Raw Fitness

	Individual() {
		parents[0] = std::numeric_limits<Garnet::Individual::history_t>::max();
		parents[1] = std::numeric_limits<Garnet::Individual::history_t>::max();
		preRank    = std::numeric_limits<Garnet::Individual::history_t>::max();
	}
	Individual(const Individual& x) 
		: chromosome(x.chromosome), 
		  fitness(x.fitness),
		  rank(x.rank),
		  preRank(x.preRank),
		  rawFitness(x.rawFitness) {
		parents[0] = x.parents[0];
		parents[1] = x.parents[1];
		preRank    = x.preRank;
	}

	/** True if raw fitness has been evaluated. */
	bool  isEvaluated() const {
		return !rawFitness.empty();
	}

	// History member
	typedef uint16_t history_t;
	history_t  parents[2]; // Parent indices in the previous generation.
	history_t  tempIndex;  // Index when evaluation.

#if 0
	static Individual create(const Garnet::TreeBuilderPtr builder, uint32_t numChromosomes, uint32_t maxInitialDepth, uint32_t minInitialSize);
#endif

	static Individual archive(const Individual& x);
};

class IndividualList : public ConfigLoader::Value, public std::vector<Individual>
{
public:
	std::vector<std::string> _scripts;

    virtual void load(const strings& tokens);
};

}//end of namespace Garnet

inline
Garnet::IndividualList operator + (const Garnet::IndividualList& a, const Garnet::IndividualList& b)
{
	Garnet::IndividualList iList(a);
	iList.insert(iList.end(), b.begin(), b.end());
	return iList;
}

#endif//INDIVIDUAL__H__
