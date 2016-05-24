
#include <boost/algorithm/string/predicate.hpp>

#include <Garnet/Drivers.h>

namespace {

int executeDistCleanProject(ConfigLoader::StringMap& params) {
	return 1;
}

int executeDistCleanCase(ConfigLoader::StringMap& params) {
	return 1;
}

}//end of anonymous namespace

int Garnet::executeDistClean(const strings& args)
{
	ConfigLoader::StringMap params = parseCommandLine(args);

	if (boost::equals(params["_IsProject"], "True")) {
		return executeDistCleanProject(params);
	}
	else {
		return executeDistCleanCase(params);
	}
}
