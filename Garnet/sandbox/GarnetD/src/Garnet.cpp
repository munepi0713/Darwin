
#include <GarnetD.h>

/**
 *
 */
void execute(StringMap& args)
{
    std::string title;
    std::string prj;
    std::string dir;
    bool        verbose = false;

    //-----------------------------------------------------
    //  Check common settings.
    //-----------------------------------------------------
    // Project name.
	if ( args.find("_Project") != args.end() ) {
        std::string::size_type pos;

        if ( (pos = args["_Project"].find(".garnet")) == args["_Project"].length() - std::string(".garnet").length() ) {
            title = args["_Project"].substr(0, pos);
            prj   = args["_Project"];
        }
        else if ( (pos = args["_Project"].find(".")) != std::string::npos ) {
            title = args["_Project"].substr(0, pos);
            prj   = args["_Project"];
        }
        else {
            title = args["_Project"];
            prj   = args["_Project"] + ".garnet";
        }
    }
    else {
        throw "A project name must be specified.";
	}
    // Working directory.
    if ( args.find("_WorkingDirectory") == args.end() ) {
        dir = title + "/";
    }
    else {
        dir = args["_WorkingDirectory"] + "/";
    }
    // Verbose
    if ( args.find("_Verbose") != args.end() ) {
        verbose = true;
    }

    //-----------------------------------------------------
    //  Check working directory exists.
    //-----------------------------------------------------
    Garnet::Configuration conf;

    // Create working directory.
    if ( !existsDir(dir) ) {
        makeDir(dir);
    }

    // Load configuration file.
    if ( existsFile(dir + ".garnet_conf") ) {
        // Config file exists in the directory. Use it.
	    std::ifstream fin;
        fin.open((dir + ".garnet_conf").c_str());
	    if ( fin.fail() ) {
		    throw std::string("Failed to open ") + prj;
	    }
        conf.load(fin, 0);
    }
    else {
        // Config file does not exist.
        // Load original conf and save it to the working directory.
	    std::ifstream fin;
        fin.open(prj.c_str());
	    if ( fin.fail() ) {
		    throw std::string("Failed to open ") + prj;
	    }
        conf.load(fin, 0);

	    std::ofstream fout;
        fout.open((dir + ".garnet_conf").c_str());
	    if ( fout.fail() ) {
		    throw std::string("Failed to open ") + (dir + ".garnet_conf");
	    }
        conf.save(fout);
    }
    if ( !conf.isValid() ) {
        throw "Invalid configuration.";
    }
    conf.dir   = dir;
    conf.title = title;

    //-----------------------------------------------------
    //  Set affinity control
    //-----------------------------------------------------
    int numMaxCPUs = conf.numMaxCPUs;
    setNumCPUs(numMaxCPUs);

    //-----------------------------------------------------
    //  Create GA Engine
    //-----------------------------------------------------
    Garnet::GAEngine*  ga = 0;
    ga = Garnet::GAEngine::create(conf);
    if ( ga == 0 ) {
        throw "Invalid GA Engine Type.";
    }

    //-----------------------------------------------------
    //  Load initial generation.
    //-----------------------------------------------------
	Garnet::Generation generation;

    // Check checkpoint file.
    if ( existsFile(dir + ".garnet_cp") ) {
        // Checkpoint file exists.
        // Load it and continue.
        std::ifstream fgen(dir + ".garnet_cp");
        generation = ga->createGenerationFromCheckPoint(fgen);
    }
    else {
        // Checkpoint file does not exist.
        if ( args.find("_InitialGeneration") == args.end() ) {
            // Generate initial generation.
            generation = ga->createGeneration();
        }
        else {
            // Load initial generation from a file. (It may be a result of a previous experiment.)
            char buf[256];
            sprintf(buf, "%s%s.%05u.txt", dir.c_str(), title.c_str(), strtoul(args["_InitialGeneration"].c_str(), 0, 0));
            std::ifstream fgen(buf);
            generation = ga->createGenerationFromFile(fgen);
        }

        // Fitness evaluation
        //   Always calculate fitness for keeping precision
        //   if previous results are available when the generation is created
        //   from a previous results.
        std::cout << "Initial Evaluation" << std::endl;
        {
		    double st, ed;
	        st = getSec();
            ga->evaluate(generation);
	        ed = getSec();
            // Show Status
            std::cout << "  Best Fitness = " << std::setprecision(6) << generation.getBestFitness() << "\n";
            std::cout << "  Time = " << std::setprecision(6) << (ed - st) << " [sec]" << std::endl;

            // Save
            if ( args.find("_InitialGeneration") == args.end() ) {
                char buf[256];
                std::ofstream fout;

                // Save the first generation.
                std::cout << "  Save ... ";
                sprintf(buf, "%s%s.%05u.txt", dir.c_str(), title.c_str(), 0);
                fout.open(buf);
                generation.save(fout);
                fout.close();
                std::cout << "OK. " << std::endl;

                // Create a summary file.
                std::cout << "  Summary ... ";
                sprintf(buf, "%s%s.csv", dir.c_str(), title.c_str());
                fout.open(buf);
                fout << "Generation,Best Fitness,Seconds\n";
                fout << generation.getGeneration() << "," << generation.getBestFitness() << "," << std::setprecision(6) << (ed - st) << std::endl;
                fout.close();
                std::cout << " OK.\n";
            }
        }
    }

	//-----------------------------------------------------
    //  Perform genetic algorithm.
	//-----------------------------------------------------	    
	double stTotal, edTotal;
	stTotal = getSec();
	while ( generation.getGeneration() < conf.maxGeneration ) {
        //----- Affinity Control -----
        {
            std::ifstream fcpu(".num_cpus");
            if ( fcpu.good() ) {
                int num;
                fcpu >> num;
                if ( num != numMaxCPUs ) {
                    std::cout << "Num CPU Changed: current=" << numMaxCPUs << ", new=" << num << std::endl;
                    setNumCPUs(numMaxCPUs = num);

                    conf.numMaxCPUs = num;
                }
            }
        }

        //----- GA -----
		double st, ed;
        double reproduceTime;
        char buf[256];
		std::ofstream fout;

        std::cout << "\nGeneration " << (generation.getGeneration() + 1) << std::endl;

        // Reproduce (including fitness evaluation)
        if ( generation.getGeneration() < conf.maxGeneration ) {
		    st = getSec();
		    generation = ga->reproduce(generation);
		    ed = getSec();
            reproduceTime = ed - st;
        }

        // Save check point.
        std::cout << "  Save check point ... ";
        sprintf(buf, "%s.garnet_cp", dir.c_str());
		fout.open(buf);
        generation.save(fout);
        fout.close();
        std::cout << " OK.\n";

        // Save Generation
        std::cout << "  Save ... ";
		st = getSec();
        sprintf(buf, "%s%s.%05u.txt", dir.c_str(), title.c_str(), generation.getGeneration());
		fout.open(buf);
        generation.save(fout);
        fout << "# " << std::setprecision(6) << reproduceTime << " [sec]\n";
        fout.close();
		ed = getSec();
        std::cout << " OK.\n";
        std::cout << "  Best Fitness = " << std::setprecision(6) << generation.getBestFitness() << "\n";
        std::cout << "  Time = " << std::setprecision(6) << reproduceTime << " [sec]" << std::endl;

        // Save Summary
        std::cout << "  Summary ... ";
        sprintf(buf, "%s%s.csv", dir.c_str(), title.c_str());
		fout.open(buf, std::ios::app);
        fout << generation.getGeneration() << "," << std::setprecision(16) << generation.getBestFitness() << "," << std::setprecision(6) << reproduceTime << std::endl;
        fout.close();
        std::cout << " OK.\n";
	}
	edTotal = getSec();
	std::cout << "\nFinished. " << std::setprecision(6) << (edTotal - stTotal) << " [sec]\n" << std::endl;
}
