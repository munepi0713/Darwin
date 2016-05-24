
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <Garnet/Drivers.h>
#include <Garnet/Project.h>

namespace fs = boost::filesystem;

namespace {
uint32_t _randomTable[100] = {
	675556231, 	123456789, 	793009821,	604492036,	 32161502,
	928089543,	562015825,	388269235,	795623078,	365887409,
	  6095187,	106978786,	237094288,	 99842303,	505801149,
	512875789,	672129171,	943762728,	700899905,	 80735504,
	519088843,	763170987,	683296795,	379738406,	941272581,
	563731386,	783857759,	473827139,	  3577223,	 44460145,
	284269597,	479124264,	567610914,	113841916,	 29651412,
	200358779,	933780151,	455230175,	 41914037,	555854492,
	858158623,	983592308,	907100376,	666440595,	291577421,
	183580485,	484783588,	951526768,	754702665,	480924878,
	876626731,	871468978,	637487530,	997053438,	 41988810,
	 63601404,	670978404,	833130310,	501051189,	271080919,
	 84092404,	277869522,	472558737,	172757196,	120840272,
	639681870,	227678884,	168868412,	888988357,	937027038,
	347445858,	184691493,	807520317,	754913791,	987091873,
	426961123,	416304151,	250884467,	346454379,	574944655,
	274779664,	781283131,	475763046,	899424997,	796297918,
	584005307,	144828745,	870976691,	187139365,	 65788925,
	661674015,	850449207,	654147486,	593652401,	170111357,
	887875887,	160641053,	898345517,	170502257,	490513354,
};
}//end of anonymous namespace

int Garnet::executeNewProj(const strings& args)
{
	ConfigLoader::StringMap params = parseCommandLine(args);
    Garnet::Configuration conf;
    Garnet::Project       proj;

	// 1. Load the original configuration.
	fs::path origconf(params["_Project"]);
	fs::ifstream fin(origconf);
	if ( fin.fail() ) {
		throw std::string("E1: Failed to open ") + origconf.string();
	}
    conf.load(fin, 0);

	// 2. Create project configurations wtih command line arguments.
	if ( params.find("_RandomCases") != params.end() ) {
		proj.randomCases = std::stoul(params["_RandomCases"], 0, 0);
	}
	if ( params.find("_MaxGeneration") != params.end() ) {
		proj.maxGeneration = std::stoul(params["_MaxGeneration"], 0, 0);
	}
	if ( params.find("_CrossValidation") != params.end() ) {
		proj.crossValidation = std::stoul(params["_CrossValidation"], 0, 0);
	}

	// 2. Create working directories.
	boost::system::error_code ec;
	fs::path workdir(params["_WorkingDirectory"]);
    if ( fs::exists(workdir, ec) ) {
		throw std::string("E2: Directory already exists. ") + workdir.string();
    }
	fs::path repodir = workdir / "reports";
	fs::create_directories(repodir, ec); // creates both workdir and workdir/reports.
	fs::path imagedir = workdir / "images";
	fs::create_directories(imagedir, ec); // creates workdir/images.

	// 3. Copy images from the original location to the directory in the project directory.
	auto copier = [&] (Task& task) {
		for_each(task.images.begin(), task.images.end(), [&] (std::string& filename) {
			fs::path orig(filename);
			fs::path dest = imagedir / (orig.filename());
			fs::copy(orig, dest, ec);
			filename = std::string("images/") + orig.filename().string();
		});
	};
	for_each(conf.tasks.begin(), conf.tasks.end(), copier);
	for_each(conf.tests.begin(), conf.tests.end(), copier);

	// 4. Create cases. (with overriding Random, Cross Validation)
	auto makeScenario = [&proj] (const std::string& caseName, int maxGen) {
		// Make scenario
		proj.scenario.push_back((boost::format("\"run %s -G %d\"") % caseName % maxGen).str());
		proj.scenario.push_back((boost::format("\"clean %s\"") % caseName).str());
		proj.scenario.push_back((boost::format("\"report %s -g %d\"") % caseName % maxGen).str());
	};
	auto createCase = [&proj, &workdir] (const std::string& caseName, int maxGen, uint32_t randomSeed, const Configuration& conf) {
		// Save config to temporary file.
		fs::path tempfile = workdir / (caseName + ".garnet.txt");
		fs::ofstream fout(tempfile);
		conf.save(fout);
		fout.close();

		// Create case.
		strings caseArgs;
		caseArgs.push_back((boost::format("%s/%s.garnet.txt") % workdir.string() % caseName).str());
		caseArgs.push_back("-G");
		caseArgs.push_back((boost::format("%u") % maxGen).str());
		caseArgs.push_back("-s");
		caseArgs.push_back((boost::format("%u") % randomSeed).str());
		caseArgs.push_back("-d");
		caseArgs.push_back((workdir / caseName).string());
		executeNewCase(caseArgs);

		// Delete temporary file.
		fs::remove(tempfile);
	};
	if (proj.crossValidation == 0) {
		// No cross validation. Use only Tasks.
		for (auto r = 0; r < proj.randomCases; r++) {
			for (auto g = 0; g < proj.maxGeneration; g += proj.generationInterleave) {
				auto caseName = (boost::format("r%02d") % r).str();
				auto maxGen = std::min(g + proj.generationInterleave, (int)proj.maxGeneration);

				makeScenario(caseName, maxGen);

				if (g == 0) {
					createCase(caseName, maxGen, _randomTable[r], conf);
				}
			}
		}
	}
	else {
		// Cross validation enabled.
		TaskList tasks;
		tasks.insert(tasks.end(), conf.tasks.begin(), conf.tasks.end());
		tasks.insert(tasks.end(), conf.tests.begin(), conf.tests.end());

		for (auto r = 0; r < proj.randomCases; r++) {
			for (auto g = 0; g < proj.maxGeneration; g += proj.generationInterleave) {
				for (auto k = 0; k < proj.crossValidation; k++) {
					auto caseName = (boost::format("r%02dk%02d") % r % k).str();
					auto maxGen = std::min(g + proj.generationInterleave, (int)proj.maxGeneration);

					makeScenario(caseName, maxGen);

					if (g == 0) {
						Configuration caseconf = conf;
						caseconf.tasks.clear();
						caseconf.tests.clear();
						for (auto t = 0; t < tasks.size(); t++) {
							if ((t % proj.crossValidation) == k) {
								caseconf.tasks.push_back(tasks[t]);
							}
							else {
								caseconf.tests.push_back(tasks[t]);
							}
						}

						createCase(caseName, maxGen, _randomTable[r], caseconf);
					}
				}
			}
		}
	}

	// 5. Save the project configuration file.
	fs::path projfile = workdir / (params["_WorkingDirectory"] + ".garnet_proj.txt");
	fs::ofstream fout(projfile);
	if ( fout.fail() ) {
		throw std::string("E3: Failed to open ") + projfile.string();
	}
    proj.save(fout);
	fout.close();

	// 6. Done!
	return 0;
}
