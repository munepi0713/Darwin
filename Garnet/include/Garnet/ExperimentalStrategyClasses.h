#ifndef GARNET__EXPERIMENTAL_STRATEGY_CLASSES__H__
#define GARNET__EXPERIMENTAL_STRATEGY_CLASSES__H__

#include <Garnet/Strategy.h>

/**
*  An implemenation strategy for UniversalEngine,
*  which uses a single unified fitness even if
*  individual could generate mulitple scores.
*  Unifying operation can be specified by
*/

namespace Garnet {

namespace ExperimentalStrategy {

struct CarryingCapacitySelector : public Selector {
	virtual std::pair<IndividualList, IndividualList> execute(const GPEngine* engine, const IndividualList& individuals) const;
};

struct CarryingCapacitySelector2 : public Selector {
	virtual std::pair<IndividualList, IndividualList> execute(const GPEngine* engine, const IndividualList& individuals) const;
};

struct MinimulGenerationGapSelector : public Selector {
	virtual std::pair<IndividualList, IndividualList> execute(const GPEngine* engine, const IndividualList& individuals) const;
};

struct MinimulGenerationGapPairMaker : public PairMaker {
	virtual std::vector<std::pair<size_t, size_t> > execute(const GPEngine* engine, const IndividualList& selection) const;
};

struct MinimulGenerationGapSorter : public Sorter {
	virtual IndividualList execute(const GPEngine* engine, const IndividualList& archive, const IndividualList& children) const;
};

}//end of namespace ExperimentalStrategy

}// end of namespace Garnet


#endif//GARNET__EXPERIMENTAL_STRATEGY_CLASSES__H__

