
#include <Garnet/ExperimentalStrategyClasses.h>

#include <algorithm>
#include <numeric>
#include <iomanip>

#include <boost/format.hpp>

#include <Common/GetTime.h>
#include <Common/PrintRange.h>
#include <Common/Mathematical.h>

#include <Garnet/Garnet.h>
#include <Garnet/RandomGenerator.h>

//---------------------------------------------------------
//---------------------------------------------------------

/**
 *  Carrying Capacity Model. (Doi, 2014)
 *
 *  It is a selection method to keep prevent from bloating individuals' size
 *  without explicit size limit.
 *
 *  In the Carrying Capacity Model, 
 *  We assume individuals in a population need VIRTUAL FOOD to live and
 *  necessary amount of virtual food is propotion of the size of an 
 *  individual. 
 *  Total amount of virtual food is given by the balance between the current
 *  best fitness and the average size of individuals.
 *  We assume the average size of the Rank 1
 *  individual is ENOUGH size to achieve the current best fitness.
 *  The average size of the Rank 1 individuals by the population size gives
 *  the total amount of virtual food.
 *  In the individual selection, each individuals consume a same amount of 
 *  virtual food as the size of the individual. If not enough virtual food is
 *  remaining for an individual, the individual cannot survive for the next
 *  generation.
 *  Let us think of the Carrying Capacity Model from the viewpoint of its 
 *  meanings. An individual of smaller size (slim) and better fitness has
 *  more chance to survive. An individual of larger size (fat) and worse
 *  fitness is hard to survive. However, even a Rank 1 individual could be
 *  killed if it is very larger sized than other Rank 1 individuals.
 * 
 *  This algorithm needs to sorting individuals to determine orders to feed
 *  virtual food. The first measurement is the rank. To determine the order 
 *  of individuals which have same rank, we use the Cartesian distance between
 *  the fitness of an individual and the theoritical best fitness.
 *  Fitness will be normalized before sorting because each objective may have 
 *  different value range.
 *
 *  It is applicable any GP method, but it should work better for 
 *  Multi-Objective Optimization.
 */

/**
 *  (2013) first version.
 */
std::pair<Garnet::IndividualList, Garnet::IndividualList> Garnet::ExperimentalStrategy::CarryingCapacitySelector::execute(const GPEngine* engine, const IndividualList& individuals) const {
	const Garnet::Configuration& conf = engine->getConfiguration();
	std::pair<IndividualList, IndividualList> ret;
	IndividualList& archive  = ret.first;
	IndividualList& selected = ret.second;

	const unsigned Ni = individuals.size();
	const unsigned Nf = conf.fitness.size();
	const unsigned Nc = conf.numChromosomes;

	// 1. Preserve elite (archive)
	//    This method preserves all individuals
	for (auto i = 0u; i < individuals.size(); i++) {
		archive.push_back(individuals[i]);
	}

	// 2. Select individuals by the food balance algorithm.

	// 2.1 Calculate adjusted fitness.
	//     (AFi)[x] = sum(Fi[1],..,Fi[Nf]) / Fi[x].  i=individual, x=fitness index.
	int topRank = individuals[0].rank;
	std::vector<double> food(Nf);
	std::vector<double> size(Nf);
	for (auto f = 0u; f < Nf; f++) {
		// Calculate the average fitness of the Rank-1 individuals.
		auto numTopRankers = 0u;
		double average = std::accumulate(individuals.cbegin(), individuals.cend(), 0.0, [&] (double a, const Garnet::Individual& b) {
			if ( b.rank == topRank ) {
				numTopRankers++;
				return a + b.fitness[f];
			}
			else {
				return a;
			}
		});
		average /= numTopRankers;

		food[f] = Nc * std::accumulate(individuals.cbegin(), individuals.cend(), 0.0, [&] (double a, const Garnet::Individual& b) {
			return a + (average / b.fitness[f]);
		});

		size[f] = std::accumulate(individuals.cbegin(), individuals.cend(), 0.0, [&] (double a, const Garnet::Individual& b) {
			if ( b.rank == topRank ) {
				return std::accumulate(b.chromosome.cbegin(), b.chromosome.cend(), a, [&] (double a, const Garnet::Chromosome& b) {
					return a + boost::num_vertices(b.getGraph());
				});
			}
			else {
				return a;
			}
		}) / numTopRankers;
	}

	// 2.2 Select individual if remaining food > feed.
	for (auto i = 0u; i < Ni; i++) {
		// 2.2.1 Calculate amount of feed.
		std::vector<double> feed(Nf);
		for (auto f = 0u; f < Nf; f++) {
			feed[f] = std::accumulate(individuals[i].chromosome.cbegin(), individuals[i].chromosome.cend(), 0.0, [&] (double a, const Garnet::Chromosome& b) {
				return a + boost::num_vertices(b.getGraph());
			}) / size[f];
		}

		// 2.2.2 Check if enough food remaining.
		bool isSelected = true;
		for (auto f = 0u; f < Nf; f++) {
			if ( food[f] < feed[f] ) {
				isSelected = false;
				break;
			}
		}
		if ( isSelected ) {
			selected.push_back(individuals[i]);
			for (auto f = 0u; f < Nf; f++) {
				food[f] -= feed[f];
			}
		}
	}

	std::cout << boost::format("\n[%s] %d selected.") % __FUNCTION__ % selected.size();

	// 2.3 Add randomly generated individuals to fill shortage.
	if ( selected.size() < Ni ) {
#if 1
		auto extention = engine->getInitializer()->execute(engine, conf.maxInitialDepth, Ni - selected.size());
		selected.insert(selected.end(), extention.begin(), extention.end());
#else
		Garnet::TreeBuilderPtr builder = Garnet::TreeBuilder::create(conf.units);
		for (unsigned i = selected.size(); i < Ni; i++) {
			selected.push_back(Garnet::Individual::create(
					builder,
					conf.numChromosomes,
					conf.maxInitialDepth,
					conf.minInitialSize));
		}
#endif
	}

	return ret;
}

/**
 *  Sort by Cartesian distance.
 */
std::pair<Garnet::IndividualList, Garnet::IndividualList> Garnet::ExperimentalStrategy::CarryingCapacitySelector2::execute(const GPEngine* engine, const IndividualList& individuals) const {
	const Garnet::Configuration& conf = engine->getConfiguration();
	std::pair<IndividualList, IndividualList> ret;
	IndividualList& archive = ret.first;
	IndividualList& selected = ret.second;

	const unsigned Ni = individuals.size();
	const unsigned Nf = conf.fitness.size();
	const unsigned Nc = conf.numChromosomes;

	// 1. Preserve elite (archive)
	//    This method preserves all individuals
	for (auto i = 0u; i < individuals.size(); i++) {
		archive.push_back(individuals[i]);
	}

	// 2. Sort
	// 2.1 Calculate normalized fitness.
	//     NormalizedF(i,f) = F(i,f) / sum(F(1,f), ..., F(Ni,f).  i=individual, f=fitness index.
	std::vector<double> sumFitness(Nf); // Sum of each fitness element.
	for (auto f = 0u; f < Nf; f++) {
		sumFitness[f] = std::accumulate(individuals.cbegin(), individuals.cend(), 0.0, [&](double a, const Garnet::Individual& b) {
			return a + b.fitness[f];
		});
	}

	// 2.2 Calculate distance to the theoritical best (the origin O).
	std::vector<double> distance(Ni); // Distance calculated based on normalized fitness.
	for (auto i = 0u; i < Ni; i++) {
		std::vector<double> normalizedFitness(Nf);
		for (auto f = 0u; f < Nf; f++) {
			normalizedFitness[f] = individuals[i].fitness[f] / sumFitness[f];
		}
		distance[i] = std::inner_product(normalizedFitness.begin(), normalizedFitness.end(), normalizedFitness.begin(), 0.0);
	}

	// 2.3 Sort
	std::vector<uint32_t> indices(Ni);
	std::iota(indices.begin(), indices.end(), 0);
	std::stable_sort(indices.begin(), indices.end(), [&](uint32_t a, uint32_t b) {
		return distance[a] < distance[b];
	});

	// 3. Select individuals by the food balance algorithm.
	const int topRank = individuals[0].rank;
	// 3.1 Calculate amount of foods.
	// Calculate the average size of the Rank-1 individuals.
	double averageSize; // Average sizes of each fitness element.
	auto numTopRankers = 0u;
	averageSize = std::accumulate(individuals.cbegin(), individuals.cend(), 0.0, [&](double a, const Garnet::Individual& b) {
		if (b.rank == topRank) {
			numTopRankers++;
			return std::accumulate(b.chromosome.cbegin(), b.chromosome.cend(), a, [&](double a, const Garnet::Chromosome& b) {
				return a + boost::num_vertices(b.getGraph());
			});
		}
		else {
			return a;
		}
	});
	averageSize /= numTopRankers;

	// Calculate amount of foods.
	double food;
	food = averageSize * Ni;

	// 3.2 Select individual if remaining food > feed.
	for (auto i = 0u; i < Ni; i++) {
		// 3.2.1 Feed in order of the distance to the fitness origin.
		const Individual& ind = individuals[indices[i]];

		// 3.2.2 Calculate foods to feed.
		double feed;
		feed = std::accumulate(individuals[i].chromosome.cbegin(), individuals[i].chromosome.cend(), 0.0, [&](double a, const Garnet::Chromosome& b) {
			return a + boost::num_vertices(b.getGraph());
		});

		// 3.2.3 Check if enough food is remaining.
		if (food >= feed) {
			selected.push_back(individuals[i]);
			food -= feed;
		}
	}

	std::cout << boost::format("\n[%s] %d selected. %.3f food remaining.") % __FUNCTION__ % selected.size() % food;

	// 4. Add randomly generated individuals to fill shortage.
	if (selected.size() < Ni) {
		auto extention = engine->getInitializer()->execute(engine, conf.maxInitialDepth, Ni - selected.size());
		selected.insert(selected.end(), extention.begin(), extention.end());
	}

	return ret;
}

/**
 *  [1] H. Sato, I. Ono, and S. Kobayashi,
 *  "A New Generation Alternation Model of Genetic Algoirthm and Its Assesment"
 *  Artificial Intelligence.
 */
std::pair<Garnet::IndividualList, Garnet::IndividualList> Garnet::ExperimentalStrategy::MinimulGenerationGapSelector::execute(const GPEngine* engine, const IndividualList& individuals) const 
{
	std::pair<IndividualList, IndividualList> ret;
	IndividualList& archive  = ret.first;
	IndividualList& selection = ret.second;

	// 1. Select two individuals without duplication.
	auto selectionN = Mathematical::selectN(individuals.size(), 2u, randInteger);
	std::stable_sort(selectionN.begin(), selectionN.end());
	//
	auto it = selectionN.begin();
	for (auto i = 0u; i < individuals.size(); i++) {
		if (it != selectionN.end() && *it == i) {
			// Selected individual is moved to children.
			selection.push_back(individuals[i]);
			it++;
		}
		else {
			// Unselected individual is moved to archive.
			archive.push_back(individuals[i]);
		}
	}
	// 1.2 Append selected individuals at the end of unselected
	//     to use later.
	archive.push_back(selection[0]);
	archive.push_back(selection[1]);

	return ret;
}

std::vector<std::pair<size_t, size_t> > Garnet::ExperimentalStrategy::MinimulGenerationGapPairMaker::execute(const GPEngine* engine, const IndividualList& selection) const 
{
	std::vector<std::pair<size_t, size_t> > ret;
	ret.push_back(std::make_pair(selection.size() - 2, selection.size() - 1));
	ret.push_back(std::make_pair(selection.size() - 1, selection.size() - 2));
	return ret;
}

Garnet::IndividualList Garnet::ExperimentalStrategy::MinimulGenerationGapSorter::execute(const GPEngine* engine, const IndividualList& archive, const IndividualList& children) const 
{
	IndividualList individuals;

	if (!archive.empty()) {
		// 1. Gather candidates of selection.
		std::vector<Individual> candidates;
		candidates.push_back(children[0]);
		candidates.push_back(children[1]);
		candidates.push_back(archive[archive.size() - 2]);
		candidates.push_back(archive[archive.size() - 1]);
		//
		std::stable_sort(candidates.begin(), candidates.end(), [](const Individual& x, const Individual& y) {
			return x.fitness[0] < y.fitness[0];
		});
		// Keep the best individual.
		individuals.push_back(candidates[0]);

		// Choose one more individual based on the roulette rule.
		//   (Roulette ratio) = ind[i].fitness / (total fitness)
		// Create roulette.
		double total = 0;
		std::for_each(candidates.begin(), candidates.end(), [&total](const Individual& x) {
			total += x.fitness[0];
		});
		std::vector<double> roulette(candidates.size(), 0.0);
		if (total == 0) {
			for (auto i = 0u; i < candidates.size(); i++) {
				roulette[i] = i * 1.0 / candidates.size();
			}
		}
		else {
			for (auto i = 1u; i < candidates.size(); i++) {
				roulette[i] = roulette[i - 1] + i * 1.0 / candidates.size();
			}
		}

		// Select
		double rate = rand01();
		auto lower = std::lower_bound(roulette.begin(), roulette.end(), rate);
		auto index = lower - roulette.begin() - 1;
		individuals.push_back(candidates[index]);

		//
		std::transform(archive.begin(), archive.begin() + (archive.size() - 2), std::back_inserter(individuals), Individual::archive);
	}
	else {
		std::copy(children.begin(), children.end(), std::back_inserter(individuals));
	}

	// 1. Merge archive and children.

    // 2. Sort in descending order of fintess[0].
	std::stable_sort(individuals.begin(), individuals.end(), [](const Individual& x, const Individual& y) {
#if 0
		return x.fitness[0] > y.fitness[0];
#else
		return x.fitness[0] < y.fitness[0];
#endif
	});
	for (auto i = 0u; i < individuals.size(); i++) {
		individuals[i].rank = i;
	}

	assert(individuals.size() == engine->getConfiguration().populationSize);

	return individuals;
}
