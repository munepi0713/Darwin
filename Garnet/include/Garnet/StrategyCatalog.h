//
//  Copyright(c) 2009-2014 Munehiro Doi
//
#ifndef GARNET__STRATEGY_CATALOG__H__
#define GARNET__STRATEGY_CATALOG__H__

#include <map>
#include <string>
#include <iterator>

#include <Garnet/Strategy.h>

namespace Garnet
{

class StrategyFactory;

typedef std::map<std::pair<StageType, std::string>, const StrategyFactory*> FactoryList;
typedef std::insert_iterator<FactoryList> FactoryInserter;

class StrategyCatalog {
public:
    virtual void loadItems(FactoryInserter& r) const = 0;
};

}// end of namespace Garnet



#endif//GARNET__STRATEGY_CATALOG__H__
