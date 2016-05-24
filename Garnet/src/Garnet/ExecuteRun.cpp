
#include <array>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <Garnet/Drivers.h>
#include <Garnet/Project.h>

namespace fs = boost::filesystem;

namespace {

int executeRunProject(ConfigLoader::StringMap& params)
{
    Garnet::Project proj;

	// 1. Load the configuration file.
	fs::path projdir(params["_Project"]);
	fs::path projfile = projdir / (params["_Project"] + ".garnet_proj.txt");
	fs::ifstream fin(projfile);
	if ( fin.fail() ) {
		throw std::string("E1: Failed to open ") + projfile.string();
	}
    proj.load(fin, 0);
	fin.close();

	// 2. Start evolution process.
	const std::array<std::string, 3> _commands = {
		"run",
		"clean",
		"report",
	};
	fs::current_path(projdir);
	for_each(proj.scenario.begin(), proj.scenario.end(), [&] (const ConfigLoader::String& commandLine) {
		strings tokens = parse(commandLine);
		auto command = tokens[0];
		tokens.erase(tokens.begin());
		switch (std::distance(_commands.begin(), std::find(_commands.begin(), _commands.end(), command))) {
		case 0 :
			Garnet::executeRun(tokens);
			break;
		case 1 :
			Garnet::executeClean(tokens);
			break;
		case 2 :
			Garnet::executeReport(tokens);
			break;
		}
	});
	fs::current_path(params["_CurrentPath"]);

	return 0;
}

int executeRunCase(ConfigLoader::StringMap& params)
{
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

	// 2. Override configurations by command line arguments.
	if ( params.find("_MaxGeneration") != params.end() ) {
		conf.maxGeneration = std::stoul(params["_MaxGeneration"], 0, 0);
	}
	if ( params.find("_InitialGeneration") != params.end() ) {
		conf.nextGeneration = std::stoul(params["_InitialGeneration"], 0, 0);
	}
	else {
		conf.nextGeneration = -1;
	}

	// 3. Start evolution process.
	Garnet::Drivers::executeGarnet(conf);

	return 0;
}

}//end of anonymous namespace

int Garnet::executeRun(const strings& args)
{
	ConfigLoader::StringMap params = parseCommandLine(args);

	// 
	if (boost::equals(params["_IsProject"], "True")) {
		return executeRunProject(params);
	}
	else {
		return executeRunCase(params);
	}
}
