#ifndef GARNET__COMMON_STRATEGY__H__
#define GARNET__COMMON_STRATEGY__H__

#include <vector>
#include <string>
#include <queue>
#include <list>
#include <iostream>

//#include <Garnet/TreeBuilder.h>
#include <Garnet/GPEngine.h>
#include <Garnet/Strategy.h>
#include <Garnet/IVGraphProcessorCommand.h>

namespace Garnet {

/**
 *  An implemenation strategy for UniversalEngine,
 *  which uses a single unified fitness even if 
 *  individual could generate mulitple scores.
 *  Unifying operation can be specified by 
 */
namespace IVTreeStrategy {

/**
 *
 */
class IVTreeEvaluator : public Evaluator, public InfoContainer {
public :
	const enum {
		EVALUATE_TESTS, // bool. default is false.
	};

    IVTreeEvaluator();
	virtual ~IVTreeEvaluator();

    // Evaluator interface.
    virtual bool initialize(const Configuration& conf);
    virtual void execute(GPEngine* engine, IndividualList& individuals);

protected :
	void evaluateTree(GPEngine* engine, IndividualList& individuals, uint32_t Nt);
	void evaluateRawFitness(GPEngine* engine, IndividualList& individuals, uint32_t Nt);
	void evaluateFitnessVector(GPEngine* engine, IndividualList& individuals);

	std::vector<PicturePerfect::ArchivePtr>  getAArchives(uint32_t Ni, uint32_t Nt);
	std::vector<PicturePerfect::ArchivePtr>  getBArchives(uint32_t Ni, uint32_t Nt);
	std::vector<PicturePerfect::ArchivePtr>  getCArchives(uint32_t Ni);

	PicturePerfect::ArchivePtr  getAArchive(unsigned task, uint32_t Ni);
	PicturePerfect::ArchivePtr  getBArchive(unsigned task, uint32_t Ni);
	PicturePerfect::ArchivePtr  getCArchive(unsigned ind);

	// Configurations
	bool  isExecTimeEnabled() const { return _enableExecTime; }
	bool  isDistCollinsEnabled() const { return _enableDistCollins; }
	bool  isDistKashimaEnabled() const { return _enableDistKashima; }
	bool  isDistNSGA2Enabled() const { return _enableDistNSGA2; }

	// Utilitities
#if 1
	Garnet::Tree& resolveAliases(Garnet::Tree& tree);
#else
	std::string resolveAliases(const std::string& src);
#endif

private:
//	uint32_t                _Ni; /// Individuals
	uint32_t                _Nc; /// Chromosomes
	uint32_t                _Nt; /// Training tasks
	uint32_t                _Nu; /// Test tasks
	uint32_t                _Nr; /// Raw fitnesses
	uint32_t                _Nf; /// Fitness vector dimensions (with standardization)
	uint32_t                _Ns; /// Explicit constants (Input IVPairs per task)
	uint32_t                _Nk; /// Implicit constants (Const_xxx)
	uint32_t                _Na; /// Implicit raw fitnesses.
	//ConfigLoader::StringMap _aliases;

	bool  _enableExecTime;
	bool  _enableDistCollins;
	bool  _enableDistKashima;
	bool  _enableDistNSGA2;

	IVGraphProcessorCommand    _treeEvaluationCommand;
	IVPairArchivePtr           _archive; /// Contains source IVPairs and intermediate IVPairs.
	//std::vector<PicturePerfect::ExecutablePtr> _evaluatorExecutables;
	std::vector<PicturePerfect::ExecutablePtr> _fitnessExecutables;
};

}//end of namespace CommonStrategy

}// end of namespace Garnet


#endif//GARNET__BASIC_GA_STRATEGY__H__

