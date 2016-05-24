
#include <Garnet/BasicStrategy.h>

#include <Garnet/StrategyCatalog.h>
#include <Garnet/StrategyFactory.h>
#include <Garnet/BasicStrategyClasses.h>

//---------------------------------------------------------
//---------------------------------------------------------
namespace {

class BasicStrategyFactory : public Garnet::StrategyFactory {
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

class BasicStrategyCatalog : public Garnet::StrategyCatalog {
public:
    virtual void loadItems(Garnet::FactoryInserter& r) const;
};

//---------------------------------------------------------
//---------------------------------------------------------

const BasicStrategyFactory _factory;
const BasicStrategyCatalog _catalog;

//---------------------------------------------------------
//---------------------------------------------------------

void  BasicStrategyFactory::resolvePreset(const std::string& name, Garnet::StrategyFactory::StageNames& stageNames) const {
    const std::array<std::string, 1> names = {
        "Default",
    };

    switch (Text::getIndex(names, name)) {
    case 0 :
        stageNames[Garnet::StageType::Initialization] = "RampedHalfAndHalf";
        stageNames[Garnet::StageType::Evaluation] = "NoEvaluation";
        stageNames[Garnet::StageType::Sort] = "Basic";
        stageNames[Garnet::StageType::Selection] = "Roulette";
        stageNames[Garnet::StageType::Termination] = "NumberOfGenerations";
        stageNames[Garnet::StageType::PairMaking] = "UniformRandom";
        stageNames[Garnet::StageType::Crossover] = "OnePoint";
        stageNames[Garnet::StageType::Mutation] = "Random";
        break;
    default :
        break;
    }
}

Garnet::Initializer*  BasicStrategyFactory::createInitializer(const std::string& name) const {
    const std::array<std::string, 3> names = {
        "Grow",
        "Full",
        "RampedHalfAndHalf",
    };

    switch (Text::getIndex(names, name)) {
    case 0 :
        return new Garnet::BasicStrategy::GrowInitializer();
    case 1:
        return new Garnet::BasicStrategy::FullInitializer();
    case 2:
        return new Garnet::BasicStrategy::RampedHalfAndHalfInitializer();
    default :
        return NULL;
    }
}

Garnet::Evaluator*  BasicStrategyFactory::createEvaluator(const std::string& name) const {
    const std::array<std::string, 1> names = {
        "NoEvaluation",
    };

    switch (Text::getIndex(names, name)) {
    case 0:
        return new Garnet::BasicStrategy::NoEvaluation();
    default:
        return NULL;
    }
}

Garnet::Sorter*  BasicStrategyFactory::createSorter(const std::string& name) const {
    const std::array<std::string, 1> names = {
        "Basic",
    };

    switch (Text::getIndex(names, name)) {
    case 0 :
        return new Garnet::BasicStrategy::BasicSorter();
    default :
        return NULL;
    }
}

Garnet::Selector*  BasicStrategyFactory::createSelector(const std::string& name) const {
    const std::array<std::string, 2> names = {
        "Roulette",
        "BinaryTournament",
    };

    switch (Text::getIndex(names, name)) {
    case 0:
        return new Garnet::BasicStrategy::RouletteSelector();
    case 1:
        //return new Garnet::BasicStrategy::BinaryTournamentSelector();
        return NULL;
    default:
        return NULL;
    }
}

Garnet::Terminator*  BasicStrategyFactory::createTerminator(const std::string& name) const {
    const std::array<std::string, 1> names = {
        "NumberOfGenerations",
    };

    switch (Text::getIndex(names, name)) {
    case 0:
        return new Garnet::BasicStrategy::MaxGenerationTerminator();
    default:
        return NULL;
    }
}

Garnet::PairMaker*  BasicStrategyFactory::createPairMaker(const std::string& name) const {
    const std::array<std::string, 1> names = {
        "UniformRandom",
    };

    switch (Text::getIndex(names, name)) {
    case 0:
        return new Garnet::BasicStrategy::UniformRandomPairMaker();
    default:
        return NULL;
    }
}

Garnet::CrossoverOperator*  BasicStrategyFactory::createCrossoverOperator(const std::string& name) const {
    const std::array<std::string, 3> names = {
        "OnePoint",
    };

    switch (Text::getIndex(names, name)) {
    case 0:
        return new Garnet::BasicStrategy::OnePointCrossover();
    default:
        return NULL;
    }
}

Garnet::MutationOperator*  BasicStrategyFactory::createMutationOperator(const std::string& name) const {
    const std::array<std::string, 2> names = {
        "NoMutation",
        "Random",
    };

    switch (Text::getIndex(names, name)) {
    case 0:
        return new Garnet::BasicStrategy::NoMutation();
    case 1:
        return new Garnet::BasicStrategy::RandomMutation();
    default:
        return NULL;
    }
}

void BasicStrategyCatalog::loadItems(Garnet::FactoryInserter& r) const {
    using namespace Garnet;

    *r++ = std::make_pair(std::make_pair(StageType::Preset, "Default"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::Initialization, "RampledHalfAndHalf"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::Initialization, "Grow"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::Initialization, "Full"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::Evaluation, "NoEvaluation"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::Sort, "Basic"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::Selection, "Roulette"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::Selection, "BinaryTournament"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::Termination, "NumberOfGeneration"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::PairMaking, "UniformRandom"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::Crossover, "OnePoint"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::Mutation, "Random"), &_factory);
    *r++ = std::make_pair(std::make_pair(StageType::Mutation, "NoMutation"), &_factory);
}

}//end of local namespace

//---------------------------------------------------------
//---------------------------------------------------------

const Garnet::StrategyCatalog& Garnet::BasicStrategy::catalog = _catalog;
