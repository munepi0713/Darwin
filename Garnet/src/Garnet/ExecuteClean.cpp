#include <regex>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <Garnet/Drivers.h>

namespace fs = boost::filesystem;

namespace {

int executeCleanProject(ConfigLoader::StringMap& params) {
	return 1;
}

int executeCleanCase(ConfigLoader::StringMap& params) {
   Garnet::Configuration conf;

	// 1. Load the configuration file.
	fs::path projdir(params["_Project"]);
	fs::path conffile = projdir / (params["_Project"] + ".garnet_conf.txt");
	fs::ifstream fin(conffile);
	if ( fin.fail() ) {
		throw std::string("E1: Failed to open ") + conffile.string();
	}
    conf.load(fin, 0);
	fin.close();
	conf.dir = params["_Project"] + "/";
	conf.prj = conffile.string();
	conf.title = params["_ProjectName"];

	// 2. For all generation directories to remove following files in the directories.
	std::for_each(fs::directory_iterator(projdir), fs::directory_iterator(), [](const fs::directory_entry& entry) {
		const fs::path& file = entry.path();
		std::string name = file.filename().string();
		if (std::regex_match(name, std::regex("[0-9]{6}"))) {
			std::for_each(fs::directory_iterator(file), fs::directory_iterator(), [](const fs::directory_entry& entryGen) {
				const fs::path& fileGen = entryGen.path();
				std::string nameGen = fileGen.filename().string();
				if (boost::ends_with(nameGen, ".png")) {
					boost::system::error_code ec;
					fs::remove(fileGen, ec);
				}
			});
		}
	});

	// 3. Remove following files from the case directory.
	boost::system::error_code ec;
	fs::remove(projdir / ".garnet_cp.txt", ec);
	fs::remove(projdir / "trace.------.txt", ec);

	return 0;
}

}//end of anonymous namespace

int Garnet::executeClean(const strings& args)
{
	ConfigLoader::StringMap params = parseCommandLine(args);

	if (boost::equals(params["_IsProject"], "True")) {
		return executeCleanProject(params);
	}
	else {
		return executeCleanCase(params);
	}
}
