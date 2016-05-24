#ifndef GARNET__STRATEGY__H__
#define GARNET__STRATEGY__H__

#include <vector>
#include <string>
#include <memory>

#include <Garnet/Individual.h>
#include <Garnet/Configuration.h>

namespace Garnet
{

class GPEngine;
typedef std::shared_ptr<GPEngine>  GPEnginePtr;

/** Initial population generator.
 * @param[in] individuals  Individuals.
 * @param[in] individuals  Individuals.
 */
struct Initializer {
    virtual bool initialize(const Configuration& conf) { return true; }
	virtual IndividualList execute(const GPEngine* engine, uint32_t depth, uint32_t count) const = 0;
};

/** Assign fitness.
 * @param[in,out] individuals  Individuals.
 */
struct Evaluator {
    virtual bool initialize(const Configuration& conf) { return true; }
    virtual void execute(GPEngine* engine, IndividualList& individuals) = 0;
};

/** Assign ranks.
 *  Truncation will be porformed if necessary.
 * @param[in,out] individuals  Individuals.
 */
struct Sorter {
    virtual bool initialize(const Configuration& conf) { return true; }
    virtual IndividualList execute(const GPEngine* engine, const IndividualList& archive, const IndividualList& children) const = 0;
};

/** Perform selection.
 *  Move the specified number of the best individuals to archive,
 *  move some individuals which have greater rank than the criteria from the rest 
 *  to the mating pool, and discard all others.
 *  @param[in] individuals  Individuals.
 *  @return    A pair of the archive and the mating pool.
 */
struct Selector {
    virtual bool initialize(const Configuration& conf) { return true; }
    virtual std::pair<IndividualList, IndividualList> execute(const GPEngine* engine, const IndividualList& individuals) const = 0;
};

/** Judge termination criteria.
 *  @param[in] individuals  Individuals.
 *  @return    True if termination criteria met. False, otherwise.
 */
struct Terminator {
    virtual bool initialize(const Configuration& conf) { return true; }
    virtual bool isFinished(const GPEngine* engine, const IndividualList& individuals, int generation) const = 0;
};

/** Make parent pairs of the individuals for crossover.
 *  @param[in] individuals  Individuals.
 *  @return    Pairs of the indices of parents in the input individuals.
 */
struct PairMaker {
    virtual bool initialize(const Configuration& conf) { return true; }
    virtual std::vector<std::pair<size_t, size_t> > execute(const GPEngine* engine, const IndividualList& selection) const = 0;
};

/** Perform crossover. 
 *  @param[in] individuals  Individuals.
 *  @return    New individuals (Children).
 */
struct CrossoverOperator {
    virtual bool initialize(const Configuration& conf) { return true; }
    virtual IndividualList execute(const GPEngine* engine, const IndividualList& individuals, const std::vector<std::pair<size_t, size_t> > parents) const = 0;
};

/** Perform mutation. 
 *  @param[in,out] individuals  Individuals.
 */
struct MutationOperator {
    virtual bool initialize(const Configuration& conf) { return true; }
    virtual void execute(const GPEngine* engine, IndividualList& individuals) const = 0;
};

enum StageType {
    Preset,
	Initialization,
	Evaluation,
	Sort,
	Selection,
	Termination,
    PairMaking,
	Crossover,
	Mutation,
};

}// end of namespace Garnet

#endif//GARNET__STRATEGY__H__

