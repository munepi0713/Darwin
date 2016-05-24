
#include <Garnet/ExperimentalStrategy.h>

#include <Garnet/ExperimentalStrategyClasses.h>
#include <Garnet/StrategyCatalog.h>
#include <Garnet/StrategyFactory.h>

//---------------------------------------------------------
//---------------------------------------------------------
namespace {

class ExperimentalStrategyFactory : public Garnet::StrategyFactory {
    virtual void                resolvePreset(const std::string& name, StageNames& stageNames) const;

    virtual Garnet::Initializer*        createInitializer(const std::string& name) const;
    virtual Garnet::Evaluator*          createEvaluator(const std::string& name) const;
    virtual Garnet::Sorter*             createSorter(const std::string& name) const;
    virtual Garnet::Selector*           createSelector(const std::string& name) const;
    virtual Garnet::Terminator*         createTerminator(const std::string& name) const;
    virtual Garnet::PairMaker*          createPairMaker(const std::string& name) const;
    virtual Garnet::CrossoverOperator*  createCrossoverOperator(const std::string& name) const;
    virtual Garnet::MutationOperator*   createMutationOperator(const std::string& name) const;
};

class ExperimentalStrategyCatalog : public Garnet::StrategyCatalog {
public:
    virtual void loadItems(Garnet::FactoryInserter& r) const;
};

//---------------------------------------------------------
//---------------------------------------------------------

const ExperimentalStrategyFactory _factory;
const ExperimentalStrategyCatalog _catalog;

//---------------------------------------------------------
//---------------------------------------------------------

void  ExperimentalStrategyFactory::resolvePreset(const std::string& name, Garnet::StrategyFactory::StageNames& stageNames) const {
    const std::array<std::string, 1> names = {
        "CarryingCapacity",
    };

    switch (Text::getIndex(names, name)) {
    case 0:
        stageNames[Garnet::StageType::Initialization] = "RampedHalfAndHalf";
        stageNames[Garnet::StageType::Evaluation] = "IVTree";
        stageNames[Garnet::StageType::Sort] = "FastNonDominated";
        stageNames[Garnet::StageType::Selection] = "CarryingCapacity";
        stageNames[Garnet::StageType::Termination] = "NumberOfGenerations";
        stageNames[Garnet::StageType::PairMaking] = "UniformRandom";
        stageNames[Garnet::StageType::Crossover] = "OnePoint";
        stageNames[Garnet::StageType::Mutation] = "Random";
        break;
    default:
        break;
    }
}

Garnet::Sorter*  ExperimentalStrategyFactory::createSorter(const std::string& name) const {
    const std::array<std::string, 1> names = {
        "MinimulGenerationGap",
    };

    switch (Text::getIndex(names, name)) {
    case 0 :
        return new Garnet::ExperimentalStrategy::MinimulGenerationGapSorter();
    default :
        return NULL;
    }
}

Garnet::Selector*  ExperimentalStrategyFactory::createSelector(const std::string& name) const {
    const std::array<std::string, 3> names = {
        "CarryingCapacity",
        "CarryingCapacity2",
        "MinimulGenerationGap",
    };

    switch (Text::getIndex(names, name)) {
    case 0:
        return new Garnet::ExperimentalStrategy::CarryingCapacitySelector();
    case 1:
        return new Garnet::ExperimentalStrategy::CarryingCapacitySelector2();
    case 2:
        return new Garnet::ExperimentalStrategy::MinimulGenerationGapSelector();
    default:
        return NULL;
    }
}

Garnet::PairMaker*  ExperimentalStrategyFactory::createPairMaker(const std::string& name) const {
    const std::array<std::string, 1> names = {
        "MinimulGenerationGap",
    };

    switch (Text::getIndex(names, name)) {
    case 0:
        return new Garnet::ExperimentalStrategy::MinimulGenerationGapPairMaker();
    default:
        return NULL;
    }
}

void ExperimentalStrategyCatalog::loadItems(Garnet::FactoryInserter& r) const {
    using namespace Garnet;

    *r++ = std::make_pair(std::make_pair(StageType::Sort, "MinimulGenerationGap"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::Selection, "CarryingCapacity"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::Selection, "CarryingCapacity2"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::Selection, "MinimulGenerationGap"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::PairMaking, "MinimulGenerationGap"), &_factory);
}

}//end of local namespace

//---------------------------------------------------------
//---------------------------------------------------------

const Garnet::StrategyCatalog& Garnet::ExperimentalStrategy::catalog = _catalog;
