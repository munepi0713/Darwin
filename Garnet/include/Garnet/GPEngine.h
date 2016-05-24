#ifndef GARNET__ENGINE__H__
#define GARNET__ENGINE__H__

#include <vector>
#include <string>
#include <queue>
#include <list>
#include <iostream>
#include <memory>

#include <boost/any.hpp>

#include <PicturePerfectEngine/Engine.h>
#include <Garnet/RandomGenerator.h>
#include <Garnet/ThreadPool.h>
#include <Garnet/Individual.h>
#include <Garnet/Configuration.h>
#include <Garnet/Generation.h>
#include <Garnet/Tree.h>
#include <Garnet/TreeBuilder.h>
#include <Garnet/Strategy.h>

namespace Garnet
{

class GPEngine;
typedef std::shared_ptr<GPEngine>  GPEnginePtr;

/**
 *
 */
class GPEngine
{
public :
	typedef const enum {
		NUMBER_OF_THREADS,
		EVALUATE_TESTS,    // bool. Default is false.
	} engine_param_t;

	GPEngine(const GPEngine& engine);
    virtual ~GPEngine();

    //-- Setting stage operators --
    /** Set initialization stage */
    void  setInitializer(Initializer* stage) { _initializer.reset(stage); }
    /** Set evaluation stage */
    void  setEvaluator(Evaluator* stage) { _evaluator.reset(stage); }
    /** Set sorting stage */
    void  setSorter(Sorter* stage) { _sorter.reset(stage); }
    /** Set selection stage */
    void  setSelector(Selector* stage) { _selector.reset(stage); }
    /** Set termination stage */
    void  setTerminator(Terminator* stage) { _terminator.reset(stage); }
    /** Set pair making stage */
    void  setPairMaker(PairMaker* stage) { _pairMaker.reset(stage); }
    /** Set crossover stage */
    void  setCrossoverOperator(CrossoverOperator* stage) { _crossover.reset(stage); }
    /** Set mutation stage */
    void  setMutationOperator(MutationOperator* stage) { _mutation.reset(stage); }

    //-- Getting/Setting GP parameters --
    void       setRandomSeed(long seed);
    void       setGeneration(const Generation& generation);
    Generation getGeneration() const { return _generation; }
    void       setPopulationSize(uint32_t Ni);
    uint32_t   getPopulationSize() const { return _Ni; }
    void       setNumChromosomes(uint32_t Nc);
    uint32_t   getNumChromosomes() const { return _Nc; }

    //-- Set operation configurations --
    void     setTraceEnabled(bool isEnabled);
    bool     isTraceEnabled() const;

    //-- Performs genetic algorithm. --
    /** Initialize */
    virtual void initialize();
    virtual void initialize(const Generation& generation);

    /** Evolves generations until termination condition meets. */
    virtual void evolve(InfoContainer& info = InfoContainer());

    //-- Perform parts of genetic algorithm. --
    /** Evaluates a generation. */
    virtual void evaluate(Generation& generation, bool willSort = true, InfoContainer& info = InfoContainer());

    /** Reproduces a generation to the next generation. */
    virtual Generation reproduce(const Generation& generation, InfoContainer& info = InfoContainer());

    /** Return if termination criteria has been met. */
    virtual bool isFinished(const Generation& generation) const;

#if 0
    //-----------------------------------------------------
	/** Return configuration */
	const Configuration& getConfiguration() const { return _conf; }
	const Configuration& getConfiguration() { return _conf; }

	/** Save the result of last execution. */
	virtual void saveLastImages(const std::string& pathPrefix, const IndividualList& individuals, const std::vector<size_t>& targets) const;

	//
	void setParameter(engine_param_t name, const boost::any& param);
	const boost::any& getParameter(engine_param_t name) const;

    std::shared_ptr<const Initializer> getInitializer() const { return _initializer; }

    // Create engine instance.
    static GPEnginePtr create(const Configuration& conf);

	TreeBuilderPtr getTreeBuilder(unsigned chr) const { (void)chr; return _builder[chr]; }
	void  loadSources(std::istream& ins);
	void  saveResults(std::ostream& outs);

	// Constants
	uint32_t getNumConstants() const { return _Nk; }
#if 0
	uint32_t getNumSources() const { return _Ns; }
#endif
#endif

private :
	std::vector<TreeBuilderPtr>         _builder;
	std::shared_ptr<Initializer>        _initializer;
	std::shared_ptr<Evaluator>          _evaluator;
	std::shared_ptr<Evaluator>          _fvector;
	std::shared_ptr<Sorter>             _sorter;
	std::shared_ptr<Selector>           _selector;
	std::shared_ptr<Terminator>         _terminator;
	std::shared_ptr<PairMaker>          _pairMaker;
	std::shared_ptr<CrossoverOperator>  _crossover;
	std::shared_ptr<MutationOperator>   _mutation;

	uint32_t                _Ni;
	uint32_t                _Nc;
#if 0
	uint32_t                _Nt;
	uint32_t                _Nu;
	uint32_t                _Nr;
	uint32_t                _Nf;
	uint32_t                _Ns;
	uint32_t                _Nk;
	uint32_t                _Na;
    ConfigLoader::StringMap _aliases;

	mutable std::shared_ptr<std::ostream>  _tout;
	mutable std::shared_ptr<std::ostream>  _dout;
#endif

protected:
    GPEngine() {}

private :
//    Configuration _conf;
    Generation    _generation;
    bool          _isTraceEnabled;
//	std::map<engine_param_t, boost::any> _params;
};

}// end of namespace Garnet


#endif//GARNET__ENGINE__H__

