
#include <iostream>
#include <iomanip>

#include <Common/PrintRange.h>

#include <Garnet/Garnet.h>
#include <Garnet/Configuration.h>

Garnet::Configuration::Configuration()
{
    // Filter tree parameters
	imageWidth = 256;
	imageHeight = 256;
    // units must be initialized by a configuration file in load().
    // alias must be initialized by a configuration file in load().
	evaluator.push_back("(Tree0)");
	fitness.push_back("(RawFit0)");
	scoringMethod = "Average"; // Obsolete, but maintain to keep compatibility.
    // tasks must be initialized by a configuration file in load().
	maxInitialDepth = 10;
	minInitialSize  = 10;

	// GP Framework
	engine = "Default";
	initializationMethod.clear();
	evaluationMethod.clear();
	sortingMethod.clear();
	selectionMethod.clear();
	terminationMethod.clear();
	pairMakingMethod.clear();
	crossoverMethod.clear();
	mutationMethod.clear();

    // GA parameters
	maxGeneration = 1000;
	populationSize = 50;
    preserveElite = true;
	crossoverRate = 0.6;
	mutationRate = 0.03333333;
    randomSeed = 87654321u;
    randomAlgorithm = "MT19937";
	numChromosomes = 1;

    // Cache control parameters (Obsolete)
	leafCache = false;
	subtreeReduction = false;
    subtreeReductionType = "Inter";
    subtreeReductionDepth = 30;
	subtreeCache = false;
	subtreeCacheDepth = 7;
	subtreeCacheLife = 300;

    // Execution control parameters
	reportInterval = 1;
	reportWhenImproved = true;
	reportTimeInterval = 60;
	imageSavingInterval = 1;
	debug = false;
	trace = false;
	numThreads.push_back(1);
	cpuAffinity.push_back(String("1*"));
	cpuAffinity.push_back(String("0*"));
	numMaxCPUs = 1;
    simpleComparator = true;
	numVerticesLimit = 3000;
	enableOpenCL = isOpenCLEnabled;
	groupingMethod = "DAG";
	shouldRemoveIntrons = true;
	topologicalSortMethod = "Sarkar";
	isCompressionEnabled = true;

    // Initialize loader
    add("Engine", engine);
    add("InitializationMethod", initializationMethod);
    add("EvaluationMethod", evaluationMethod);
    add("SortingMethod", sortingMethod);
    add("SelectionMethod", selectionMethod);
    add("TerminationMethod", terminationMethod);
	add("PairMakingMethod", pairMakingMethod);
	add("CrossoverMethod", crossoverMethod);
    add("MutationMethod", mutationMethod);
    add("Evaluator", evaluator);
    add("Fitness", fitness);
    add("ImageWidth", imageWidth);
    add("ImageHeight", imageHeight);
    add("Unit", units);
    add("UnitM", units);
    add("Alias", aliases);
    add("ScoringMethod", scoringMethod);
    add("Task", tasks);
    add("Test", tests);
    add("MaxGeneration", maxGeneration);
    add("PopulationSize", populationSize);
    add("PreserveElite", preserveElite);
    add("CrossoverRate", crossoverRate);
    add("MutationRate", mutationRate);
    add("RandomSeed", randomSeed);
    add("RandomAlgorithm", randomAlgorithm);
    add("NumChromosomes", numChromosomes);
	add("NumVerticesLimit", numVerticesLimit);
    add("ReportInterval", reportInterval);
    add("ReportWhenImproved", reportWhenImproved);
    add("ReportTimeInterval", reportTimeInterval);
    add("ImageSavingInterval", imageSavingInterval);
    add("Debug", debug);
    add("Trace", trace);
    add("NumThreads", numThreads);
	add("CPUAffinity", cpuAffinity);
    add("NumMaxCPUs", numMaxCPUs);
    add("SimpleComparator", simpleComparator);
    add("EnableOpenCL", enableOpenCL);
    add("FilterCountPenaltyWeight", filterCountPenaltyWeight);
	add("MaxInitialDepth", maxInitialDepth);
	add("MinInitialSize", minInitialSize);
	add("GroupingMethod", groupingMethod);
	add("ShouldRemoveIntrons", shouldRemoveIntrons);
	add("TopologicalSortMethod", topologicalSortMethod);
	add("IsCompressionEnabled", isCompressionEnabled);
	add("LeafCache", leafCache); // Obsolete, but maintain to keep compatibility.
	add("SubtreeReduction", subtreeReduction); // Obsolete, but maintain to keep compatibility.
	add("SubtreeReductionType", subtreeReductionType); // Obsolete, but maintain to keep compatibility.
	add("SubtreeReductionDepth", subtreeReductionDepth); // Obsolete, but maintain to keep compatibility.
	add("SubtreeCache", subtreeCache); // Obsolete, but maintain to keep compatibility.
	add("SubtreeCacheDepth", subtreeCacheDepth); // Obsolete, but maintain to keep compatibility.
	add("SubtreeCacheLife", subtreeCacheLife); // Obsolete, but maintain to keep compatibility.

	//
	nextGeneration = -1;
}

bool Garnet::Configuration::isValid() const
{
    bool ret = true;
    if ( tasks.empty() ) {
		throw "Tasks must be specified.";
	}
	if ( units.getKeys().size() == 0 ) {
		throw "Units must be specified.";
	}
	if ( evaluator.empty() ) {
		throw "Evaluator definition must be specified.";
	}
	if ( fitness.empty() ) {
		throw "Fitness definition must be specified.";
	}
    return ret;
}

void Garnet::Configuration::save(std::ostream& fout) const
{
    // Common
    fout << "ImageWidth               " << imageWidth;
    fout << "\nImageHeight              " << imageHeight;
    fout << "\n";

    // Control
    fout << "\nReportInterval           " << reportInterval;
    fout << "\nReportWhenImproved       " << reportWhenImproved;
    fout << "\nReportTimeInterval       " << reportTimeInterval;
    fout << "\nImageSavingInterval      " << imageSavingInterval;
    fout << "\nDebug                    " << debug;
    fout << "\nTrace                    " << trace;
    fout << "\nNumThreads               " << print_range(numThreads.begin(), numThreads.end(), ", ");
    fout << "\nCPUAffinity              " << print_range(cpuAffinity.begin(), cpuAffinity.end(), ", ");
    fout << "\nEnableOpenCL             " << enableOpenCL;
    fout << "\nNumVerticesLimit         " << numVerticesLimit;
    fout << "\nMaxInitialDepth          " << maxInitialDepth;
    fout << "\nMinInitialSize           " << minInitialSize;
    fout << "\nGroupingMethod           " << groupingMethod;
    fout << "\nShouldRemoveIntrons      " << shouldRemoveIntrons;
    fout << "\nTopologicalSortMethod    " << topologicalSortMethod;
    fout << "\n";

	// GP Framework
	if (!engine.empty()) fout << "\nEngine                   " << engine;
	if (!initializationMethod.empty()) fout << "\nInitializationMethod     " << initializationMethod;
	if (!evaluationMethod.empty()) fout << "\nEvaluationMethod         " << evaluationMethod;
	if (!sortingMethod.empty()) fout << "\nSortingMethod            " << sortingMethod;
	if (!selectionMethod.empty()) fout << "\nSelectionMethod          " << selectionMethod;
	if (!terminationMethod.empty()) fout << "\nTerminationMethod        " << terminationMethod;
	if (!pairMakingMethod.empty()) fout << "\nPairMakingMethod         " << pairMakingMethod;
	if (!crossoverMethod.empty()) fout << "\nCrossoverMethod          " << crossoverMethod;
	if (!mutationMethod.empty()) fout << "\nMutationMethod           " << mutationMethod;

    // GA Parameters
    fout << "\nEvaluator                " << print_range(evaluator.begin(), evaluator.end(), ",");
	fout << "\nFitness                  " << print_range(fitness.begin(), fitness.end(), ", ");
    fout << "\nMaxGeneration            " << maxGeneration;
    fout << "\nPopulationSize           " << populationSize;
    fout << "\nPreserveElite            " << preserveElite;
    fout << "\nCrossoverRate            " << std::setprecision(16) << crossoverRate;
    fout << "\nMutationRate             " << std::setprecision(16) << mutationRate;
    fout << "\nRandomSeed               " << randomSeed;
    fout << "\nRandomAlgorithm          " << randomAlgorithm;
    fout << "\nNumChromosomes           " << numChromosomes;
    fout << "\nFilterCountPenaltyWeight " << std::setprecision(16) << filterCountPenaltyWeight;
    fout << "\n";

    // Evaluators
    fout << "\nScoringMethod            " << scoringMethod;
    fout << "\nSimpleComparator         " << simpleComparator;
    fout << "\n";

    // Evaluators
	std::vector<int> keys = units.getKeys();
	if ( keys.size() == 1 ) {
		for (auto it = units[0].begin(); it != units[0].end(); it++) {
			fout << "Unit  " << *it << "\n";
		}
	}
	else {
		for (auto jt = keys.begin(); jt != keys.end(); jt++) {
			for (auto it = units[*jt].begin(); it != units[*jt].end(); it++) {
				fout << "UnitM  " << *jt << ", " << *it << "\n";
			}
		}
	}
    for (StringMap::const_iterator it = aliases.begin(); it != aliases.end(); it++) {
        fout << "Alias " << it->first << ", " << it->second << "\n";
    }
    fout << "\n";

    // Tasks and Tests
	auto saveTask = [] (std::ostream& os, const std::string& type, TaskList::const_iterator& first, TaskList::const_iterator& last) {
		for (auto it = first; it != last; it++) {
			const Task& task = *it;
			os << type << " " << std::setprecision(16) << task.a 
				 << ", " << std::setprecision(16) << task.b;
			for (std::vector<std::string>::size_type j = 0; j < task.images.size(); j++) {
				os << ", \"" << task.images[j]
					 << "\", " << std::setprecision(16) << task.properties[j];
			}
			os << "\n";
		}
	};
#if 1
	saveTask(fout, "Task", tasks.begin(), tasks.end());
	saveTask(fout, "Test", tests.begin(), tests.end());
#else
    for (TaskList::size_type i = 0; i < tasks.size(); i++) {
        const Task& task = tasks[i];
        fout << "Task " << std::setprecision(16) << task.a 
             << ", " << std::setprecision(16) << task.b;
        for (std::vector<std::string>::size_type j = 0; j < task.images.size(); j++) {
            fout << ", \"" << task.images[j]
                 << "\", " << std::setprecision(16) << task.properties[j];
        }
        fout << "\n";
    }
    for (TaskList::size_type i = 0; i < tests.size(); i++) {
        const Task& test = tests[i];
        fout << "Test " << std::setprecision(16) << test.a 
             << ", " << std::setprecision(16) << test.b;
        for (std::vector<std::string>::size_type j = 0; j < test.images.size(); j++) {
            fout << ", \"" << test.images[j]
                 << "\", " << std::setprecision(16) << test.properties[j];
        }
        fout << "\n";
    }
#endif
    fout << std::endl;
}
