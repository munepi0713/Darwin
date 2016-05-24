//
//  Copyright(c) 2009-2014 Munehiro Doi
//
#ifndef GARNET__STRATEGY_FACTORY__H__
#define GARNET__STRATEGY_FACTORY__H__

#include <map>
#include <string>

#include <Garnet/Configuration.h>
#include <Garnet/Strategy.h>

namespace Garnet
{

class StrategyFactory {
public:
    typedef std::map<StageType, std::string> StageNames;

    virtual bool                resolvePreset(const std::string& name, StageNames& stageNames) const { return false;  }

    virtual Initializer*        createInitializer(const std::string& name, const Configuration& conf) const { return NULL; }
    virtual Evaluator*          createEvaluator(const std::string& name, const Configuration& conf) const { return NULL; }
    virtual Sorter*             createSorter(const std::string& name, const Configuration& conf) const { return NULL; }
    virtual Selector*           createSelector(const std::string& name, const Configuration& conf) const { return NULL; }
    virtual Terminator*         createTerminator(const std::string& name, const Configuration& conf) const { return NULL; }
    virtual PairMaker*          createPairMaker(const std::string& name, const Configuration& conf) const { return NULL; }
    virtual CrossoverOperator*  createCrossoverOperator(const std::string& name, const Configuration& conf) const { return NULL; }
    virtual MutationOperator*   createMutationOperator(const std::string& name, const Configuration& conf) const { return NULL; }
};

}// end of namespace Garnet

#endif//GARNET__STRATEGY_FACTORY__H__
