#ifndef GARNET__PROJECT__H__
#define GARNET__PROJECT__H__

#include <Common/ConfigLoader.h>

namespace Garnet
{

/**
 *
 */
struct Project : public ConfigLoader
{
    Integer            maxGeneration;
    Integer            randomCases;
    Integer            crossValidation;
	StringList         scenario;
    Integer            generationInterleave;
	Boolean            reportParetoFront;
	Boolean            reportFitness;
	Boolean            reportFilterCount;
	Boolean            reportGeneratedTree;
	Boolean            reportTreeDetail;

    Project();

    void save(std::ostream& fout) const;
};

}//end of namespace Garnet


#endif//GARNET__PROJECT__H__
