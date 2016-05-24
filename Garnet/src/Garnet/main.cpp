/**
 *  Garnet
 */
#pragma warning(disable: 4819)

#define _CRT_SECURE_NO_WARNINGS
#define BOOST_FILESYSTEM_NO_DEPRECATED

//#include <map>
#include <string>
#include <array>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <iterator>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <boost/format.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/sequential_vertex_coloring.hpp>

#include <Common/Text.h>
#include <PicturePerfectEngine/Exception.h>
#include <Garnet/Garnet.h>
#include <Garnet/PPEGraph.h>
#include <Garnet/Drivers.h>

typedef ConfigLoader::StringMap  StringMap;

void  printUsage();
int   dispatch(const strings& args);
void  dispatch(StringMap& args);
void  execute(StringMap& args);

/**
 *
 */
int main(int argc, const char* argv[])
{
	strings args = to_strings(argc, argv);
	args.erase(args.begin()); // Remove command name.

#if defined(_DEBUG)
	if (dispatch(args) != 0) {
		// Error.
		Garnet::executeHelp(args);
	}
	return 0;
#else
	try {
		if (dispatch(args) != 0) {
			// Error.
			Garnet::executeHelp(args);
		}
		return 0;
	}
	catch (const char* msg) {
		std::cerr << "[ERROR] " << msg << std::endl;
		return 1;
	}
	catch (std::string msg) {
		std::cerr << "[ERROR] " << msg << std::endl;
		return 1;
	}
	catch (cv::Exception& e) {
		std::cerr << "[ERROR] " << e.msg << " (Code: " << e.code << ")." << std::endl;
		return 1;
	}
	catch (PicturePerfect::Exception e) {
		std::cerr << "[ERROR] " << e.what() << std::endl;
		return 1;
	}
#endif
}

int dispatch(const strings& args_)
{
	std::array<std::string, 9> commands = {
		"newproj",
		"newcase",
		"run",
		"update",
		"report",
		"clean",
		"distclean",
		"archive",
		"help",
	};

	strings args = args_;

	if (args.empty()) {
		// Few arguments.
		return Garnet::executeHelp(args);
	}

	// Global dispach.
	auto command = args[0];
	args.erase(args.begin()); // Remove command.
	auto it = std::find(commands.begin(), commands.end(), command);
	if (it == commands.end()) return 1;
	switch (std::distance(commands.begin(), it)) {
	case 0: // newproj
		return Garnet::executeNewProj(args);
	case 1: // newcase
		return Garnet::executeNewCase(args);
	case 2: // run
		return Garnet::executeRun(args);
	case 3: // update
		return Garnet::executeUpdate(args);
	case 4: // report
		return Garnet::executeReport(args);
	case 5: // clean
		return Garnet::executeClean(args);
	case 6: // distclean
		return Garnet::executeDistClean(args);
	case 7: // archive
		return Garnet::executeArchive(args);
	case 8: // help
		return Garnet::executeHelp(args);
	}

	// Undefined error.
	return 1;
}

/**
 *
 */
int Garnet::executeHelp(const strings& args)
{
	std::cout <<
		"\nGarnet (build on " << __TIMESTAMP__ << ")"
		"\n(c) 2008-2014 Munehiro Doi"
		"\nAll rights reserved."
		"\n"
		"\n[Usage]"
		"\n  Garnet Command Project [Options]"
		"\n"
		"\n[Command]"
		"\n  newproj        Create new project."
		"\n  newcase        Create new case."
		"\n  info           Show project information."
//		"\n  queue          Queue."
		"\n  run            Execute evolution for a project or a case."
		"\n  update         Update project configuration."
		"\n  report         Make reports about a project or a case."
		"\n  clean          Clean intermediate information a project or a case."
		"\n  distclean      Delete all generated files for a project or a case."
		"\n  archive        Clean and archive a project or a case."
		"\n  help           Show help."
//		"\n  agent [-d directory]  Start as an agent."
		"\n"
		"\n[Options]"
		"\n  -G number      Set max generation."
		"\n  -S number      Set the number of random cases."
		"\n  -K number      Set the number of k-fold cross validation."
		"\n  -d directory   Set a directory name to be used."
		<< std::endl;

	return 0;
}
