
#include <Garnet/ACTITStrategy.h>

#include <Garnet/StrategyCatalog.h>
#include <Garnet/StrategyFactory.h>

//---------------------------------------------------------
//---------------------------------------------------------
namespace {

class ACTITStrategyFactory : public Garnet::StrategyFactory {
    virtual void                resolvePreset(const std::string& name, StageNames& stageNames) const;
};

class ACTITStrategyCatalog : public Garnet::StrategyCatalog {
public:
    virtual void loadItems(Garnet::FactoryInserter& r) const;
};

//---------------------------------------------------------
//---------------------------------------------------------

const ACTITStrategyFactory _factory;
const ACTITStrategyCatalog _catalog;

//---------------------------------------------------------
//---------------------------------------------------------
void  ACTITStrategyFactory::resolvePreset(const std::string& name, Garnet::StrategyFactory::StageNames& stageNames) const {
    const std::array<std::string, 1> names = {
        "ACTIT",
    };

    switch (Text::getIndex(names, name)) {
    case 0:
        stageNames[Garnet::StageType::Initialization] = "RampedHalfAndHalf";
        stageNames[Garnet::StageType::Evaluation] = "IVTree";
        stageNames[Garnet::StageType::Sort] = "MinimumGenerationGap";
        stageNames[Garnet::StageType::Selection] = "MinimumGenerationGap";
        stageNames[Garnet::StageType::Termination] = "NumberOfGenerations";
        stageNames[Garnet::StageType::PairMaking] = "MinimumGenerationGap";
        stageNames[Garnet::StageType::Crossover] = "OnePoint";
        stageNames[Garnet::StageType::Mutation] = "Random";
        break;
    default:
        break;
    }
}

void ACTITStrategyCatalog::loadItems(Garnet::FactoryInserter& r) const {
    using namespace Garnet;

    *r++ = std::make_pair(std::make_pair(StageType::Preset, "ACTIT"), &_factory);
}

}//end of local namespace

//---------------------------------------------------------
//---------------------------------------------------------

const Garnet::StrategyCatalog& Garnet::ACTITStrategy::catalog = _catalog;
