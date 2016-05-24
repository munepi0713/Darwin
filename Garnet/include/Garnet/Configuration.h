#ifndef GARNET__CONFIGURATION__H__
#define GARNET__CONFIGURATION__H__

#include <Common/ConfigLoader.h>

namespace Garnet
{

/**
 *  Task
 */
struct Task
{
    double                    a;
    double                    b;
    std::vector<std::string>  images;
    std::vector<double>       properties;
};

struct TaskList : public ConfigLoader::Value, public std::vector<Task>
{
public :
    virtual void load(const strings& tokens)
    {
        ConfigLoader::NumArgsException::greaterEqOrThrow(5, tokens.size(), tokens[0] + " command must have equals to or more than $1 argument, but $2 args specified.");
        Task task;
        task.a = strtod(tokens[1].c_str(), 0);
        task.b = strtod(tokens[2].c_str(), 0);
        for (unsigned int i = 3; i < tokens.size(); i+=2) {
            task.images.push_back(tokens[i]);
            task.properties.push_back(strtod(tokens[i+1].c_str(), 0));
        }
        push_back(task);
    }
};

typedef ConfigLoader::StringList BuildingBlocks;

struct BuildingBlockList : public ConfigLoader::Value
{
public :
    virtual void load(const strings& tokens) {
        if ( tokens[0].compare("Unit") == 0 ) {
            ConfigLoader::NumArgsException::matchOrThrow(2, tokens.size(), tokens[0] + " command must have $1 argument, but $2 args specified.");
            _bbs[0].push_back(tokens[1]);
        }
        else if ( tokens[0].compare("UnitM") == 0 ) {
            ConfigLoader::NumArgsException::matchOrThrow(3, tokens.size(), tokens[0] + " command must have $1 argument, but $2 args specified.");
            int id = strtol(tokens[1].c_str(), 0, 0);
            _bbs[id].push_back(tokens[2]);
        }
    }

    const BuildingBlocks& operator[](int idx) const {
        return _bbs.at(idx);
    }

    BuildingBlocks& operator[](int idx) {
        return _bbs[idx];
    }

    operator const BuildingBlocks& () const {
        return _bbs.at(0);
    }

    std::vector<int>  getKeys() const {
        std::vector<int> ret;
        for (auto it = _bbs.begin(); it != _bbs.end(); it++) {
            ret.push_back(it->first);
        }
        return ret;
    }

	std::map<int, BuildingBlocks>::const_iterator cbegin() const {
		return _bbs.cbegin();
	}

	std::map<int, BuildingBlocks>::const_iterator cend() const {
		return _bbs.cend();
	}

private :
    std::map<int, BuildingBlocks> _bbs;
};

/**
 *
 */
struct Configuration : public ConfigLoader
{
    // Filter Tree Parameters
    Integer            imageWidth;
    Integer            imageHeight;
    BuildingBlockList  units;
    StringMap          aliases;
    TaskList           tasks;
    TaskList           tests;
    Integer            maxInitialDepth;
	Integer            minInitialSize;

	// GP Framework
    String             engine;
	String             initializationMethod;
	String             evaluationMethod;
	String             sortingMethod;
	String             selectionMethod;
	String             terminationMethod;
	String             pairMakingMethod;
	String             crossoverMethod;
	String             mutationMethod;

    // GA Parameters
    StringTuple        evaluator;
    StringTuple        fitness;
    Integer            maxGeneration;
    Integer            populationSize;
    Boolean            preserveElite;
    Double             crossoverRate;
    Double             mutationRate;
    Long               randomSeed;
    String             randomAlgorithm;
    Integer            numChromosomes;
    Double             filterCountPenaltyWeight; // Fitness is reduced by the formula: fitness * exp(-Kx). weight is 0.95 if K=0.000256466 and x = 200.
                       
    // Execution Control Parameters
    Integer            reportInterval;
    Boolean            reportWhenImproved;
    Double             reportTimeInterval;
    Integer            imageSavingInterval;
    Boolean            debug;
    Boolean            trace;
    Array<Integer>     numThreads;
	Array<String>      cpuAffinity;
    Integer            numMaxCPUs;
    Boolean            simpleComparator;
    Integer            numVerticesLimit;
	Boolean            enableOpenCL;
	String             groupingMethod; // DAG or FOREST
	Boolean            shouldRemoveIntrons;
	String             topologicalSortMethod; // Sarker or Standard
	Boolean            isCompressionEnabled;
                       
	// Obsolete parameters.
	String             scoringMethod;
	Boolean            leafCache;
	Boolean            subtreeReduction;
	String             subtreeReductionType;
	Integer            subtreeReductionDepth;
	Boolean            subtreeCache;
	Integer            subtreeCacheDepth;
	Integer            subtreeCacheLife;

	// Working Parameters
	String             prj;
    String             dir;
    String             title;
	Boolean            verbose;
	Integer            nextGeneration; // -1:scan. 0-:start from the specified generation.

    Configuration();

    bool isValid() const;
    void save(std::ostream& fout) const;
};

}//end of namespace Garnet


#endif//GARNET__CONFIGURATION__H__
