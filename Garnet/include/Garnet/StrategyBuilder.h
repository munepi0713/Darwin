//
//  Copyright(c) 2009-2014 Munehiro Doi
//
#ifndef GARNET__STRATEGY_BUILDER__H__
#define GARNET__STRATEGY_BUILDER__H__

#include <queue>
#include <string>

#include <Garnet/GPEngine.h>

namespace Garnet
{

class GPEngineBuilder {
public:
    GPEngineBuilder&  use(const StrategyFactory& factory);

    GPEngineBuilder&  setConfiguration(const Configuration& conf);

    GPEngineBuilder&  setRandomSeed(long seed);
    GPEngineBuilder&  setPopulationSize(uint32_t size);
    GPEngineBuilder&  setNumChromosomes(uint32_t num);

    GPEngineBuilder&  setPreset(const std::string& name);
    GPEngineBuilder&  setInitializer(const std::string& name);
    GPEngineBuilder&  setEvaluator(const std::string& name);
    GPEngineBuilder&  setSorter(const std::string& name);
    GPEngineBuilder&  setSelector(const std::string& name);
    GPEngineBuilder&  setTerminator(const std::string& name);
    GPEngineBuilder&  setPairMaker(const std::string& name);
    GPEngineBuilder&  setCrossoverOperator(const std::string& name);
    GPEngineBuilder&  setMutationOperator(const std::string& name);

    GPEnginePtr       create();

private :
    GPEnginePtr                         _engine;
    const Configuration*                _conf;
    std::vector<const StrategyFactory*> _factories;
};

}// end of namespace Garnet

#endif//GARNET__STRATEGY_BUILDER__H__
