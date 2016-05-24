
#include <Garnet/IVTreeStrategy.h>

#include <Garnet/StrategyCatalog.h>
#include <Garnet/StrategyFactory.h>
#include <Garnet/IVTreeStrategyClasses.h>

//---------------------------------------------------------
//---------------------------------------------------------
namespace {

class IVTreeStrategyFactory : public Garnet::StrategyFactory {
    virtual Garnet::Evaluator*          createEvaluator(const std::string& name) const;
};

class IVTreeStrategyCatalog : public Garnet::StrategyCatalog {
public:
    virtual void loadItems(Garnet::FactoryInserter& r) const;
};

//---------------------------------------------------------
//---------------------------------------------------------

const IVTreeStrategyFactory _factory;
const IVTreeStrategyCatalog _catalog;

//---------------------------------------------------------
//---------------------------------------------------------

Garnet::Evaluator*  IVTreeStrategyFactory::createEvaluator(const std::string& name) const {
    const std::array<std::string, 1> names = {
        "IVTree",
    };

    switch (Text::getIndex(names, name)) {
    case 0:
        return new Garnet::IVTreeStrategy::IVTreeEvaluator();
    default:
        return NULL;
    }
}

void IVTreeStrategyCatalog::loadItems(Garnet::FactoryInserter& r) const {
    using namespace Garnet;

    *r++ = std::make_pair(std::make_pair(StageType::Evaluation, "IVTree"), &_factory);
}

}//end of local namespace

//---------------------------------------------------------
//---------------------------------------------------------

const Garnet::StrategyCatalog& Garnet::IVTreeStrategy::catalog = _catalog;
