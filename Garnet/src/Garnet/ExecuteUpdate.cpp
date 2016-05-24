
#include <boost/algorithm/string/predicate.hpp>

#include <Garnet/Drivers.h>


namespace {

int executeUpateProject(ConfigLoader::StringMap& params) {
	return 1;
}

int executeUpateCase(ConfigLoader::StringMap& params) {
	return 1;
}

}//end of anonymous namespace

int Garnet::executeUpdate(const strings& args)
{
	ConfigLoader::StringMap params = parseCommandLine(args);

	if (boost::equals(params["_IsProject"], "True")) {
		return executeUpateProject(params);
	}
	else {
		return executeUpateCase(params);
	}
}
