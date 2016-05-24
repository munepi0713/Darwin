
#include <Garnet/NSGA2StrategyClasses.h>

#include <algorithm>
#include <numeric>
#include <iomanip>

#include <boost/format.hpp>

#include <Common/GetTime.h>
#include <Common/PrintRange.h>
#include <Common/Mathematical.h>

#include <Garnet/Garnet.h>
#include <Garnet/RandomGenerator.h>

namespace {
	
/**
 *  @param[out]  dist is A container which is able to access by [] operator.
 *  @param[in]   fitness is a function. double func(uint32_t individualIndex, uint32_t fitnessIndex)
 */
template <typename Cont, typename Dist, typename Fitness>
void crowding_distance_assignment(Cont I, Dist& dist, Fitness fitness, uint32_t Nf)
{
	// number of solutions in I
	uint32_t Ni = I.size();

	// Initailize distance
	for (auto i = 0u; i < Ni; i++) {
		dist(i) = 0u;
	}

	if (Ni <= 2) return;

	for (auto f = 0u; f < Nf; f++) {
		// Sort using each objective value.
		std::stable_sort(I.begin(), I.end(), [=](uint32_t x, uint32_t y) -> bool {
			return fitness(x, f) < fitness(y, f);
		});

		// So that boundary points are always selected.
		dist(I.front()) = std::numeric_limits<double>::max();
		dist(I.back()) = std::numeric_limits<double>::max();

		// For all other points
		for (auto i = 1u; i < Ni - 1; i++) {
			dist(I[i]) += (fitness(I[i + 1], f) - fitness(I[i - 1], f)) / (fitness(I.back(), f) - fitness(I.front(), f));
		}
	}
}

template <typename Rank, typename Dist, typename Pred>
class CrowdedComparisonOperator
{
public :
	CrowdedComparisonOperator(Rank rank, Dist dist, Pred pred) : _rank(rank), _pred(pred), _dist(dist) {}
	CrowdedComparisonOperator(const CrowdedComparisonOperator& x) : _rank(x._rank), _pred(x._pred), _dist(x._dist) {}

	bool operator()(uint32_t x, uint32_t y) {
		if (_pred(_rank(x), _rank(y))) {
			return true;
		}
		if (_rank(x) == _rank(y) && _dist(x) > _dist(y)) {
			return true;
		}
		return false;
	}

private :
	Rank _rank;
	Dist _dist;
	Pred _pred;
};

template <typename Rank, typename Dist, typename Pred>
CrowdedComparisonOperator<Rank, Dist, Pred> crowded_comparison_operator(Rank rank, Dist dist, Pred pred) {
	return CrowdedComparisonOperator<Rank, Dist, Pred>(rank,  dist, pred);
}

std::map<uint32_t, double> get_crowding_distance(const Garnet::IndividualList& individuals)
{
	// 1. Initialize index list by filling the original positions of each items.
	std::vector<uint32_t> indices(individuals.size());
	for (auto i = 0u; i < individuals.size(); i++) {
		indices[i] = i;
	}

	// 2. Create accessor.
	std::map<uint32_t, double> distMap; // <K,V>=<orignal position, distance>
	auto fitness = [&individuals](uint32_t i, uint32_t f) -> double {
		return individuals[i].fitness[f];
	};
	auto rank = [&individuals](uint32_t i) -> double {
		return individuals[i].rank;
	};
	auto dist = [&distMap](uint32_t i) -> double& {
		return distMap[i];
	};

	// 3. Assign distance.
	crowding_distance_assignment(indices, dist, fitness, individuals.front().fitness.size());

	return distMap;
}

template <typename Cont, typename Pred>
void crowding_distance_sort(Cont& container, Pred pred)
{
	if (container.size() < 2) {
		return;
	}

#if 1
	// 'It' is assumed as an instance of Individual class.
	// It is a big object, so we do not directly sort the list of Its, 
	// but sort indices and copy actual objects by referencing the sorted indices.

	// 1. Initialize index list by filling the original positions of each items.
	std::vector<uint32_t> indices(container.size());
	for (auto i = 0u; i < container.size(); i++) {
		indices[i] = i;
	}

	// 2. Create accessor.
	std::map<uint32_t, double> distMap; // <K,V>=<orignal position, distance>
	auto fitness = [&container](uint32_t i, uint32_t f) -> double {
		return container[i].fitness[f];
	};
	auto rank = [&container](uint32_t i) -> double {
		return container[i].rank;
	};
	auto dist = [&distMap](uint32_t i) -> double& {
		return distMap[i];
	};

	// 3. Assign distance.
	crowding_distance_assignment(indices, dist, fitness, container.front().fitness.size());

	// 4. Sort indices by distance.
	std::stable_sort(indices.begin(), indices.end(), crowded_comparison_operator(rank, dist, pred));

	// 5. Reassign based on the new order.
	Cont newList;
	for (auto i = 0u; i < container.size(); i++) {
		newList.push_back(container[indices[i]]);
	}
	container.swap(newList);
#else
	//uint32_t Ni = std::distance(first, last); // number of solutions in I
	uint32_t Nf = first->fitness.size();

	// Calculate crowding distance
	std::map<const std::iterator_traits<It>::value_type*, double> dist;
	for (auto f = 0u; f < Nf - 1; f++) {
		std::vector<std::pair<double, const std::iterator_traits<It>::value_type*> > temp;
		for (It it = first; it != last; it++) {
			temp.push_back(std::make_pair(it->fitness[f], &(*it)));
		}

		// Sort individuals by f-th fitness.
		std::stable_sort(temp.begin(), temp.end());

		// Accumulate distance.
		dist[temp.front().second] = std::numeric_limits<double>::infinity();
		dist[temp.back().second] = std::numeric_limits<double>::infinity();
		for (auto it = next(temp.cbegin()); it != prev(temp.cend()); it++) {
			dist[it->second] += next(it)->first - prev(it)->first;
		}
	}

	// Sort
	std::stable_sort(first, last,
		[&](const Garnet::Individual& x, const Garnet::Individual& y) -> bool {
		if (pred(x.rank, y.rank)) {
			return true;
		}
		if (x.rank == y.rank && dist[&x] > dist[&y]) {
			return true;
		}
		return false;
	});
#endif
}

}// end of anonymous namespace


//---------------------------------------------------------

std::pair<Garnet::IndividualList, Garnet::IndividualList> Garnet::NSGA2Strategy::BinaryTournamentSelector::execute(const GPEngine* engine, const IndividualList& individuals) const {
	const Garnet::Configuration& conf = engine->getConfiguration();
	const uint32_t Ni = conf.populationSize;

	std::pair<IndividualList, IndividualList> ret;
	IndividualList& archive  = ret.first;
	IndividualList& children = ret.second;

	// 1. Preserve elite (archive)
	//    NSGA2 preserves all individuals in the current generation.
	archive.assign(individuals.begin(), individuals.end());


	// 2. Select individuals by the binary tournament rule.
	//    Comparation is performed with the Crowded-Compariosn Operator.
	//    NOTE: The smaller fitness value is the better.

	// 2.1 Calculate crowding distance.
	auto distMap = get_crowding_distance(individuals);

	// 2.2 Get the Crowded-Comparison Operator with the caclulated crowding distance.
	auto rank = [&individuals](uint32_t i) -> double {
		return individuals[i].rank;
	};
	auto dist = [&distMap](uint32_t i) -> double& {
		return distMap[i];
	};
	auto comp = crowded_comparison_operator(rank, dist, std::less<double>());

	// 2.3 Perform binary tournament.
	for (auto i = 0u; i < Ni; i++) {
		auto selection = Mathematical::selectN(Ni, 2u, randInteger);
		const auto a = selection[0];
		const auto b = selection[1];
		children.push_back(individuals[comp(a, b) ? a : b]);
	}

	return ret;
}
//---------------------------------------------------------

namespace {
template <typename Cont>
void truncate(Cont& list, uint32_t maxSize)
{
	if (list.size() > maxSize) {
		list.resize(maxSize);
	}
}

// Smaller fitness is better.
// "y1 dominates y2" in Pareto efficiency means
// y1[i] <= y2[i] for every elements and y1[j] < y2[j] for some.
// @return 1 if p dominates q, -1 if q dominates p, 0 otherwise.
template <typename Array, typename Pred>
int dominates(const Array& p, const Array& q, uint32_t Nf, Pred pred)
{
	uint32_t p2q = 0;
	uint32_t q2p = 0;
	for (auto f = 0u; f < Nf; f++) {
		if ( pred(p[f], q[f]) ) p2q++;
		if ( pred(q[f], p[f]) ) q2p++;
	}
	return p2q == Nf ? 1 : q2p == Nf ? -1 : 0;
}

void fast_non_dominated_ranker(Garnet::IndividualList& individuals)
{
	using namespace Garnet;
	using namespace PicturePerfect;

	const size_t Ni = individuals.size();
	const size_t Nf = individuals.front().fitness.size();// engine->getConfiguration().fitness.size();

    // Search the first front.
	std::vector<uint32_t> F;                   // Current front.
	std::vector<uint32_t> n(Ni);               // The number of individuals that the i-th individual is dominated.
	std::vector<std::vector<uint32_t> > S(Ni); // Individuals that the i-th individual dominates.
	// Initialize dominating status.
	for (auto p = 0u; p < individuals.size(); p++) {
		uint32_t& np = n[p];
		np = 0;
		for (auto q = 0u; q < individuals.size(); q++) {
			switch ( dominates(individuals[p].fitness, individuals[q].fitness, Nf, std::less<double>()) ) {
			case 1 : // p dominates q
				S[p].push_back(q);
				break;
			case -1 : // q dominates p
				np++;
				break;
			}
		}

		if ( np == 0 ) {
			individuals[p].rank = 1;
			F.push_back(p);
		}
	}

	// Determine next fronts.
	uint32_t i = 1;
	while ( !F.empty() ) {
		std::vector<uint32_t> Q;
		std::for_each(F.cbegin(), F.cend(), [&] (uint32_t p) {
			std::for_each(S[p].cbegin(), S[p].cend(), [&] (uint32_t q) {
				uint32_t& nq = n[q];
				nq--;
				if ( nq == 0 ) {
					individuals[q].rank = i + 1;
					Q.push_back(q);
				}
			});
		});
		i++;
		F = Q;
	}
}

template <typename It, typename Pred>
void non_dominated_sort(It first, It last, Pred pred)
{
	std::stable_sort(first, last,
		[&](const Garnet::Individual& x, const Garnet::Individual& y) -> bool {
			if ( x.rank < y.rank ) {
				return true;
			}
			if ( x.rank == y.rank ) {
				if ( pred(x.fitness.back(), y.fitness.back()) ) {
					return true;
				}
				else if ( x.fitness.back() == y.fitness.back() ) {
					for (auto i = 0u; i < x.fitness.size() - 1; i++) {
						if ( pred(x.fitness[i], y.fitness[i]) ) {
							return true;
						}
						if ( pred(y.fitness[i], x.fitness[i]) ) {
							return false;
						}
					}
				}
			}
			return false;
	});
}

template <typename It, typename Pred>
void strong_non_dominated_sort(It first, It last, Pred pred)
{
	std::stable_sort(first, last,
		[&](const Garnet::Individual& x, const Garnet::Individual& y) -> bool {
			if ( x.rank < y.rank ) {
				return true;
			}
			if ( x.rank == y.rank ) {
				if ( dominates(x.fitness, y.fitness, x.fitness.size() - 1, std::less<double>()) == 1 ) {
					return true;
				}
				if ( pred(x.fitness.back(), y.fitness.back()) ) {
					return true;
				}
			}
			return false;
	});
}

}

//void Garnet::NSGA2Strategy::FastNonDominatedSorter::execute(const GPEngine* engine, IndividualList& individuals) const 
Garnet::IndividualList Garnet::NSGA2Strategy::FastNonDominatedSorter::execute(const GPEngine* engine, const IndividualList& archive, const IndividualList& children) const 
{
	// 1. Merge archive and children.
	IndividualList individuals;
	individuals = children;
	//std::copy(children.begin(), children.end(), std::back_inserter(individuals));
	std::transform(archive.begin(), archive.end(), std::back_inserter(individuals), Individual::archive);
	// 1.1 Preserve original ranking.
	for (auto i = 0u; i < children.size(); i++) {
		individuals[i].tempIndex = i;
	}
	for (auto i = children.size(); i < individuals.size(); i++) {
		individuals[i].tempIndex = std::numeric_limits<Garnet::Individual::history_t>::max();
	}

	//
	fast_non_dominated_ranker(individuals);

#if 1
	crowding_distance_sort(individuals, std::less<double>());
#elif 0
	crowding_distance_sort(individuals.begin(), individuals.end(), std::greater<double>());
#else
	non_dominated_sort(individuals.begin(), individuals.end(), std::greater<double>());
#endif

	// Truncate to the size of population
	truncate(individuals, engine->getConfiguration().populationSize);

	return individuals;
}

//void Garnet::NSGA2Strategy::StrongFastNonDominatedSorter::execute(const GPEngine* engine, IndividualList& individuals) const 
Garnet::IndividualList Garnet::NSGA2Strategy::StrongFastNonDominatedSorter::execute(const GPEngine* engine, const IndividualList& archive, const IndividualList& children) const 
{
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

	// 2.
	fast_non_dominated_ranker(individuals);

	strong_non_dominated_sort(individuals.begin(), individuals.end(), std::greater<double>());

	// Truncate to the size of population
	truncate(individuals, engine->getConfiguration().populationSize);

	return individuals;
}

//void Garnet::NSGA2Strategy::SPFastNonDominatedSorter::execute(const GPEngine* engine, IndividualList& individuals) const 
Garnet::IndividualList Garnet::NSGA2Strategy::SPFastNonDominatedSorter::execute(const GPEngine* engine, const IndividualList& archive, const IndividualList& children) const 
{
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

	// 2.
	fast_non_dominated_ranker(individuals);

#if 1
	crowding_distance_sort(individuals, std::less<double>());
#else
	non_dominated_sort(individuals.begin(), individuals.end(), std::less<double>());
#endif

	// Truncate to the size of population
	truncate(individuals, engine->getConfiguration().populationSize);

	return individuals;
}

//void Garnet::NSGA2Strategy::StrongSPFastNonDominatedSorter::execute(const GPEngine* engine, IndividualList& individuals) const 
Garnet::IndividualList Garnet::NSGA2Strategy::StrongSPFastNonDominatedSorter::execute(const GPEngine* engine, const IndividualList& archive, const IndividualList& children) const 
{
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

	// 2.
	fast_non_dominated_ranker(individuals);

	strong_non_dominated_sort(individuals.begin(), individuals.end(), std::less<double>());

	// Truncate to the size of population
	truncate(individuals, engine->getConfiguration().populationSize);

	return individuals;
}
