#ifndef GARNET__DRIVERS__H__
#define GARNET__DRIVERS__H__

#include <boost/filesystem.hpp>

#include <Garnet/Individual.h>
//#include <Common/ConfigLoader.h>
#include <Garnet/Configuration.h>

namespace Garnet
{

class Drivers
{
public :

	typedef ConfigLoader::StringMap StringMap;

	static void executeGarnet(StringMap& args);
	static void executeGarnet(const Configuration& conf);
	static void executeTree(StringMap& args);
	static void executeTrees(StringMap& args);
	static void executeDetail(StringMap& args);
	static void executeReport(StringMap& args);
	static void executeModules(StringMap& args, std::map<std::string, bool>& activeModules);
	static void executeFitnessVector(StringMap& args);

	static void resolveProjectInformation(const StringMap& args, Garnet::Configuration* conf);

};//end of class Drivers

void executeGarnet(const Configuration& conf);

/**
*  Global command.
*/
int executeNewProj(const strings& args);
int executeNewCase(const strings& args);
int executeUpdate(const strings& args);
int executeRun(const strings& args);
int executeReport(const strings& args);
int executeClean(const strings& args);
int executeDistClean(const strings& args);
int executeArchive(const strings& args);
int executeHelp(const strings& args);

/**
 *  @return  -1: no results. >0: last generation.
 */
int findLastGeneration(const boost::filesystem::path& projdir);
uint32_t findBestIndividualIndex(const Garnet::IndividualList& individuals);

/**
 *  Utility.
 */
ConfigLoader::StringMap parseCommandLine(const strings& args);

}//end of namespace Garnet

#endif//GARNET__DRIVERS__H__
