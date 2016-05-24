
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <Garnet/Drivers.h>

namespace fs = boost::filesystem;

int Garnet::executeNewCase(const strings& args)
{
	ConfigLoader::StringMap params = parseCommandLine(args);
    Garnet::Configuration conf;

	// 1. Load the original configuration.
	fs::path origconf(params["_Project"]);
	fs::ifstream fin(origconf);
	if ( fin.fail() ) {
		throw std::string("E1: Failed to open ") + origconf.string();
	}
    conf.load(fin, 0);

	// 2. Override configurations by command line arguments.
	if ( params.find("_RandomSeed") != params.end() ) {
		// Override random seed if specified.
		conf.randomSeed = std::stoul(params["_RandomSeed"], 0, 0);
	}
	if ( params.find("_MaxGeneration") != params.end() ) {
		conf.maxGeneration = std::stoul(params["_MaxGeneration"], 0, 0);
	}

	// 3. Create working directories.
	boost::system::error_code ec;
	fs::path workdir(params["_WorkingDirectory"]);
    if ( fs::exists(workdir, ec) ) {
		throw std::string("E2: Directory already exists. ") + workdir.string();
    }
	fs::create_directories(workdir / "reports", ec); // creates both workdir and workdir/reports.

	// 4. Save configuration file.
	fs::path workconf = workdir / (workdir.filename().string() + ".garnet_conf.txt");
	fs::ofstream fout(workconf);
	if ( fout.fail() ) {
		throw std::string("E3: Failed to open ") + workconf.string();
	}
    conf.save(fout);
	fout.close();

	// 4. Done!
	return 0;
}
