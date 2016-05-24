#ifndef GARNET__NSGA2_STRATEGY_CLASSES__H__
#define GARNET__NSGA2_STRATEGY_CLASSES__H__

#include <Garnet/Strategy.h>

namespace Garnet {

/**
 *  NGSA2Strategy is an implemenation of Strategy used with
 *  UniversalEngine.
 *  This is based on [1].
 *  [1] K. Deb, et al. "A Fast and Elitist Multiobjective 
 *      Genetic Algorithm: NSGA-II," IEEE Trans. on Evolutionary
 *      Computation, Vol.6, No.2, April 2002.
 */
namespace NSGA2Strategy {

struct BinaryTournamentSelector : public Selector {
	virtual std::pair<IndividualList, IndividualList> execute(const GPEngine* engine, const IndividualList& individuals) const;
};

/**
 *  Fast Non Dominated Sorter 
 *  This sorter assumes as the last element of a fitness vector is distance value. Larger distance is better.
 *  If no distance you use, place dummy constant (like 1) to the last element of a fitness vector.
 */
struct FastNonDominatedSorter : public Sorter {
//	virtual void execute(const GPEngine* engine, IndividualList& individuals) const;
	virtual IndividualList execute(const GPEngine* engine, const IndividualList& archive, const IndividualList& children) const;
};

/**
 *  Strong-Order Fast Non Dominated Sorter 
 *  This sorter take an individual with smaller fitnesses from two if those rank is same. 
 *  This sorter assumes as the last element of a fitness vector is distance value. Larger distance is better.
 *  If no distance you use, place dummy constant (like 1) to the last element of a fitness vector.
 */
struct StrongFastNonDominatedSorter : public Sorter {
//	virtual void execute(const GPEngine* engine, IndividualList& individuals) const;
	virtual IndividualList execute(const GPEngine* engine, const IndividualList& archive, const IndividualList& children) const;
};

/**
 *  Smaller Prefered Fast Non Dominated Sorter 
 *  This sorter assumes as the last element of a fitness vector is distance value. Smaller distance is better.
 *  If no distance you use, place dummy constant (like 1) to the last element of a fitness vector.
 */
struct SPFastNonDominatedSorter : public Sorter {
//	virtual void execute(const GPEngine* engine, IndividualList& individuals) const;
	virtual IndividualList execute(const GPEngine* engine, const IndividualList& archive, const IndividualList& children) const;
};

/**
 *  Strong-Order Smaller Prefered Fast Non Dominated Sorter
 *  This sorter take an individual with smaller fitnesses from two if those rank is same. 
 *  This sorter assumes as the last element of a fitness vector is distance value. Smaller distance is better.
 *  If no distance you use, place dummy constant (like 1) to the last element of a fitness vector.
 */
struct StrongSPFastNonDominatedSorter : public Sorter {
//	virtual void execute(const GPEngine* engine, IndividualList& individuals) const;
	virtual IndividualList execute(const GPEngine* engine, const IndividualList& archive, const IndividualList& children) const;
};

template <typename Iterator>
void  calculateCrowdedDistanceAssignment(Iterator first, Iterator second)
{
	const double INFINITY_VAL = std::numeric_limits<double>::max();
	std::vector<Individual*> I;
	uint32_t Ni = std::distance(first, second); // number of solutions in I
	uint32_t Nf = first->fitness.size() - 1;
	std::for_each(first, second, [&] (Individual& ind) {
		I.push_back(&ind);
		ind.fitness.back() = 0;                // initialize distance
	});
	for (auto f = 0u; f < Nf; f++) {
		// sort using each objective value.
		std::stable_sort(I.begin(), I.end(), [&] (Individual* a, Individual* b) {
			return a->fitness[f] < b->fitness[f];
		});

		// so that bounardy points are always selected.
		I.front()->fitness.back() = I.back()->fitness.back() = INFINITY_VAL;

		// for all other points
		double frange = I.back()->fitness[f] - I.front()->fitness[f];
		for (auto i = 1u; i < Ni - 1; i++) {
			if (I[i]->fitness.back() != INFINITY_VAL) {
				I[i]->fitness.back() += (I[i]->fitness[f] - I[i - 1]->fitness[f])/frange;
			}
		}
	}
}

}//end of namespace NSGA2Strategy

}// end of namespace Garnet

#endif//GARNET__NSGA2_STRATEGY_CLASSES__H__

