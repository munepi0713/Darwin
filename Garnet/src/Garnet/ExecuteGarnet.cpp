#include <fstream>
#include <iostream>
#include <sstream>
#include <queue>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <Common/ConfigLoader.h>
#include <Common/File.h>
#include <Common/GetTime.h>
#include <Common/Text.h>
#include <Common/PrintRange.h>

#include <Garnet/Affinity.h>
#include <Garnet/Drivers.h>
#include <Garnet/Dictionary.h>
#include <Garnet/Garnet.h>
#include <Garnet/IVTreeStrategy.h>

#include <PicturePerfectEngine/Archive.h>
#include <PicturePerfectEngine/PicturePerfectEngine.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

namespace fs = boost::filesystem;

void Garnet::Drivers::resolveProjectInformation(const StringMap& args, Garnet::Configuration* conf)
{
	if ( args.find("_Project") == args.end() ) {
        throw "A project name must be specified.";
	}
    // Project title.
	//   'title' is made by removing path and extension from _Project.
	//   Note: All extensions will be removed if multiple extensions are included.
	{
        std::string::size_type st, ed;

		st = args.at("_Project").find_last_of("/\\");
		st = (st == std::string::npos ? 0 : st + 1);

		ed = args.at("_Project").find_first_of('.', st);

		conf->title = args.at("_Project").substr(st, ed - st);
	}
    // Working directory.
	//   If _WorkingDirectory is specified, use it.
	//   otherwise, if _Project includes a path, use "thepath/title".
	//   otherwise, use "./title"
    if ( args.find("_WorkingDirectory") != args.end() ) {
        conf->dir = args.at("_WorkingDirectory") + "/";
	}
	else {
		std::string::size_type pos = args.at("_Project").find_last_of("/\\");
		conf->dir = (boost::format("%s/%s/") 
			% (pos != std::string::npos ? args.at("_Project").substr(0, pos) : ".") 
			% conf->title).str();
    }
    // Project filename.
	{
        std::string::size_type st, pos;

		st = args.at("_Project").find_last_of("/\\");
		st = (st == std::string::npos ? 0 : st + 1);

        if ( (pos = args.at("_Project").find(".", st)) != std::string::npos ) {
            conf->prj   = args.at("_Project");
        }
        else {
            conf->prj   = args.at("_Project") + ".garnet";
        }
    }
    // Verbose
    if ( args.find("_Verbose") != args.end() ) {
        conf->verbose = true;
    }
}

void updateCPUAffinity(const std::string& cpuAffinity, const std::string& gpuAffinity)
{
	std::string affinity[2] = { cpuAffinity, gpuAffinity };
	uint32_t numCPUs[2] = { getNumCPUs(), 0 };

	// Default Affinity.
	if ( affinity[0].empty() ) {
		// Based on NumMaxCPUs
		affinity[0] = "1*";
	}
	if ( affinity[1].empty() ) {
		// No GPU by default.
		affinity[1] = "0*";
	}

	// Expand wild card.
	for (auto i = 0u; i < 2; i++) {
		std::string& s = affinity[i];
		if ( Text::endsWith(s, "*") ) {
			if ( s.size() < 2 ) {
				throw "Invalid format of Affinity Mask.";
			}
			s.pop_back();
			s.append(std::string(numCPUs[i], s.back()));
		}
	}

	// Trancate to match the number of existing cores.
	for (auto i = 0u; i < 2; i++) {
		std::string& s = affinity[i];
		if ( s.size() > numCPUs[i] ) {
			s.resize(numCPUs[i]);
		}
	}

	// Check the number of '1'.
	uint32_t numCores[2] = { 0u, 0u };
	for (auto i = 0u; i < 2; i++) {
		std::string& s = affinity[i];
		uint32_t&    n = numCores[i];
		std::for_each(s.crbegin(), s.crend(), [&] (std::string::value_type c) {
			n <<= 1;
			if ( c == '1' ) {
				n |= 1;
			}
		});
	}
	if ( numCores[0] + numCores[1] == 0 ) {
		throw "At least 1 core must be specified for Affinity Mask.";
	}

	// Set affinity.
	setCPUAffinity(numCores[0]);
}

namespace {
	template <typename It1, typename It2>
	bool less(It1 first1, It1 last1, It2 first2) {
		for (; first1 != last1; first1++, first2++) {
			if ( !(*first1 < *first2) ) {
				return false;
			}
		}
		return true;
	}
}

/**
 *
 */
void Garnet::Drivers::executeGarnet(StringMap& args)
{
	boost::system::error_code ec;

    Garnet::Configuration conf;

    //-----------------------------------------------------
    //  Check common settings.
    //-----------------------------------------------------
	resolveProjectInformation(args, &conf);

	const std::string& title = conf.title;
	const std::string& prj = conf.prj;
	const std::string& dir = conf.dir;
	const bool&        verbose = conf.verbose;

    //-----------------------------------------------------
    //  Check working directory exists.
    //-----------------------------------------------------

    // Create working directory.
	
    if ( !fs::exists(dir, ec) ) {
		fs::create_directories(dir, ec);
    }

    // Load configuration file.
	if ( fs::exists(dir + CONF_FILENAME, ec) ) {
        // Config file exists in the directory. Use it.
	    std::ifstream fin;
        fin.open((dir + CONF_FILENAME).c_str());
	    if ( fin.fail() ) {
		    throw std::string("E1: Failed to open ") + prj;
	    }
        conf.load(fin, 0);
    }
    else {
        // Config file does not exist.
        // Load original conf and save it to the working directory.
	    std::ifstream fin;
        fin.open(prj.c_str());
	    if ( fin.fail() ) {
		    throw std::string("E2: Failed to open ") + prj;
	    }
        conf.load(fin, 0);

		if ( args.find("_RandomSeed") != args.end() ) {
			// Override random seed if specified.
			conf.randomSeed = strtoul(args["_RandomSeed"].c_str(), 0, 0);
		}

		std::string tempname = dir + ".temp";
		std::string savename = dir + CONF_FILENAME;
	    std::ofstream fout;
        fout.open(tempname.c_str());
	    if ( fout.fail() ) {
		    throw std::string("E3: Failed to open ") + (dir + CONF_FILENAME);
	    }
        conf.save(fout);
		fout.close();

		fs::rename(
				fs::path(Text::toWideChar(tempname)),
				fs::path(Text::toWideChar(savename)));
    }
    if ( !conf.isValid() ) {
        throw "Invalid configuration.";
    }

	// Override maximum generation
	if ( args.find("_MaxGeneration") != args.end() ) {
		conf.maxGeneration = stoul(args["_MaxGeneration"], 0, 0);
	}
	// Override initial generation
	if ( args.find("_InitialGeneration") != args.end() ) {
		conf.nextGeneration = stoul(args["_InitialGeneration"], 0, 0);
	}
	else {
		conf.nextGeneration = -1;
	}

	// Go next.
	executeGarnet(conf);
}

void Garnet::Drivers::executeGarnet(const Configuration& conf)
{
	boost::system::error_code ec;
	StopWatch watchAll;

	const std::string& dir = conf.dir;

	// Initialize settings.
	Settings::load(conf);

    //-----------------------------------------------------
    //  Create GA Engine
    //-----------------------------------------------------
    Garnet::GPEnginePtr  ga;
	{
		std::ofstream ftrace((boost::format("%strace.------.txt") % conf.dir).str().c_str());
		setTraceStream(&ftrace);

		ga = Garnet::GPEngine::create(conf);
		if ( !ga ) {
			throw "Invalid GA Engine Type.";
		}

		setTraceStream(0);
		ftrace.close();
	}

    //-----------------------------------------------------
    //  Set affinity control
    //-----------------------------------------------------
	{
		// CPU Affinity
		std::string affinity[2];
		affinity[0] = conf.cpuAffinity.size() > 0 ? conf.cpuAffinity[0] : std::string(conf.numMaxCPUs, '1') + "0*";
		affinity[1] = conf.cpuAffinity.size() > 1 ? conf.cpuAffinity[1] : std::string("0*");
		updateCPUAffinity(affinity[0], affinity[1]);

		// Number of Threads
		ga->setParameter(Garnet::GPEngine::NUMBER_OF_THREADS, int(conf.numThreads[0]));
	}

	//-----------------------------------------------------
	//  Prepare directories.
	//-----------------------------------------------------
	fs::path projdir(Text::toWideChar(conf.dir));
	fs::path gendir = projdir / "generations";
	if (!fs::exists(gendir)) {
		fs::create_directories(gendir);
	}
	fs::path reportdir = projdir / "reports";
	if (!fs::exists(reportdir)) {
		fs::create_directories(reportdir);
	}

    //-----------------------------------------------------
    //  Prepare initial generation.
    //-----------------------------------------------------
	Garnet::Generation generation;
	Summaries summaries;

	// Initial individuals (generation) will be prepared in
	// following rules;
	// 1. If the initial generation is specified and it is 0,
	//    generate individuals randomly.
	// 2. If the initial generation is specified and it is not 0,
    //    load from generation log having specified generation.
	//    (Error exit if the file does not exist.)
	// 3. If the initial generaiton is not specified and a checkpoint
	//    file exists, load from it.
	// 4. If any conditions above are not met, generate individuals
	//    randomly.
    std::cout << "\nInitial Generation";
	{
		std::ofstream ftrace;
		if ( isTrace() ) {
			ftrace.open((boost::format("%strace.000000.txt") % conf.dir).str().c_str());
			setTraceStream(&ftrace);
		}

		//
		bool isEvaluated = false;
		StopWatch watch;
		if ( conf.nextGeneration != -1 ) {
			uint32_t gnum = conf.nextGeneration;
			if ( gnum == 0 ) {
				// 1. Restart from initial state.
				std::cout << "\n  Generating C1 ... " << std::flush;
				generation = Garnet::Generation::create(ga);
				std::cout << "Done";
			}
			else {
				// 2. Load from specified generation.
				std::cout << "\n  Loading Generation #" << gnum << " ... " << std::flush;
				fs::path genfile = Common::File::findGenerationFile(fs::path(dir));
				Common::File::readFromFile(genfile,
					[&generation](std::istream& ins) {
					generation.load(ins, 0);
				});
				isEvaluated = true;
				std::cout << "Done";
			}
		}
		else {
			std::string fn = (dir + CP_FILENAME);
			if (fs::exists(dir + CP_FILENAME, ec) || fs::exists(fn + ".gz", ec)) {
				// 3. Load from check point.
				std::cout << "\n  Loading Auto-Saved Check Point ... " << std::flush;
				Common::File::readFromFile(fn,
					[&generation](std::istream& ins) {
					generation.load(ins, 0);
				});
				isEvaluated = true;
				std::cout << "Done";
			}
			else {
				// 4. Start from initial state.
				std::cout << "\n  Generating C4 ... " << std::flush;
				generation = Garnet::Generation::create(ga);
				std::cout << "Done";
			}
		}

		// Evaluate and save log in case 1 or 4.
        if ( !isEvaluated ) {
			StopWatch watchE;
			evaluate(ga, generation);
			watchE.stop();
			writeGenerationLog(ga, generation, conf.dir);

			addSummary(ga, generation, watchE.get(), summaries);
			//writeGenerationImages(ga, generation, dir);
			writeSummary(ga, summaries);
			summaries.clear();
        }

        // Show Status
		watch.stop();
		printInformation(generation, watch.get());

		//
		setTraceStream(0);
		ftrace.close();
    }

	//-----------------------------------------------------
    //  Perform genetic algorithm.
	//-----------------------------------------------------
	fs::path cpuFileName(CPU_FILENAME);
	std::time_t             lastUpdateCpuFile = 0u;
	bool      loop = true;
	std::vector<double>    bestFitness(conf.fitness.size(), std::numeric_limits<double>::max());
	uint32_t  lastImageSaving = generation.getGeneration();
	StopWatch watchCP;
	while ( loop ) {
		std::ofstream ftrace;
		if ( isTrace() ) {
			ftrace.open((boost::format("%strace.%06u.txt") % conf.dir % (generation.getGeneration() + 1)).str().c_str());
			setTraceStream(&ftrace);
		}

		//
		StopWatch watch;
		StopWatch watchE;
		bool changed = false;

        // Runtime Affinity Control
		if ( fs::exists(cpuFileName) ) {
			std::time_t updateTime = fs::last_write_time(cpuFileName);
			if ( updateTime > lastUpdateCpuFile ) {
	            std::ifstream fcpu(CPU_FILENAME);
				if ( fcpu.good() ) {
					std::string threads, cpu, gpu;
					fcpu >> threads >> cpu >> gpu;
					fcpu.close();
					lastUpdateCpuFile = updateTime;

					// Number of Threads
					if ( !threads.empty() ) {
						int numThreads = std::stoi(threads);
						std::cout << "\nNum Threads Changed: " << std::flush;
						ga->setParameter(Garnet::GPEngine::NUMBER_OF_THREADS, int(numThreads));
						std::cout << numThreads << std::flush;
					}

					// CPU Affinity
					if ( !cpu.empty() ) {
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
		isFinished = ga->isFinished(generation);
		if ( isFinished ) {
			std::cout << "Finished.";
			loop = false;
			goto Closing;
		}
		else {
			std::cout << "Continue.";
		}

		// GA2. Process next generation.
        std::cout << "\n\nGeneration " << (generation.getGeneration() + 1);
		if ( isProcessTrace() ) std::cerr << "\n\nGeneration " << (generation.getGeneration() + 1);

        // Reproduce (including fitness evaluation)
		// # generation number will increment after reproduction.
		reproduce(ga, generation);

		auto& f = generation.individuals.front().fitness;
		if ( less(f.cbegin(), std::next(f.cbegin(), f.size() - 1), bestFitness.cbegin()) ) {
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
		addSummary(ga, generation, watchE.get(), summaries);
		// Check condition to save check point and summary file.
		bool save = false;
		if ( (conf.reportInterval > 0u) && (summaries.size() >= conf.reportInterval) ) {
			save = true;
		}
		if ( conf.reportWhenImproved && changed ) {
			save = true;
		}
		if ( (conf.reportTimeInterval > 0u) && (watchCP.lap() >= conf.reportTimeInterval) ) {
			save = true;
		}
		// Save check point and summary file if the condition been met.
		if ( save || !loop ) {

			writeGenerationLog(ga, generation, conf.dir);
#if 0
			if ( !loop || ((conf.imageSavingInterval > 0) && ((generation.getGeneration() - lastImageSaving) >= conf.imageSavingInterval)) ) {
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
	watchAll.stop();

	// Finished.
	//times[TIME_LIFE] = watchAll.lap();
	std::cout << boost::format("\n\nFinished. %.3f [sec]") % watchAll.get() << std::endl;
}


void Garnet::executeGarnet(const Configuration& conf) {
    boost::system::error_code ec;
    StopWatch watchAll;

    const std::string& dir = conf.dir;

    // Initialize settings.
    Settings::load(conf);

    //-----------------------------------------------------
    //  Create GA Engine
    //-----------------------------------------------------
    Garnet::GPEnginePtr  ga;
    {
        Garnet::GPEngineBuilder builder;

        ga = builder
            .addCatalog(IVTreeStrategy::catalog)
            .setConfigurations(conf)
            .create();
        if (!ga) {
            throw "Invalid GA Engine Type.";
        }
    }

    //-----------------------------------------------------
    //  Set affinity control
    //-----------------------------------------------------
    {
        // CPU Affinity
        std::string affinity[2];
        affinity[0] = conf.cpuAffinity.size() > 0 ? conf.cpuAffinity[0] : std::string(conf.numMaxCPUs, '1') + "0*";
        affinity[1] = conf.cpuAffinity.size() > 1 ? conf.cpuAffinity[1] : std::string("0*");
        updateCPUAffinity(affinity[0], affinity[1]);

        // Number of Threads
        ga->setParameter(Garnet::GPEngine::NUMBER_OF_THREADS, int(conf.numThreads[0]));
    }

    //-----------------------------------------------------
    //  Prepare directories.
    //-----------------------------------------------------
    fs::path projdir(Text::toWideChar(conf.dir));
    fs::path gendir = projdir / "generations";
    if (!fs::exists(gendir)) {
        fs::create_directories(gendir);
    }
    fs::path reportdir = projdir / "reports";
    if (!fs::exists(reportdir)) {
        fs::create_directories(reportdir);
    }

    //-----------------------------------------------------
    //  Prepare initial generation.
    //-----------------------------------------------------
    Garnet::Generation generation;
    Summaries summaries;

    // Initial individuals (generation) will be prepared in
    // following rules;
    // 1. If the initial generation is specified and it is 0,
    //    generate individuals randomly.
    // 2. If the initial generation is specified and it is not 0,
    //    load from generation log having specified generation.
    //    (Error exit if the file does not exist.)
    // 3. If the initial generaiton is not specified and a checkpoint
    //    file exists, load from it.
    // 4. If any conditions above are not met, generate individuals
    //    randomly.
    std::cout << "\nInitial Generation";
    {
        std::ofstream ftrace;
        if (isTrace()) {
            ftrace.open((boost::format("%strace.000000.txt") % conf.dir).str().c_str());
            setTraceStream(&ftrace);
        }

        //
        bool isEvaluated = false;
        StopWatch watch;
        if (conf.nextGeneration != -1) {
            uint32_t gnum = conf.nextGeneration;
            if (gnum == 0) {
                // 1. Restart from initial state.
                std::cout << "\n  Generating C1 ... " << std::flush;
                generation = Garnet::Generation::create(ga);
                std::cout << "Done";
            }
            else {
                // 2. Load from specified generation.
                std::cout << "\n  Loading Generation #" << gnum << " ... " << std::flush;
                fs::path genfile = Common::File::findGenerationFile(fs::path(dir));
                Common::File::readFromFile(genfile,
                    [&generation](std::istream& ins) {
                    generation.load(ins, 0);
                });
                isEvaluated = true;
                std::cout << "Done";
            }
        }
        else {
            std::string fn = (dir + CP_FILENAME);
            if (fs::exists(dir + CP_FILENAME, ec) || fs::exists(fn + ".gz", ec)) {
                // 3. Load from check point.
                std::cout << "\n  Loading Auto-Saved Check Point ... " << std::flush;
                Common::File::readFromFile(fn,
                    [&generation](std::istream& ins) {
                    generation.load(ins, 0);
                });
                isEvaluated = true;
                std::cout << "Done";
            }
            else {
                // 4. Start from initial state.
                std::cout << "\n  Generating C4 ... " << std::flush;
                generation = Garnet::Generation::create(ga);
                std::cout << "Done";
            }
        }

        // Evaluate and save log in case 1 or 4.
        if (!isEvaluated) {
            StopWatch watchE;
            evaluate(ga, generation);
            watchE.stop();
            writeGenerationLog(ga, generation, conf.dir);

            addSummary(ga, generation, watchE.get(), summaries);
            //writeGenerationImages(ga, generation, dir);
            writeSummary(ga, summaries);
            summaries.clear();
        }

        // Show Status
        watch.stop();
        printInformation(generation, watch.get());

        //
        setTraceStream(0);
        ftrace.close();
    }

    //-----------------------------------------------------
    //  Perform genetic algorithm.
    //-----------------------------------------------------
    fs::path cpuFileName(CPU_FILENAME);
    std::time_t             lastUpdateCpuFile = 0u;
    bool      loop = true;
    std::vector<double>    bestFitness(conf.fitness.size(), std::numeric_limits<double>::max());
    uint32_t  lastImageSaving = generation.getGeneration();
    StopWatch watchCP;
    while (loop) {
        std::ofstream ftrace;
        if (isTrace()) {
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
        isFinished = ga->isFinished(generation);
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
        addSummary(ga, generation, watchE.get(), summaries);
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
    watchAll.stop();

    // Finished.
    //times[TIME_LIFE] = watchAll.lap();
    std::cout << boost::format("\n\nFinished. %.3f [sec]") % watchAll.get() << std::endl;
}
