#ifndef GARNET__UNIFIED_FITNESS_STRATEGY__H__
#define GARNET__UNIFIED_FITNESS_STRATEGY__H__

#include <vector>
#include <string>
#include <queue>
#include <list>
#include <iostream>

#include <Garnet/ThreadPool.h>

#include <Garnet/TreeBuilder.h>
#include <Garnet/UniversalEngine.h>

namespace Garnet {

/**
 *  An implemenation strategy for UniversalEngine,
 *  which uses a single unified fitness even if 
 *  individual could generate mulitple scores.
 *  Unifying operation can be specified by 
 */
namespace UnifiedFitnessStrategy {

const enum ScoringMethod
{
	SCORE_AVERAGE = 0,
	SCORE_MINIMUM,
	SCORE_MAXIMUM,
	SCORE_MEDIAN,
	SCORE_LOWERSIGMA,
	SCORE_TF_AVERAGE,
	SCORE_TF_MINIMUM,
	SCORE_TF_CORRECT_RATE,
	SCORE_CORRELATION_TO_1,
    SCORE_CLUSTER_2,
};

struct UnifyingSorter : public UniversalEngine::Sorter {
	ScoringMethod  _scoringMethod;
	UnifyingSorter(const std::string& type);
	virtual void execute(const UniversalEngine* engine, IndividualList& individuals) const;
};

struct RouletteSelector : public UniversalEngine::Selector {
	virtual std::pair<IndividualList, IndividualList> execute(const UniversalEngine* engine, const IndividualList& individuals) const;
};

struct Terminator : public UniversalEngine::Terminator {
	virtual bool isFinished(const UniversalEngine* engine, const IndividualList& individuals, int generation) const;
};

struct PairMaker : public UniversalEngine::PairMaker {
	virtual std::vector<std::pair<size_t, size_t> > execute(const UniversalEngine* engine, IndividualList& individuals) const;
};

struct OnePointCrossover : public UniversalEngine::CrossoverOperator {
	virtual IndividualList execute(const UniversalEngine* engine, const IndividualList& individuals, const std::vector<std::pair<size_t, size_t> > parents) const;
};

struct RandomMutation : public UniversalEngine::MutationOperator {
	virtual void execute(const UniversalEngine* engine, IndividualList& individuals) const;
};

}//end of namespace UnifiedFitnessStrategy

}// end of namespace Garnet


#endif//GARNET__UNIFIED_FITNESS_STRATEGY__H__

