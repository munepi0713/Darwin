/**
 *
 */
#pragma warning(disable: 4819)

#include <algorithm>
#include <numeric>
#include <iomanip>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <Common/PrintRange.h>

#include <Garnet/Garnet.h>
#include <Garnet/TreeBuilder.h>
#include <Garnet/Generation.h>
#include <Garnet/IVTreeStrategy.h>


//---------------------------------------------------------
//---------------------------------------------------------
template <typename It>
uint16_t check_sum(const It& first, const It& last)
{
	uint16_t sum = 0;
	for (It it = first; it != last; it++) {
		sum += check_sum_cast<uint16_t>(*it);
	}
	return sum;
}

//---------------------------------------------------------
//---------------------------------------------------------

Garnet::Generation::Generation()
{
    add("Generation",  generation);
    add("RandomSeed",  randomSeed);

     add("Chromosomes", individuals);
    add("FitnessV",    individuals);
    add("RawFitnessV", individuals);
	add("Rank",        individuals);
}

std::vector<size_t> Garnet::Generation::getBestIndividuals() const
{
	// Check individuals has been sorted.
	if ( !_isEvaluated ) {
		throw "getBestIndividuals() is invoked, but this generation has not been evaluated yet.";
	}

	// Copy individuals which has the minimum ranking.
	std::vector<size_t> ret;
	for (auto i = 0u; i < individuals.size() && individuals[i].rank == individuals.front().rank; i++) {
		ret.push_back(i);
	}
	return ret;
}

void Garnet::Generation::save(std::ostream& os) const
{
	if (Settings::isCompressionEnabled()) {
#define _F(value)  (((value) == 65535) ? (int)-1 : (int)(value))
#define FOR_ALL_R(sentence) for (auto r = 0u; r < ind.rawFitness.size(); r++) { sentence; }
#define FOR_ALL_C(sentence) for (auto c = 0u; c < ind.chromosome.size(); c++) { sentence; }

		os << "Generation    " << generation << "\n";
		os << "RandomSeed    " << randomSeed << "\n";

#if 0
		for (int i = individuals.size() - 1; i >= 0; i--) {
			const Individual& ind = individuals[i];
			os << "\nFitnessV      " << i << ", " << print_range(ind.fitness.cbegin(), ind.fitness.cend(), ", ", double_hex_putter());
			FOR_ALL_R(os << "\nRawFitnessV   " << i << ", " << r << ", " << print_range(ind.rawFitness[r].cbegin(), ind.rawFitness[r].cend(), ", ", double_hex_putter()));
			os << "\n# Parents     " << i << ", " << _F(ind.parents[0]) << ", " << _F(ind.parents[1]);
			FOR_ALL_C(os << "\n# Crossover   " << i << ", " << c << ", " << _F(ind.chromosome[c].cross[0]) << ", " << _F(ind.chromosome[c].cross[1]));
			FOR_ALL_C(os << "\n# Mutation    " << i << ", " << c << ", " << print_range(ind.chromosome[c].mutate.cbegin(), ind.chromosome[c].mutate.cend(), ", "));
			FOR_ALL_C({
				os << "\nChromosomes   " << i << ", " << c << ", " << (boost::format("%04x") % check_sum_cast<uint16_t>(ind.chromosome[c])) << ", " << ind.chromosome[c];
			});
			os << "\n";
		}
#else
#if 1
		auto bulkPrint = [this, &os](void(*printer)(std::ostream& os, int i, const Individual& ind)) {
			for (int i = individuals.size() - 1; i >= 0; i--) {
				const Individual& ind = individuals[i];
				printer(os, i, ind);
			}
		};
#define FOR_ALL_I(body) bulkPrint([](std::ostream& os, int i, const Individual& ind) {\
			body;\
		})

		FOR_ALL_I(os << "\nRank          " << i << ", " << ind.rank);
		FOR_ALL_I(os << "\nFitnessV      " << i << ", " << print_range(ind.fitness.cbegin(), ind.fitness.cend(), ", ", double_hex_putter()));
		FOR_ALL_I(FOR_ALL_R(os << "\nRawFitnessV   " << i << ", " << r << ", " << print_range(ind.rawFitness[r].cbegin(), ind.rawFitness[r].cend(), ", ", double_hex_putter())));
		FOR_ALL_I(os << "\n# Parents     " << i << ", " << _F(ind.parents[0]) << ", " << _F(ind.parents[1]));
		FOR_ALL_I(FOR_ALL_C(os << "\n# Crossover   " << i << ", " << c << ", " << _F(ind.chromosome[c].cross[0]) << ", " << _F(ind.chromosome[c].cross[1])));
		FOR_ALL_I(FOR_ALL_C(os << "\n# Mutation    " << i << ", " << c << ", " << print_range(ind.chromosome[c].mutate.cbegin(), ind.chromosome[c].mutate.cend(), ", ")));
		FOR_ALL_I(FOR_ALL_C({
				os << "\nChromosomes   " << i << ", " << c << ", " << (boost::format("%04x") % check_sum_cast<uint16_t>(ind.chromosome[c])) << ", " << ind.chromosome[c];
			});
			os << "\n");
#else
		for (int i = individuals.size() - 1; i >= 0; i--) {
			const Individual& ind = individuals[i];
			os << "\nRank          " << i << ", " << ind.rank;
		}
		for (int i = individuals.size() - 1; i >= 0; i--) {
			const Individual& ind = individuals[i];
			os << "\nFitnessV      " << i << ", " << print_range(ind.fitness.cbegin(), ind.fitness.cend(), ", ", double_hex_putter());
		}
		for (int i = individuals.size() - 1; i >= 0; i--) {
			const Individual& ind = individuals[i];
			FOR_ALL_R(os << "\nRawFitnessV   " << i << ", " << r << ", " << print_range(ind.rawFitness[r].cbegin(), ind.rawFitness[r].cend(), ", ", double_hex_putter()));
		}
		for (int i = individuals.size() - 1; i >= 0; i--) {
			const Individual& ind = individuals[i];
			os << "\n# Parents     " << i << ", " << _F(ind.parents[0]) << ", " << _F(ind.parents[1]);
		}
		for (int i = individuals.size() - 1; i >= 0; i--) {
			const Individual& ind = individuals[i];
			FOR_ALL_C(os << "\n# Crossover   " << i << ", " << c << ", " << _F(ind.chromosome[c].cross[0]) << ", " << _F(ind.chromosome[c].cross[1]));
		}
		for (int i = individuals.size() - 1; i >= 0; i--) {
			const Individual& ind = individuals[i];
			FOR_ALL_C(os << "\n# Mutation    " << i << ", " << c << ", " << print_range(ind.chromosome[c].mutate.cbegin(), ind.chromosome[c].mutate.cend(), ", "));
		}
		for (int i = individuals.size() - 1; i >= 0; i--) {
			const Individual& ind = individuals[i];
			FOR_ALL_C({
				os << "\nChromosomes   " << i << ", " << c << ", " << (boost::format("%04x") % check_sum_cast<uint16_t>(ind.chromosome[c])) << ", " << ind.chromosome[c];
			});
			os << "\n";
		}
#endif
#endif
		os << std::flush;

#undef _F
	}
	else {
#define _F(value)  (((value) == 65535) ? (int)-1 : (int)(value))
#define FOR_ALL_R(sentence) for (auto r = 0u; r < ind.rawFitness.size(); r++) { sentence; }
#define FOR_ALL_C(sentence) for (auto c = 0u; c < ind.chromosome.size(); c++) { sentence; }

		os << "Generation    " << generation << "\n";
		os << "RandomSeed    " << randomSeed << "\n";

		for (unsigned int i = 0; i < individuals.size(); i++) {
			const Individual& ind = individuals[i];
			os << "\n# Individual  " << i;
			os << "\nRank          " << i << ", " << ind.rank;
			os << "\n# Pre-rank    " << i << ", " << _F(ind.preRank);
			os << "\nFitnessV      " << i << ", " << print_range(ind.fitness.cbegin(), ind.fitness.cend(), ", ", double_hex_putter());
			os << "\n# FVector     " << i << ", " << print_range(ind.fitness.cbegin(), ind.fitness.cend(), ", ", double_digit_putter(10, 6));
			FOR_ALL_R(os << "\nRawFitnessV   " << i << ", " << r << ", " << print_range(ind.rawFitness[r].cbegin(), ind.rawFitness[r].cend(), ", ", double_hex_putter()));
			FOR_ALL_R(os << "\n# Raw Fitness " << i << ", " << r << ", " << ind.rawFitness[r].average() << ", " << print_range(ind.rawFitness[r].cbegin(), ind.rawFitness[r].cend(), ", "));
			FOR_ALL_C(os << "\n# Vertices    " << i << ", " << c << ", " << ind.chromosome[c].getNumVertices());
			os << "\n# Parents     " << i << ", " << _F(ind.parents[0]) << ", " << _F(ind.parents[1]);
			FOR_ALL_C(os << "\n# Crossover   " << i << ", " << c << ", " << _F(ind.chromosome[c].cross[0]) << ", " << _F(ind.chromosome[c].cross[1]));
			FOR_ALL_C(os << "\n# Mutation    " << i << ", " << c << ", " << print_range(ind.chromosome[c].mutate.cbegin(), ind.chromosome[c].mutate.cend(), ", "));
			FOR_ALL_C({
				auto intermediate = ind.chromosome[c].getIntermediate<IVTreeStrategy::IVTreeIntermediate>();
				os << "\n# IGraph      " << i << ", " << c << ", " << (intermediate ? intermediate->igraph : "?");
			});
			FOR_ALL_C({
				auto intermediate = ind.chromosome[c].getIntermediate<IVTreeStrategy::IVTreeIntermediate>();
				os << "\n# VGraph      " << i << ", " << c << ", " << (intermediate ? intermediate->vgraph : "?");
			});
			FOR_ALL_C({
				auto intermediate = ind.chromosome[c].getIntermediate<IVTreeStrategy::IVTreeIntermediate>();
				os << "\n# AGraph      " << i << ", " << c << ", " << (intermediate ? intermediate->agraph : "?");
			});
			FOR_ALL_C({
				os << "\nChromosomes   " << i << ", " << c << ", " << (boost::format("%04x") % check_sum_cast<uint16_t>(ind.chromosome[c])) << ", " << ind.chromosome[c];
			});
			os << "\n";
		}

		// Save script.
		for (auto i = 0u; i < individuals._scripts.size(); i++) {
			auto lines = Text::split(individuals._scripts[i], '\n');
			os << "\n# Script " << i << "\n# " << Text::join(lines.begin(), lines.end(), "\n# ") << "\n# -----";
		}

		os << std::flush;

#undef _F
	}
}

Garnet::Generation  Garnet::Generation::create(const Garnet::GPEnginePtr engine)
{
	// Generate individuals for a generation.
	// Each individual has a tree with the depth equals to or less than the max depth.
    Garnet::Generation generation;
	generation.individuals = engine->getInitializer()->execute(
		engine.get(),
		engine->getConfiguration().maxInitialDepth,
		engine->getConfiguration().populationSize);
    generation.generation = 0;
	generation.randomSeed = randInteger();

    return generation;
}


Garnet::Generation  Garnet::Generation::createFromFile(std::istream& ins)
{
	// Loading is performed by using ConfigLoader interface.
	Generation generation;
	generation.load(ins, 0);
	return generation;
}
