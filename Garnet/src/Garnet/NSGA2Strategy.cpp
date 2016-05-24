
#include <Garnet/NSGA2Strategy.h>

#include <Garnet/StrategyCatalog.h>
#include <Garnet/StrategyFactory.h>
#include <Garnet/NSGA2StrategyClasses.h>

//---------------------------------------------------------
//---------------------------------------------------------
namespace {

class NSGA2StrategyFactory : public Garnet::StrategyFactory {
    virtual bool                        resolvePreset(const std::string& name, StageNames& stageNames) const;

    virtual Garnet::Initializer*        createInitializer(const std::string& name, const Configuration& conf) const;
    virtual Garnet::Evaluator*          createEvaluator(const std::string& name, const Configuration& conf) const;
    virtual Garnet::Sorter*             createSorter(const std::string& name, const Configuration& conf) const;
    virtual Garnet::Selector*           createSelector(const std::string& name, const Configuration& conf) const;
    virtual Garnet::Terminator*         createTerminator(const std::string& name, const Configuration& conf) const;
    virtual Garnet::PairMaker*          createPairMaker(const std::string& name, const Configuration& conf) const;
    virtual Garnet::CrossoverOperator*  createCrossoverOperator(const std::string& name, const Configuration& conf) const;
    virtual Garnet::MutationOperator*   createMutationOperator(const std::string& name, const Configuration& conf) const;
};

//---------------------------------------------------------
//---------------------------------------------------------

const NSGA2StrategyFactory _factory;

//---------------------------------------------------------
//---------------------------------------------------------

bool  NSGA2StrategyFactory::resolvePreset(const std::string& name, Garnet::StrategyFactory::StageNames& stageNames) const {
    const std::array<std::string, 6> names = {
        "NSGA-II",
        "NSGA-II-Strong",
        "NSGA-II-Fooding",
        "NSGA-II-SP",
        "NSGA-II-SP-Fooding",
        "NSGA-II-Strong-SP-Fooding",
    };

    switch (Text::getIndex(names, name)) {
    case 0:
        stageNames[Garnet::StageType::Initialization] = "RampedHalfAndHalf";
        stageNames[Garnet::StageType::Evaluation] = "IVTree";
        stageNames[Garnet::StageType::Sort] = "FastNonDominated";
        stageNames[Garnet::StageType::Selection] = "BinaryTournamentCCO";
        stageNames[Garnet::StageType::Termination] = "NumberOfGenerations";
        stageNames[Garnet::StageType::PairMaking] = "UniformRandom";
        stageNames[Garnet::StageType::Crossover] = "OnePoint";
        stageNames[Garnet::StageType::Mutation] = "Random";
        break;
    case 1:
        stageNames[Garnet::StageType::Initialization] = "RampedHalfAndHalf";
        stageNames[Garnet::StageType::Evaluation] = "IVTree";
        stageNames[Garnet::StageType::Sort] = "StrongFastNonDominated";
        stageNames[Garnet::StageType::Selection] = "BinaryTournament";
        stageNames[Garnet::StageType::Termination] = "NumberOfGenerations";
        stageNames[Garnet::StageType::PairMaking] = "UniformRandom";
        stageNames[Garnet::StageType::Crossover] = "OnePoint";
        stageNames[Garnet::StageType::Mutation] = "Random";
        break;
    case 2:
        stageNames[Garnet::StageType::Initialization] = "RampedHalfAndHalf";
        stageNames[Garnet::StageType::Evaluation] = "IVTree";
        stageNames[Garnet::StageType::Sort] = "FastNonDominated";
        stageNames[Garnet::StageType::Selection] = "CarryingCapacity";
        stageNames[Garnet::StageType::Termination] = "NumberOfGenerations";
        stageNames[Garnet::StageType::PairMaking] = "UniformRandom";
        stageNames[Garnet::StageType::Crossover] = "OnePoint";
        stageNames[Garnet::StageType::Mutation] = "Random";
        break;
    case 3:
        stageNames[Garnet::StageType::Initialization] = "RampedHalfAndHalf";
        stageNames[Garnet::StageType::Evaluation] = "IVTree";
        stageNames[Garnet::StageType::Sort] = "SPFastNonDominated";
        stageNames[Garnet::StageType::Selection] = "BinaryTournament";
        stageNames[Garnet::StageType::Termination] = "NumberOfGenerations";
        stageNames[Garnet::StageType::PairMaking] = "UniformRandom";
        stageNames[Garnet::StageType::Crossover] = "OnePoint";
        stageNames[Garnet::StageType::Mutation] = "Random";
        break;
    case 4:
        stageNames[Garnet::StageType::Initialization] = "RampedHalfAndHalf";
        stageNames[Garnet::StageType::Evaluation] = "IVTree";
        stageNames[Garnet::StageType::Sort] = "SPFastNonDominated";
        stageNames[Garnet::StageType::Selection] = "CarryingCapacity";
        stageNames[Garnet::StageType::Termination] = "NumberOfGenerations";
        stageNames[Garnet::StageType::PairMaking] = "UniformRandom";
        stageNames[Garnet::StageType::Crossover] = "OnePoint";
        stageNames[Garnet::StageType::Mutation] = "Random";
        break;
    case 5:
        stageNames[Garnet::StageType::Initialization] = "RampedHalfAndHalf";
        stageNames[Garnet::StageType::Evaluation] = "IVTree";
        stageNames[Garnet::StageType::Sort] = "StrongSPFastNonDominated";
        stageNames[Garnet::StageType::Selection] = "CarryingCapacity";
        stageNames[Garnet::StageType::Termination] = "NumberOfGenerations";
        stageNames[Garnet::StageType::PairMaking] = "UniformRandom";
        stageNames[Garnet::StageType::Crossover] = "OnePoint";
        stageNames[Garnet::StageType::Mutation] = "Random";
        break;
    default:
        return false;
    }
}

Garnet::Sorter*  NSGA2StrategyFactory::createSorter(const std::string& name, const Configuration& conf) const {
    const std::array<std::string, 4> names = {
        "FastNonDominated",
        "StrongFastNonDominated",
        "SPFastNonDominated",
        "StrongSPFastNonDominated",
    };

    switch (Text::getIndex(names, name)) {
    case 0:
        return new Garnet::NSGA2Strategy::FastNonDominatedSorter();
    case 1:
        return new Garnet::NSGA2Strategy::StrongFastNonDominatedSorter();
    case 2:
        return new Garnet::NSGA2Strategy::SPFastNonDominatedSorter();
    case 3:
        return new Garnet::NSGA2Strategy::StrongSPFastNonDominatedSorter();
    default:
        return NULL;
    }
}

Garnet::Selector*  NSGA2StrategyFactory::createSelector(const std::string& name, const Configuration& conf) const {
    const std::array<std::string, 1> names = {
        "BinaryTournamentCCO",
    };

    switch (Text::getIndex(names, name)) {
    case 0:
        return new Garnet::NSGA2Strategy::BinaryTournamentSelector();
    case 1:
        //return new Garnet::NSGA2Strategy::BinaryTournamentSelector();
        return NULL;
    default:
        return NULL;
    }
}

}//end of local namespace

//---------------------------------------------------------
//---------------------------------------------------------

const Garnet::StrategyFactory& Garnet::NSGA2Strategy::factory = _factory;
