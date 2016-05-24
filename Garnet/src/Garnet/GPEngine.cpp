#pragma warning(disable : 4819)

#include <algorithm>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <memory>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/graph/graph_utility.hpp>

#include <Common/GetTime.h>
#include <Common/Text.h>

#include <PicturePerfectEngine/Vector.h>

#include <Garnet/Affinity.h>
#include <Garnet/BasicStrategy.h>
#include <Garnet/DefaultTreeBuilder.h>
#include <Garnet/ExperimentalStrategy.h>
#include <Garnet/Garnet.h>
#include <Garnet/GPEngine.h>
#include <Garnet/IVTreeStrategy.h>
#include <Garnet/NSGA2Strategy.h>
#include <Garnet/RandomGenerator.h>

//---------------------------------------------------------
//---------------------------------------------------------
namespace fs = boost::filesystem;

//---------------------------------------------------------
//---------------------------------------------------------
#define PRINT_PROGRESS(msg) std::cout << msg << std::flush;

//---------------------------------------------------------
//---------------------------------------------------------
namespace {

template <typename O>
std::shared_ptr<O>  envelop(O* o)
{
	return std::shared_ptr<O>(o);
}


#define CONF_FILENAME ".garnet_conf.txt"
#define CP_FILENAME   ".garnet_cp.txt"
#define CPU_FILENAME  ".num_cpus.txt"

struct Summary {
    uint32_t generation;
    double   bestFitness;
    double   averageFitness;
    double   worstFitness;
    uint32_t maxSize;
    double   aveageSize;
    uint32_t minSize;
    double   seconds;
    uint32_t operations;
    uint32_t memories;
};
typedef std::vector<Summary> Summaries;

void  evaluate(Garnet::GPEnginePtr ga, Garnet::Generation& generation) {
    std::cout << "\n  Evaluating ";
    ga->evaluate(generation);
    std::cout << " Done";
}

void  reproduce(Garnet::GPEnginePtr ga, Garnet::Generation& generation) {
    std::cout << "\n  Reproducing ";
    generation = ga->reproduce(generation);
    std::cout << " Done";
}

void  writeGenerationLog(const Garnet::GPEnginePtr& ga, const Garnet::Generation& generation, const std::string& dir) {
    std::cout << "\n  Saving Log ... " << std::flush;

    // Format a log about the current generation in temporary memory.
    fs::path cpfile = Common::File::writeToFile(
        str(boost::format("%s" CP_FILENAME) % dir),
        [&generation](std::ostream& outs) {
        generation.save(outs);
    },
        Garnet::Settings::isCompressionEnabled());

    // Rename the temporary file to a generation log.
    boost::system::error_code ec;
    fs::path genfile = str(boost::wformat(L"%1$s%2$cgenerations%2$c%3$06u.txt%4$s") % Text::toWideChar(dir) % fs::path::preferred_separator % generation.generation % (Garnet::Settings::isCompressionEnabled() ? L".gz" : L""));
    fs::copy_file(cpfile, genfile, fs::copy_option::overwrite_if_exists, ec);
    if (ec) {
        std::cout << ec.message() << std::endl;
        std::cerr << ec.message() << std::endl;
    }

    std::cout << "Done";
}

void writeGenerationImages(const Garnet::GPEnginePtr& ga, const Garnet::Generation& generation, const std::string& dir) {
    std::cout << "\n  Saving Images ... " << std::flush;
    ga->saveLastImages(dir + ".garnet_result_", generation.individuals, generation.getBestIndividuals());
    std::cout << "Done";
}

void addSummary(const Garnet::Generation& generation, double time, Summaries& summaries) {
    const int Ni = generation.individuals.size();
    const int Nc = generation.individuals.front().chromosome.size();

    double maxFit = 0.0;
    double aveFit = 0.0;
    double minFit = std::numeric_limits<double>::max();
    uint32_t maxSize = 0u;
    double   aveSize = 0.0;
    uint32_t minSize = 0xffffffffu;
    for (auto i = 0; i < Ni; i++) {
        const Garnet::Individual& ind = generation.individuals[i];
        uint32_t size = 0;
        for (auto c = 0; c < Nc; c++) {
            size += ind.chromosome[c].getNumVertices();
        }
        if (size > maxSize) maxSize = size;
        if (size < minSize) minSize = size;
        aveSize += size;

        double fitness = ind.fitness[0];
        if (fitness > maxFit) maxFit = fitness;
        if (fitness < minFit) minFit = fitness;
        aveFit += fitness;
    }

    Summary s;
    s.generation = generation.generation;
    s.bestFitness = maxFit;
    s.averageFitness = aveFit / Ni;
    s.worstFitness = minFit;
    s.maxSize = maxSize;
    s.aveageSize = aveSize / Ni;
    s.minSize = minSize;
    s.seconds = time;

    summaries.push_back(s);
}

void  writeSummary(const Garnet::GPEnginePtr& ga, const Summaries& summaries) {
    const Garnet::Configuration& conf = ga->getConfiguration();
    std::ofstream fout;
    std::string fn;

    if (summaries.empty()) return;

    std::cout << "\n  Saving Summary ... " << std::flush;

    // Flush stored summary to the summary file.
    fn = (boost::format("%s%s.csv") % conf.dir % conf.title).str();
    fout.open(fn.c_str(), std::ios::app);
    // Output header if new file is created.
    if (summaries.front().generation == 0) {
        fout << "Generation,Best Fitness,Average Fitness,Worst Fitness,Max Size,Average Size,Min Size,Seconds\n";
    }
    // Flushing summaries to temporary memory.
    std::stringstream sout;
    for (auto it = summaries.begin(); it != summaries.end(); it++) {
        sout << boost::format("%u,%.6f,%.6f,%.6f,%u,%.1f,%u,%.3f\n")
            % it->generation
            % it->bestFitness
            % it->averageFitness
            % it->worstFitness
            % it->maxSize
            % it->aveageSize
            % it->minSize
            % it->seconds;
    }

    // Write the summaries to the file.
    fout << sout.str();
    fout.close();

    std::cout << "Done";
}

void  printInformation(const Garnet::Generation& generation, double time) {
    std::cout << "\n  Best Fitness: (" << print_range(generation.individuals[0].fitness.begin(), generation.individuals[0].fitness.end(), ", ") << ")";
    std::cout << boost::format("\n  Time: %.6f [sec]") % time;
}

}//end of local namespace


//---------------------------------------------------------
//---------------------------------------------------------

Garnet::GPEngine::GPEngine(const GPEngine& engine)
 : _builder(engine._builder),
 _initializer(engine._initializer),
 _evaluator(engine._evaluator),
 _sorter(engine._sorter),
 _selector(engine._selector),
 _terminator(engine._terminator),
 _pairMaker(engine._pairMaker),
 _crossover(engine._crossover),
 _mutation(engine._mutation) {
    setParameter(EVALUATE_TESTS, false);
}

Garnet::GPEngine::~GPEngine() {
}

#if 0
bool  Garnet::GPEngine::initialize(const Configuration& conf) {

    // 0. Reset random generator.
    Garnet::srand(Garnet::RandomGenerator::NOT_CHANGE, conf.randomSeed);

    //
    _Nk = 0;
    _Ni = conf.populationSize;
    _Nc = conf.numChromosomes;
    _Nt = conf.tasks.size();
    _Nu = conf.tests.size();
    _Nr = conf.evaluator.size();
    _Nf = conf.fitness.size();
    _Ns = conf.tasks.front().images.size();
    _Na = 5;

    // Create Tree Builder from Building Blocks.
    //   Each chromosome can have its dedicated building block, 
    //   but all chromosome use a common B.B. for now.
    for (auto c = 0u; c < _Nc; c++) {
        _builder.push_back(Garnet::TreeBuilder::create(conf.units));
    }

}
#endif

void  Garnet::GPEngine::initialize() {
    // Generate individuals for a generation.
    // Each individual has a tree with the depth equals to or less than the max depth.
    Garnet::Generation generation;
    generation.individuals = _initializer->execute(this, _Ni);
    generation.generation = 0;
    generation.randomSeed = randInteger();
    _generation = generation;

    // Perform initial evaluation.
    StopWatch watchE;
    evaluate(generation);
    watchE.stop();
    writeGenerationLog(this, generation, conf.dir);

    Summaries summaries;
    addSummary(generation, watchE.get(), summaries);
    //writeGenerationImages(ga, generation, dir);
    writeSummary(this, summaries);
    summaries.clear();
}

void  Garnet::GPEngine::initialize(const Garnet::Generation& generation) {
    _generation = generation;
}

void  Garnet::GPEngine::evolve(InfoContainer& info) {
    // Checks a pre-evaluated initial generation has been set.
    if (!_generation.isEvaluated) {
        GARNET_THROW(, "Generation must be set first.");
    }

    //
    Generation& generation = _generation;
    fs::path cpuFileName(CPU_FILENAME);
    std::time_t             lastUpdateCpuFile = 0u;
    bool      loop = true;
    std::vector<double>    bestFitness(conf.fitness.size(), std::numeric_limits<double>::max());
    uint32_t  lastImageSaving = generation.getGeneration();
    StopWatch watchEvolve;
    StopWatch watchCP;
    while (loop) {

        std::ofstream ftrace;
        if (isTraceEnabled()) {
            ftrace.open((boost::format("%strace.%06u.txt") % conf.dir % (generation.getGeneration() + 1)).str().c_str());
            setTraceStream(&ftrace);
        }

        //
        StopWatch watch;
        StopWatch watchE;
        bool changed = false;

        // Runtime Affinity Control
        if (fs::exists(cpuFileName)) {
            std::time_t updateTime = fs::last_write_time(cpuFileName);
            if (updateTime > lastUpdateCpuFile) {
                std::ifstream fcpu(CPU_FILENAME);
                if (fcpu.good()) {
                    std::string threads, cpu, gpu;
                    fcpu >> threads >> cpu >> gpu;
                    fcpu.close();
                    lastUpdateCpuFile = updateTime;

                    // Number of Threads
                    if (!threads.empty()) {
                        int numThreads = std::stoi(threads);
                        std::cout << "\nNum Threads Changed: " << std::flush;
                        ga->setParameter(Garnet::GPEngine::NUMBER_OF_THREADS, int(numThreads));
                        std::cout << numThreads << std::flush;
                    }

                    // CPU Affinity
                    if (!cpu.empty()) {
                        std::cout << "\nNum CPU Changed: " << std::flush;
                        updateCPUAffinity(cpu, gpu);
                        std::cout << "cpu=" << cpu << ", gpu=" << gpu << std::flush;
                    }
                }
            }
        }

        // GA1. Check termination condition met
        std::cout << "\n  Checking termination ... ";
        bool isFinished;
        isFinished = _terminator->isFinished(this, generation.individuals, generation.generation);
        if (isFinished) {
            std::cout << "Finished.";
            loop = false;
            goto Closing;
        }
        else {
            std::cout << "Continue.";
        }

        // GA2. Process next generation.
        std::cout << "\n\nGeneration " << (generation.getGeneration() + 1);
        if (isProcessTrace()) std::cerr << "\n\nGeneration " << (generation.getGeneration() + 1);

        // Reproduce (including fitness evaluation)
        // # generation number will increment after reproduction.
        reproduce(ga, generation);

        auto& f = generation.individuals.front().fitness;
        if (less(f.cbegin(), std::next(f.cbegin(), f.size() - 1), bestFitness.cbegin())) {
            bestFitness = generation.individuals.front().fitness;
            changed = true;
        }

        // Save total time.
        watchE.stop();

        // Show Status
        watch.stop();
        printInformation(generation, watch.get());

    Closing:
        // Store summary data.
        addSummary(generation, watchE.get(), summaries);
        // Check condition to save check point and summary file.
        bool save = false;
        if ((conf.reportInterval > 0u) && (summaries.size() >= conf.reportInterval)) {
            save = true;
        }
        if (conf.reportWhenImproved && changed) {
            save = true;
        }
        if ((conf.reportTimeInterval > 0u) && (watchCP.lap() >= conf.reportTimeInterval)) {
            save = true;
        }
        // Save check point and summary file if the condition been met.
        if (save || !loop) {

            writeGenerationLog(ga, generation, conf.dir);
#if 0
            if (!loop || ((conf.imageSavingInterval > 0) && ((generation.getGeneration() - lastImageSaving) >= conf.imageSavingInterval))) {
                writeGenerationImages(ga, generation, dir);
                lastImageSaving = generation.getGeneration();
            }
#endif
            writeSummary(ga, summaries);
            summaries.clear();
            watchCP.reset();
        }

        setTraceStream(0);
        ftrace.close();
    }
    watchEvolve.stop();
}

void Garnet::GPEngine::evaluate(Garnet::Generation& generation, bool willSort, InfoContainer& info)
{
	// 5. Evaluate
	PRINT_PROGRESS("(5");
	_evaluator->execute(this, generation.individuals);

	// 6. Generating FitnessVector.
	PRINT_PROGRESS("6");
#if 0
	_fvector->execute(this, generation.individuals);
#endif

	// 7. Rank/Sort
	if (willSort) {
		PRINT_PROGRESS("7");
#if 0
		for (auto i = 0u; i < generation.individuals.size(); i++) {
			generation.individuals[i].tempIndex = i;
		}
#endif
#if 1
		generation.individuals = _sorter->execute(this, IndividualList(), generation.individuals);
#else
		_sorter->execute(this, generation.individuals);
#endif
	}

	//
//	generation._isEvaluated = true;
	PRINT_PROGRESS(")\n");
}

Garnet::Generation Garnet::GPEngine::reproduce(const Generation& generation, InfoContainer& info)
{
	Garnet::Generation     ret;
	Garnet::IndividualList archive;   // Elites.
	Garnet::IndividualList selection; // Selected for next gen, or mating pool.
	Garnet::IndividualList children;
	std::map<int, StopWatch> watches;

#if 0
	// Output all raw scores (if Trace enabled)
	if ( isTrace() ) {
		std::ostream* os;
		os = new std::ofstream(
				inline_sprintf("%s/%s.%05u.trace.txt",
						getConfiguration().dir.c_str(),
						getConfiguration().title.c_str(),
						generation.getGeneration())
				.c_str());
		_tout.reset(os);
		os = new std::ofstream(
				inline_sprintf("%s/%s.%05u.debug.txt",
						getConfiguration().dir.c_str(),
						getConfiguration().title.c_str(),
						generation.getGeneration())
				.c_str());
		_dout.reset(os);

		getTraceOut() << "\"individual\": [";
		for (unsigned int n = 0; n < generation.individuals.size(); n++) {
			const Individual& individual = generation.individuals[n];
			const Garnet::ChromosomeList& chromosomes = individual.chromosome;

            getTraceOut() << "\n  \"chromosome\": [";
			for (size_t nc = 0; nc < chromosomes.size(); nc++) {
                getTraceOut() << "\n    \"score\": [";
				for (size_t nt = 0; nt < chromosomes[nc].score.size(); nt++) {
					getTraceOut() << inline_sprintf("\n      %.16f", chromosomes[nc].score[nt]);
					if ( nt < chromosomes[nc].score.size() - 1 ) getTraceOut() << ",";
				}
                getTraceOut() << "\n    ]";
				if ( nc < chromosomes.size() - 1 ) getTraceOut() << ",";
            }
            getTraceOut() << "\n  ]";
			if ( n < generation.individuals.size() - 1 ) getTraceOut() << ",";
		}
        getTraceOut() << "\n  ]\n";
	}
#endif

	// 0. Reset random generator.
	Garnet::srand(Garnet::RandomGenerator::NOT_CHANGE, generation.randomSeed);

	// 1. Selection
	//   Select elite and remove worst individuals.
	//   Size
	//     generation.individuals: Individuals of the previous generation. Ni.
	//     archive:                Indices of archived individuals. 0 <= Narchive <= Ni.
	//     selection:              Indices of individuals which are candidates to be saved to the next geneneration. 0 < Nselection <= Ni.
	PRINT_PROGRESS("(1");
	watches[1].reset();
	boost::tie(archive, selection) = _selector->execute(this, generation.individuals);
	watches[1].stop();

	// 2. Mating
	//   Make parent pairs. The number of children which a pair makes depends on the crossover operator.
	//   Size
	//     parents:  Indices of individual pairs. 0 < Nparents.
	PRINT_PROGRESS("2");
	watches[2].reset();
	std::vector<std::pair<size_t, size_t> > parents = _pairMaker->execute(this, selection);
	watches[2].stop();

	// 3. Crossover
	//   Make children from parent pairs. Two children are usually made by the operator.
	//   children has -1 as pre-rank.
	//   Size
	//     children: Individuals made from parents. 0 < Nchildren.
	PRINT_PROGRESS("3");
	watches[3].reset();
	children = _crossover->execute(this, generation.individuals, parents);
	watches[3].stop();

	// 4. Mutation
	//   Apply mutation operator for the individuals made at step 2.
	//   Size
	//     children: Individuals made from parents. 0 < Nchildren.
	PRINT_PROGRESS("4");
	watches[4].reset();
	_mutation->execute(this, children);
	watches[4].stop();

#if 1
	// 5. Evaluation
	//   Evaluate the individuals made by at step 2-4.
	//   Archive won't be evaluated again.
	//   Size
	//     children: Individuals made from parents. 0 < Nchildren.
	PRINT_PROGRESS("5");
	watches[5].reset();
	_evaluator->execute(this, children);
	watches[5].stop();

	// 6/7. Aggregation//Rank/Sort & Truncate to the population size.
	//   Size
	//     (Before) ret.individuals: Nchildren + Ni.
	//     (After)  ret.individuals: Ni.
	PRINT_PROGRESS("67");
	// 7.1 Save evaluation indices.
	// 7.2 Rank/Sort/Truncate
	watches[6].reset();
	ret.individuals = _sorter->execute(this, archive, children);
	watches[6].stop();
#else
#if 1
	// 5. Evaluation
	//   Evaluate the individuals made by at step 2-4.
	//   Archive won't be evaluated again.
	//   Size
	//     children: Individuals made from parents. 0 < Nchildren.
	PRINT_PROGRESS("5");
	_evaluator->execute(this, children);

	// 6. Aggregation/Generating FitnessVector.
	//   Aggregate the previous and current generations and calculate final fitness (fitness vector).
	//   Size
	//     ret.individuals: Nchildren + Ni
	PRINT_PROGRESS("6");
	ret.individuals = children;
	std::transform(archive.begin(), archive.end(), std::back_inserter(ret.individuals), Garnet::Individual::archive);
#else
	// 5. Evaluation
	//   Evaluate the individuals made by at step 2-4.
	//   Archive won't be evaluated again.
	//   Size
	//     children: Individuals made from parents. 0 < Nchildren.
	PRINT_PROGRESS("5");
	_evaluator->execute(const_cast<GPEngine*>(this), children);

	// 6. Aggregation/Generating FitnessVector.
	//   Aggregate the previous and current generations and calculate final fitness (fitness vector).
	//   Size
	//     ret.individuals: Nchildren + Ni
	PRINT_PROGRESS("6");
	ret.individuals = children;
	std::transform(archive.begin(), archive.end(), std::back_inserter(ret.individuals), Garnet::Individual::archive);
	_fvector->execute(const_cast<GPEngine*>(this), ret.individuals);
#endif

	// 7. Rank/Sort & Truncate to the population size.
	//   Size
	//     (Before) ret.individuals: Nchildren + Ni.
	//     (After)  ret.individuals: Ni.
	PRINT_PROGRESS("7");
	// 7.1 Save evaluation indices.
	for (auto i = 0u; i < children.size(); i++) {
		ret.individuals[i].tempIndex = i;
	}
	for (auto i = children.size(); i < ret.individuals.size(); i++) {
		ret.individuals[i].tempIndex = std::numeric_limits<Garnet::Individual::history_t>::max();
	}
	// 7.2 Rank/Sort/Truncate
	_sorter->execute(this, ret.individuals);
#endif

	// X. Finalize.
	ret.generation  = generation.generation + 1;
	ret.randomSeed  = randInteger(0x7fffffff);
//	ret._isEvaluated = true;
	PRINT_PROGRESS(")\n");

#if 1
	std::cerr << boost::format("\n Generation %u,") % ret.generation
		<< boost::format("%10.3f,") % watches[1].get()
		<< boost::format("%10.3f,") % watches[2].get()
		<< boost::format("%10.3f,") % watches[3].get()
		<< boost::format("%10.3f,") % watches[4].get()
		<< boost::format("%10.3f,") % watches[5].get()
		<< boost::format("%10.3f,") % watches[6].get()
		<< std::endl;
#endif

#if 0
	// Flush trace/debug streams.
	_tout->flush();
	_dout->flush();
#endif

	// Return the new generation.
	return ret;
}

bool Garnet::GPEngine::isFinished(const Garnet::Generation& generation) const 
{
	if ( generation.getGeneration() >= getConfiguration().maxGeneration )	{
		return true;
	}
	return false;
}

void Garnet::GPEngine::saveLastImages(const std::string& pathPrefix, const IndividualList& individuals, const std::vector<size_t>& indices) const
{
	std::for_each(indices.begin(), indices.end(), [&] (size_t i) {
		auto index = individuals[i].tempIndex;
		if ( index != std::numeric_limits<Garnet::Individual::history_t>::max() ) {
			for (auto t = 0u; t < _Nt; t++) {
				for (auto c = 0u; c < _Nc; c++) {
					std::string fn = (boost::format("%sA_I%04u_C%02u_T%03u.png") % pathPrefix % i % c % t).str();
					auto result = std::dynamic_pointer_cast<IVTreeStrategy::IVTreeIntermediate>(individuals[i].chromosome[c].intermediate);
					if (result) {
						result->iv[t].saveImage(fn);
					}
				}
#if 0
				for (auto r = 0u; r < _Nr; r++) {
					std::string fn = (boost::format("%sB_I%04u_R%02u_T%03u.png") % pathPrefix % i % r % t).str();
					_archive->saveImage(ADDR_I1B + 0x500000u + t * _Ni * _Nr + index * _Nr + r, fn);
				}
#endif
			}
		}
	});
}

void Garnet::GPEngine::setParameter(engine_param_t name, const boost::any& param)
{
	_params[name] = param;
}

const boost::any& Garnet::GPEngine::getParameter(engine_param_t name) const
{
	return _params.at(name);
}

Garnet::GPEnginePtr Garnet::GPEngine::create(const Garnet::Configuration& conf)
{
    return GPEnginePtr(new GPEngine());
}
