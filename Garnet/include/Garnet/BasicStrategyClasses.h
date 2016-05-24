#ifndef GARNET__BASIC_STRATEGY_CLASSES__H__
#define GARNET__BASIC_STRATEGY_CLASSES__H__

#include <Garnet/Strategy.h>

/**
*  An implemenation strategy for UniversalEngine,
*  which uses a single unified fitness even if
*  individual could generate mulitple scores.
*  Unifying operation can be specified by
*/
namespace Garnet {

namespace BasicStrategy {

struct GrowInitializer : public Initializer {
    virtual IndividualList execute(const GPEngine* engine, uint32_t depth, uint32_t count) const;
};

struct FullInitializer : public Initializer {
    virtual IndividualList execute(const GPEngine* engine, uint32_t depth, uint32_t count) const;
};

struct RampedHalfAndHalfInitializer : public Initializer {
    virtual IndividualList execute(const GPEngine* engine, uint32_t depth, uint32_t count) const;
};

struct NoEvaluation : public Evaluator {
    virtual void execute(GPEngine* engine, IndividualList& individuals) {};
};

struct BasicSorter : public Sorter {
    //	virtual void execute(const GPEngine* engine, IndividualList& individuals) const;
    virtual IndividualList execute(const GPEngine* engine, const IndividualList& archive, const IndividualList& children) const;
};

struct RouletteSelector : public Selector {
    virtual std::pair<IndividualList, IndividualList> execute(const GPEngine* engine, const IndividualList& individuals) const;
};

struct MaxGenerationTerminator : public Terminator {
    virtual bool isFinished(const GPEngine* engine, const IndividualList& individuals, int generation) const;
};

struct UniformRandomPairMaker : public PairMaker {
    virtual std::vector<std::pair<size_t, size_t> > execute(const GPEngine* engine, const IndividualList& selection) const;
};

struct OnePointCrossover : public CrossoverOperator {
    virtual IndividualList execute(const GPEngine* engine, const IndividualList& individuals, const std::vector<std::pair<size_t, size_t> > parents) const;
};

struct RandomMutation : public MutationOperator {
    virtual void execute(const GPEngine* engine, IndividualList& individuals) const;
};

struct NoMutation : public MutationOperator {
    virtual void execute(const GPEngine* engine, IndividualList& individuals) const {}
};

}//end of namespace BasicStrategy

}// end of namespace Garnet


#endif//GARNET__BASIC_STRATEGY_CLASSES__H__

