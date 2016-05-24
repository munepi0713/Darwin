
#include <iostream>
#include <iomanip>

#include <Garnet/Project.h>

Garnet::Project::Project()
{
	maxGeneration = 5000;
	randomCases = 1;
	crossValidation = 0;
	generationInterleave = 1000;
	reportParetoFront = true;
	reportFitness = true;
	reportFilterCount = true;
	reportGeneratedTree = true;
	reportTreeDetail = true;

    // Initialize loader
    add("MaxGeneration", maxGeneration);
    add("RandomCases", randomCases);
    add("CrossValidation", crossValidation);
	add("GenerationInterleave", generationInterleave);
    add("ReportParetoFront", reportParetoFront);
    add("ReportFitness", reportFitness);
    add("ReportFilterCount", reportFilterCount);
    add("ReportGeneratedTree", reportGeneratedTree);
	add("ReportTreeDetail", reportTreeDetail);
    add("Scenario", scenario);
}

void Garnet::Project::save(std::ostream& fout) const
{
    fout << "\nMaxGeneration        " << maxGeneration;
    fout << "\nRandomCases          " << randomCases;
    fout << "\nCrossValidation      " << crossValidation;
    fout << "\nGenerationInterleave " << generationInterleave;
    fout << "\nReportParetoFront    " << reportParetoFront;
    fout << "\nReportFitness        " << reportFitness;
    fout << "\nReportFilterCount    " << reportFilterCount;
    fout << "\nReportGeneratedTree  " << reportGeneratedTree;
    fout << "\nReportTreeDetail     " << reportTreeDetail;
	for_each(scenario.begin(), scenario.end(), [&fout](const String& task) {
		fout << "\nScenario             " << task;
	});
    fout << std::endl;
}
