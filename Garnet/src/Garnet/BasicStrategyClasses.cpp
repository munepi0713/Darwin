
#include <Garnet/BasicStrategyClasses.h>

#include <algorithm>
#include <numeric>
#include <iomanip>
#include <utility>
#include <memory>

#include <boost/format.hpp>

#include <Common/GetTime.h>
#include <Common/PrintRange.h>
#include <Common/Text.h>

#include <Garnet/Garnet.h>
#include <Garnet/RandomGenerator.h>

//---------------------------------------------------------
//---------------------------------------------------------
namespace {
    Garnet::Individual createIndividualByGrow(const Garnet::GPEngine* engine, uint32_t depth, uint32_t Nc) {
        Garnet::Individual ind;
        for (auto c = 0u; c < Nc; c++) {
            ind.chromosome.push_back(engine->getTreeBuilder(0)->generateByFull(depth));
        }
        return ind;
    }
    Garnet::Individual createIndividualByFull(const Garnet::GPEngine* engine, uint32_t depth, uint32_t Nc) {
        Garnet::Individual ind;
        for (auto c = 0u; c < Nc; c++) {
            ind.chromosome.push_back(engine->getTreeBuilder(0)->generateByFull(depth));
        }
        return ind;
    }
}

#define GROW(depth) ret.push_back(createIndividualByGrow(engine, (depth), Nc))
#define FULL(depth) ret.push_back(createIndividualByGrow(engine, (depth), Nc))

Garnet::IndividualList Garnet::BasicStrategy::GrowInitializer::execute(const GPEngine* engine, uint32_t depth, uint32_t count) const {
    IndividualList ret;
    uint32_t Nc = engine->getConfiguration().numChromosomes;

    for (auto i = 0u; i < count; i++) {
        GROW(depth);
    }

    return ret;
}

Garnet::IndividualList Garnet::BasicStrategy::FullInitializer::execute(const GPEngine* engine, uint32_t depth, uint32_t count) const {
    IndividualList ret;
    uint32_t Nc = engine->getConfiguration().numChromosomes;

    for (auto i = 0u; i < count; i++) {
        FULL(depth);
    }

    return ret;
}

Garnet::IndividualList Garnet::BasicStrategy::RampedHalfAndHalfInitializer::execute(const GPEngine* engine, uint32_t depth, uint32_t count) const {
    IndividualList ret;
    uint32_t Nc = engine->getConfiguration().numChromosomes;

    for (auto i = 0u; i < count; i += 10) {
        if (i + 0 < count) GROW(depth * 5 / 5);
        if (i + 1 < count) FULL(depth * 5 / 5);
        if (i + 2 < count) GROW(depth * 2 / 5);
        if (i + 3 < count) FULL(depth * 2 / 5);
        if (i + 4 < count) GROW(depth * 4 / 5);
        if (i + 5 < count) FULL(depth * 4 / 5);
        if (i + 6 < count) GROW(depth * 1 / 5);
        if (i + 7 < count) FULL(depth * 1 / 5);
        if (i + 8 < count) GROW(depth * 3 / 5);
        if (i + 9 < count) FULL(depth * 3 / 5);
    }

    return ret;
}

//void Garnet::BasicStrategy::BasicSorter::execute(const GPEngine* engine, IndividualList& individuals) const 
Garnet::IndividualList Garnet::BasicStrategy::BasicSorter::execute(const GPEngine* engine, const IndividualList& archive, const IndividualList& children) const {
    using namespace Garnet;
    using namespace PicturePerfect;

    // 1. Merge archive and children.
    IndividualList individuals;
    std::copy(children.begin(), children.end(), std::back_inserter(individuals));
    std::transform(archive.begin(), archive.end(), std::back_inserter(individuals), Individual::archive);
    // 1.1 Preserve original ranking.
    for (auto i = 0u; i < children.size(); i++) {
        individuals[i].tempIndex = i;
    }
    for (auto i = children.size(); i < individuals.size(); i++) {
        individuals[i].tempIndex = std::numeric_limits<Garnet::Individual::history_t>::max();
    }

    // 2. Sort in descending order of fintess[0].
    std::stable_sort(
        individuals.begin(), individuals.end(),
        [](const Individual& x, const Individual& y) -> bool {
        return x.fitness[0] > y.fitness[0];
    });
    for (auto i = 0u; i < individuals.size(); i++) {
        individuals[i].rank = i;
    }

    // 3. Truncate to the size of population
    individuals.resize(engine->getConfiguration().populationSize);

    return individuals;
}

std::pair<Garnet::IndividualList, Garnet::IndividualList> Garnet::BasicStrategy::RouletteSelector::execute(const GPEngine* engine, const IndividualList& individuals) const {
    const Garnet::Configuration& conf = engine->getConfiguration();
    std::pair<IndividualList, IndividualList> ret;
    IndividualList& archive = ret.first;
    IndividualList& children = ret.second;

    // 1. Preserve elite (archive)
    if (conf.preserveElite) {
        archive.push_back(individuals[0]);
    }

    // 2. Select individuals by the roulette rule.
    // 2.1 Make roulette.
    //     (Adjusted Fitness) = Fitness[i] / (Total Fitness).
    double total = 0;
    std::for_each(individuals.begin(), individuals.end(),
        [&total](const Individual& x) { total += x.fitness[0]; }
    );
    std::vector<double>  roulette;
    if (total == 0) {
        for (auto i = 0u; i < individuals.size(); i++) {
            roulette.push_back(i * 1.0 / individuals.size());
        }
    }
    else {
        double accum = 0;
        for (auto i = 0u; i < individuals.size(); i++) {
            roulette.push_back(accum);
#if 0 // for debug
            std::cerr << boost::format("[%s] roulette[%u]=%f\n") % __FUNCTION__ % i % accum;
#endif
            accum += individuals[i].fitness[0] / total;
        }
    }

    // 2.2 Select a pie from roulette and select the corresponding 
    //     individual.
    for (auto i = 0u; i < individuals.size(); i++) {
        double rate = rand01();
        auto lower = std::lower_bound(roulette.begin(), roulette.end(), rate);
        auto index = lower - roulette.begin() - 1;

#if 0 // for debug
        std::cerr << boost::format("[%s] rate=%f, selected=%u\n") % __FUNCTION__ % rate % index;
#endif

        children.push_back(individuals[index]);
    }

    return ret;
}

bool Garnet::BasicStrategy::MaxGenerationTerminator::isFinished(const GPEngine* engine, const IndividualList& individuals, int generation) const {
    (void)individuals;
    if (generation >= engine->getConfiguration().maxGeneration) {
        return true;
    }
    return false;
}

std::vector<std::pair<size_t, size_t> > Garnet::BasicStrategy::UniformRandomPairMaker::execute(const GPEngine* engine, const IndividualList& selection) const {
    std::vector<std::pair<size_t, size_t> > ret;
    const uint32_t Ni = engine->getConfiguration().populationSize;
    const uint32_t N = selection.size();
    for (auto i = 0u; i < (Ni + 1) / 2; i++) {
        ret.push_back(std::make_pair(randInteger(N), randInteger(N)));
    }
    return ret;
}

Garnet::IndividualList Garnet::BasicStrategy::OnePointCrossover::execute(const GPEngine* engine, const IndividualList& individuals, const std::vector<std::pair<size_t, size_t> > parents) const {
    const size_t Nc = engine->getConfiguration().numChromosomes;
    const size_t Nf = engine->getConfiguration().fitness.size();

    Garnet::IndividualList ret;

    for (auto p = 0u; p < parents.size(); p++) {
        const size_t a = parents[p].first;
        const size_t b = parents[p].second;

        Garnet::Individual childC;
        Garnet::Individual childD;

        childC.parents[0] = a;
        childC.parents[1] = b;
        childD.parents[0] = b;
        childD.parents[1] = a;
        childC.preRank = std::numeric_limits<Garnet::Individual::history_t>::max();
        childD.preRank = std::numeric_limits<Garnet::Individual::history_t>::max();

        for (auto c = 0u; c < Nc; c++) {
            const Garnet::Chromosome& parentA = individuals[a].chromosome[c];
            const Garnet::Chromosome& parentB = individuals[b].chromosome[c];

            // 1. Determine cut points.
            int cpA = randInteger(parentA.getNumVertices());
            int cpB = randInteger(parentB.getNumVertices());

            // 2. Exchange trees under cut points.
#if 1
            //Garnet::Tree treeC;
            //Garnet::Tree treeD;
            childC.chromosome.push_back(parentA);
            childC.chromosome.back().cross[0] = cpA;
            childC.chromosome.back().cross[1] = cpB;
            childD.chromosome.push_back(parentB);
            childD.chromosome.back().cross[0] = cpB;
            childD.chromosome.back().cross[1] = cpA;
            swapSubTrees(childC.chromosome.back(), cpA, childD.chromosome.back(), cpB);
#else
            std::string rootA = parentA.getSExpression(parentA.getRoot(), parentA.getVertex(cpA));
            std::string rootB = parentB.getSExpression(parentB.getRoot(), parentB.getVertex(cpB));
            std::string childA = parentA.getSExpression(parentA.getVertex(cpA));
            std::string childB = parentB.getSExpression(parentB.getVertex(cpB));

            if (Garnet::isProcessTrace()) {
                std::cout << boost::format("\n[%s] rootA:        %s") % __FUNCTION__ % rootA;
                std::cout << boost::format("\n[%s] rootB:        %s") % __FUNCTION__ % rootB;
                std::cout << boost::format("\n[%s] childA:       %s") % __FUNCTION__ % childA;
                std::cout << boost::format("\n[%s] childB:       %s") % __FUNCTION__ % childB;
            }

            rootA.replace(rootA.find("(?)"), 3, childB);
            rootB.replace(rootB.find("(?)"), 3, childA);

            if (Garnet::isProcessTrace()) {
                std::cout << boost::format("\n[%s] rootA+childB: %s") % __FUNCTION__ % rootA;
                std::cout << boost::format("\n[%s] rootB+childA: %s") % __FUNCTION__ % rootB;
            }

            // 3. Put trees.
            childC.chromosome.push_back(Garnet::Tree(rootA));
            childC.chromosome.back().cross[0] = cpA;
            childC.chromosome.back().cross[1] = cpB;
            childD.chromosome.push_back(Garnet::Tree(rootB));
            childD.chromosome.back().cross[0] = cpB;
            childD.chromosome.back().cross[1] = cpA;
#endif
        }

        childC.fitness.resize(Nf);
        childD.fitness.resize(Nf);

        ret.push_back(childC);
        ret.push_back(childD);
    }
    return ret;
}

void Garnet::BasicStrategy::RandomMutation::execute(const GPEngine* engine, IndividualList& individuals) const {
    const Garnet::Configuration& conf = engine->getConfiguration();
    double rate = conf.mutationRate;
    const size_t Ni = individuals.size();//conf.populationSize;
    const size_t Nc = conf.numChromosomes;

    for (auto i = 0u; i < Ni; i++) {
        for (auto c = 0u; c < Nc; c++) {
            Chromosome& chromosome = individuals[i].chromosome[c];

            // Pick up locus to be mutated.
            std::vector<Garnet::Tree::VertexDesc> mutationLocus;
            for (auto n = 0u; n < chromosome.getNumVertices(); n++) {
                if (rand01() < rate) {
                    mutationLocus.push_back(chromosome.getVertex(n));
                    chromosome.mutate.push_back(n);
                }
            }

            // Get S Expression with all mutating locus replaced with (?).
            std::string sexp = chromosome.getSExpression(chromosome.getRoot(), mutationLocus);
            std::string::size_type pos;
            while ((pos = sexp.find("(?)")) != std::string::npos) {
                sexp.replace(pos, 3, engine->getTreeBuilder(c)->generateBySize(conf.maxInitialDepth).getSExpression());
            }

            chromosome.resetGraph(sexp);
        }
    }
}
