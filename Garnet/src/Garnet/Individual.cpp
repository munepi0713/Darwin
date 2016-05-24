#pragma warning(disable: 4819)

#include <boost/format.hpp>

#include <Common/Compiler.h>
#include <Common/GetTime.h>
#include <Garnet/Individual.h>

void Garnet::IndividualList::load(const strings& tokens)
{
	ConfigLoader::NumArgsException::greaterEqOrThrow(3, tokens.size(), tokens[0] + " command must have $1 argument, but $2 args specified.");

	// Load
	unsigned n = strtoul(tokens[1].c_str(), 0, 0);
	while (this->size() <= n) {
		this->push_back(Individual());
	}
	Individual& ind = this->at(n);
	if (Text::equals(tokens[0], "Rank")) {
		// Format: [1]ind#, [2]Rank
		ind.rank = strtol(tokens[2].c_str(), 0, 0);
	}
	else if (Text::equals(tokens[0], "Chromosomes")) {
		// Format: [1]ind#, [2]chromosome#, [3]checksum, [4]chromosome
		uint32_t c = stoi(tokens[2]);
		if (ind.chromosome.size() < (c + 1)) {
			ind.chromosome.resize(c + 1);
		}
		ind.chromosome[c].resetGraph(tokens[4]);
	}
	else if (Text::equals(tokens[0], "FitnessV")) {
		// Format: [1]ind#, [2]Fitness[0], ...
		for (unsigned int i = 2; i < tokens.size(); i++) {
			union {
				double    d;
				uint64_t  u;
			} t;
			t.u = STRTOULL(tokens[i].c_str(), 0, 0);
			ind.fitness.push_back(t.d);
		}
	}
	else if (Text::equals(tokens[0], "RawFitnessV")) {
		// Format: [1]ind#, [2]Average, [3]RawFitness[0], ... 
		PicturePerfect::Vector<double> v;
		for (unsigned int i = 3; i < tokens.size(); i++) {
			union {
				double    d;
				uint64_t  u;
			} t;
			t.u = STRTOULL(tokens[i].c_str(), 0, 0);
			v.push_back(t.d);
		}
		ind.rawFitness.push_back(v);
	}
}

#if 0
Garnet::Individual Garnet::Individual::create(const Garnet::TreeBuilderPtr builder, uint32_t numChromosomes, uint32_t maxInitialDepth, uint32_t minInitialSize)
{
	Garnet::Individual ind;
	for (int c = 0; c < numChromosomes; c++) {
		ind.chromosome.push_back(builder->generateBySize(maxInitialDepth, minInitialSize));
	}
	ind.parents[0]  = std::numeric_limits<Garnet::Individual::history_t>::max();
	ind.parents[1]  = std::numeric_limits<Garnet::Individual::history_t>::max();
	ind.preRank     = std::numeric_limits<Garnet::Individual::history_t>::max();
	return ind;
}
#endif

Garnet::Individual Garnet::Individual::archive(const Garnet::Individual& x) {
	Garnet::Individual ret = x;
	ret.preRank = ret.rank;
	ret.parents[0] = std::numeric_limits<Garnet::Individual::history_t>::max();
	ret.parents[1] = std::numeric_limits<Garnet::Individual::history_t>::max();
	std::for_each(ret.chromosome.begin(), ret.chromosome.end(), [] (Chromosome& c) {
		c.cross[0] = std::numeric_limits<Garnet::Individual::history_t>::max();
		c.cross[1] = std::numeric_limits<Garnet::Individual::history_t>::max();
		c.mutate.clear();
	});
	return ret;
}
