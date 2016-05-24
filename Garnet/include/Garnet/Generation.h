/**
 *
 *  Last Modified: 2012-12-10
 */
#ifndef GARNET__GENERATION__H__
#define GARNET__GENERATION__H__

#include <vector>
#include <string>
#include <queue>
#include <list>
#include <iostream>

#include <Common/ConfigLoader.h>
#include <PicturePerfectEngine/Engine.h>
#include <Garnet/RandomGenerator.h>
#include <Garnet/ThreadPool.h>
#include <Garnet/Individual.h>
#include <Garnet/Configuration.h>

namespace Garnet
{

class GPEngine;
typedef std::shared_ptr<GPEngine>  GPEnginePtr;

typedef std::vector<double> FitnessList;

/**
 *  Generation is a set of Individuals.
 *  Individual has a fitness vector and a set of Chromosomes.
 *  Chromosome is a Tree and has its score.
 */
class Generation : public ConfigLoader
{
public:
	Integer         generation;
	Long            randomSeed;
	IndividualList  individuals;

	Generation();

	/**
	 *
	 */
	bool    isEvaluated() const { return _isEvaluated; }

	/**
	 *  Return the current generation number.
	 */
	int     getGeneration() const { return generation; }

	/**
	 *  Return individuals which have the best (minimum) ranking
	 *  as their indices.
	 */
	std::vector<size_t>  getBestIndividuals() const;

	/**
	 *  Serialize a generation instance.
	 */
	void    save(std::ostream& outs) const;

	/**
	 *  Create generation instance which tends to be used as an initial generation.
	 *  @return  Generation. Generation is 0. RandomSeed is initialized.
	 *           Individuals are initialized as specified by conf.
	 */
	static Generation create(const GPEnginePtr engine);

	/**
	 *  Deserialize a generation instance.
	 */
	static Generation createFromFile(std::istream& ins);
protected:
private:
	bool            _isEvaluated;
};


}// end of namespace Garnet


#endif//GARNET__GENERATION__H__

