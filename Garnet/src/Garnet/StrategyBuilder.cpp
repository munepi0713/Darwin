
#define  _CRT_SECURE_NO_WARNINGS


#include <Garnet/StrategyBuilder.h>

#include <Garnet/GPEngine.h>
#include <Garnet/StrategyFactory.h>

const Garnet::StrategyFactory*  Garnet::StrategyBuilder::findFactory(Garnet::StageType type, const std::string& name) const {
    auto key = std::make_pair(type, name);
    auto it = std::find_if(_factories.cbegin(), _factories.cend(), [key](const FactoryList::value_type& x) {
        return key.first == x.first.first && key.second == x.first.second;
    });
    if (it != _factories.cend()) {
        return it->second;
    }
    else {
        return NULL;
    }
}


void  Garnet::StrategyBuilder::use(const Garnet::StrategyCatalog& catalog) {
    catalog.loadItems(FactoryInserter(_factories, _factories.end()));
}

void  Garnet::StrategyBuilder::setPreset(const std::string& name) {
    _commands.push(std::make_pair(StageType::Preset, name));
}

void  Garnet::StrategyBuilder::setInitializer(const std::string& name) {
    _commands.push(std::make_pair(StageType::Initialization, name));
}

void  Garnet::StrategyBuilder::setEvaluator(const std::string& name) {
    _commands.push(std::make_pair(StageType::Evaluation, name));
}

void  Garnet::StrategyBuilder::setSorter(const std::string& name) {
    _commands.push(std::make_pair(StageType::Sort, name));
}

void  Garnet::StrategyBuilder::setSelector(const std::string& name) {
    _commands.push(std::make_pair(StageType::Selection, name));
}

void  Garnet::StrategyBuilder::setTerminator(const std::string& name) {
    _commands.push(std::make_pair(StageType::Preset, name));
}

void  Garnet::StrategyBuilder::setPairMaker(const std::string& name) {
    _commands.push(std::make_pair(StageType::PairMaking, name));
}

void  Garnet::StrategyBuilder::setCrossoverOperator(const std::string& name) {
    _commands.push(std::make_pair(StageType::Crossover, name));
}

void  Garnet::StrategyBuilder::setMutationOperator(const std::string& name) {
    _commands.push(std::make_pair(StageType::Mutation, name));
}

bool  Garnet::StrategyBuilder::apply(Garnet::GPEnginePtr engine) {
    StrategyFactory::StageNames names;

    // Determine the algorithm names for each stage by following
    // queued commands.
    while (!_commands.empty()) {
        auto& command = _commands.front();
        if (command.first == StageType::Preset) {
            auto factory = findFactory(StageType::Preset, command.second);
            if (factory) {
                factory->resolvePreset(command.second, names);
            }
            else {
                // if failed to resolve the specified preset,
                // apply() will fail.
                return false;
            }
        }
        else {
            names[command.first] = command.second;
        }
        _commands.pop();
    }

    // Find the factory for each stage.
    // factorys are objects which creates an instance of the algorithm 
    // specified by |name|.
    const StrategyFactory* factory;

    if ((factory = findFactory(StageType::Initialization, names[StageType::Initialization]))) {
        engine->setInitializer(factory->createInitializer(names[StageType::Initialization]));
    }
    else {
        return false;
    }

    if ((factory = findFactory(StageType::Evaluation, names[StageType::Evaluation]))) {
        engine->setEvaluator(factory->createEvaluator(names[StageType::Evaluation]));
    }
    else {
        return false;
    }

    if ((factory = findFactory(StageType::Sort, names[StageType::Sort]))) {
        engine->setSorter(factory->createSorter(names[StageType::Sort]));
    }
    else {
        return false;
    }

    if ((factory = findFactory(StageType::Selection, names[StageType::Selection]))) {
        engine->setSelector(factory->createSelector(names[StageType::Selection]));
    }
    else {
        return false;
    }

    if ((factory = findFactory(StageType::Termination, names[StageType::Termination]))) {
        engine->setTerminator(factory->createTerminator(names[StageType::Termination]));
    }
    else {
        return false;
    }

    if ((factory = findFactory(StageType::PairMaking, names[StageType::PairMaking]))) {
        engine->setPairMaker(factory->createPairMaker(names[StageType::PairMaking]));
    }
    else {
        return false;
    }

    if ((factory = findFactory(StageType::Crossover, names[StageType::Crossover]))) {
        engine->setCrossoverOperator(factory->createCrossoverOperator(names[StageType::Crossover]));
    }
    else {
        return false;
    }

    if ((factory = findFactory(StageType::Mutation, names[StageType::Mutation]))) {
        engine->setMutationOperator(factory->createMutationOperator(names[StageType::Mutation]));
    }
    else {
        return false;
    }

    return true;
}

