
#include <boost/algorithm/string/predicate.hpp>

#include <Garnet/Drivers.h>

namespace {

int executeArchiveProject(ConfigLoader::StringMap& params) {
	return 1;
}

int executeArchiveCase(ConfigLoader::StringMap& params) {
	return 1;
}

}//end of anonymous namespace

int Garnet::executeArchive(const strings& args)
{
	ConfigLoader::StringMap params = parseCommandLine(args);

	if (boost::equals(params["_IsProject"], "True")) {
		return executeArchiveProject(params);
	}
	else {
		return executeArchiveCase(params);
	}
}
