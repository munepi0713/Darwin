
#include <algorithm>
#include <numeric>
#include <iomanip>

#include <boost/format.hpp>

#include <Common/InlineSprintf.h>
#include <Common/Synchronize.h>
#include <Common/GetTime.h>
#include <Common/PrintRange.h>

#include <Garnet/Garnet.h>
#include <Garnet/ThreadPool.h>
#include <Garnet/Affinity.h>
#include <Garnet/Dictionary.h>
#include <Garnet/RandomGenerator.h>
#include <Garnet/TreeKernelCollins.h>

#include <Garnet/UnifiedFitnessStrategy.h>

namespace {

class EvaluateTaskJob : public Job
{
public :
	EvaluateTaskJob(
			std::vector<PicturePerfect::PicturePerfectEnginePtr>& ppe, 
			const PicturePerfect::ExecutablePtr executable,
			const PicturePerfect::ArchivePtr source,
			PicturePerfect::ArchivePtr result)
			: _ppe(ppe), _executable(executable), _source(source), _result(result) {}
	EvaluateTaskJob(const EvaluateTaskJob& x)
			: _ppe(x._ppe), _executable(x._executable), _source(x._source), _result(x._result) {}
	~EvaluateTaskJob() {}

    virtual void run() {
		int threadId = getThreadId();
		assert(threadId != -1);

		_ppe[threadId]->execute(_executable, _source, _result);
		std::cout << ".";
	}

private :
	std::vector<PicturePerfect::PicturePerfectEnginePtr> _ppe;
	const PicturePerfect::ExecutablePtr     _executable;
	const PicturePerfect::ArchivePtr        _source;
	PicturePerfect::ArchivePtr              _result;
};

}

//---------------------------------------------------------
//---------------------------------------------------------
#if 0
static
int  compareTrees(const std::string& a, const std::string b)
{
	std::string ida, idb;

	ida = TreeManipulator::getIdentifier(a);
	idb = TreeManipulator::getIdentifier(b);
	int diff = ida.compare(idb);
	if ( diff != 0 ) return diff;
	else
	{
		TreeManipulator::TreePosList suba, subb;
		suba = TreeManipulator::getSubTrees(a);
		subb = TreeManipulator::getSubTrees(b);
		
		for (unsigned int i = 0; i < suba.size(); i++) // suba and subb must have same number of children. (because they are same id)
		{
			diff = compareTrees(suba[i].second, subb[i].second);
			if ( diff != 0 ) return diff;
		}
	}
	return 0;
}
#endif

static
bool  compareIndividualsByFitness(const Garnet::Individual& a, const Garnet::Individual& b)
{
	if ( a.fitness > b.fitness ) return true;
	else return false;
}

static
double calcCorrelation(const std::vector<double>& a, const std::vector<double>& b)
{
    int n = static_cast<int>(a.size());
    if ( a.size() != b.size() ) throw "calcCorreleation: Inputs must be same lengths.";
    if ( n == 1 ) return 0;

    double sa = 0;
    double sb = 0;
    for (int i = 0; i < n; i++) {
        sa += a[i];
        sb += b[i];
        //std::cout << inline_sprintf("corr: %.f %.f", a[i], b[i]) << std::endl;
    }
    if ( sa == 0 || sb == 0 ) return 0;
    sa /= n;
    sb /= n;

    double saa = 0;
    double sbb = 0;
    double sab = 0;
    for (int i = 0; i < n; i++) {
        double da, db;
        da = a[i] - sa;
        db = b[i] - sb;
        saa += da * da;
        sbb += db * db;
        sab += da * db;
    }
    if ( saa == 0 || sbb == 0 ) return 0;
    saa = sqrt(saa / (n - 1));
    sbb = sqrt(sbb / (n - 1));
    sab /= (n - 1) * saa * sbb;

    return sab;
}

//---------------------------------------------------------
//---------------------------------------------------------
//

//---------------------------------------------------------
//---------------------------------------------------------
Garnet::UnifiedFitnessStrategy::Evaluator::Evaluator() 
{
	_threadPool.reset(ThreadPool::createInstance(8));

}

void Garnet::UnifiedFitnessStrategy::Evaluator::execute(UniversalEngine* engine, IndividualList& individuals) const 
{
	using namespace Garnet;
	using namespace PicturePerfect;

	const Configuration& conf = engine->getConfiguration();

	const size_t Ni = conf.populationSize;
	const size_t Nt = conf.tasks.size();
	const size_t Nr = conf.evaluator.size();
	const size_t Nc = conf.numChromosomes;
	const size_t Nf = conf.fitness.size();
	const size_t Ns = conf.tasks.front().images.size();
	const size_t Ncpu = conf.numMaxCPUs;

	std::map<uint32_t, StopWatch> watches;

	#define WATCH_DO(index, statement) \
		watches[(index)].reset();  \
		statement;                 \
		watches[(index)].stop()

	// 1. Evaluate all trees for each task.	
	//    Results will be stored for each task separetely,
	//    because it is convenient to reuse an executable.
	std::vector<Tree> trees;
	ExecutablePtr executable;
	std::vector<double> fitness;

	// 1. Evaluate trees.
	// 1.1 Gather trees.
	WATCH_DO(11, 
		for (auto i = 0u; i < Ni; i++) {
			for (auto c = 0u; c < Nc; c++) {
				trees.push_back(Tree((boost::format("(Sink%u%s)") % (Ns + i * Nc + c) % individuals[i].chromosome[c].getSExpression()).str()));
			}
		}
	);
	std::cout << "1";

	// 1.2 Compile tree.
	//     Executable is saved for later reference.
	WATCH_DO(12, compileIVTrees(trees, executable, engine->getPPE()));
	engine->setTreeExecutable(executable);
	std::cout << "1";

	// 1.3 Execute script.
	//     Apply a common executable to archives associated with tasks.
	if ( conf.numThreads <= 1 ) {
		// Single-thread version.
		WATCH_DO(13, for (auto t = 0u; t < Nt; t++) {
			engine->getPPE()->execute(
					executable,
					engine->getSourceArchive(t),
					engine->getRawResultArchive(t));
			std::cout << ".";
		});
	}
	else {
		// Multi-thread version.
		WATCH_DO(13, {
			JobQueue jobQueue;
			for (auto t = 0u; t < Nt; t++) {
				Job* job = new EvaluateTaskJob(
						engine->getPPEList(),
						executable,
						engine->getSourceArchive(t),
						engine->getRawResultArchive(t));
				jobQueue.push(job);
			}
			_threadPool->start(jobQueue);
			_threadPool->wait();
		});
	}
	if ( isTrace() ) {
		for (auto t = 0u; t < Nt; t++) {
			for (auto s = 0u; s < Ns; s++) {
				std::string fn = (boost::format("source_t%u_s%u.png") % t % s).str();
				engine->getSourceArchive(t)->saveImage(ADDR_I3B + s, fn);
			}
		}

		for (auto t = 0u; t < Nt; t++) {
			for (auto i = 0u; i < Ni; i++) {
				for (auto c = 0u; c < Nc; c++) {
					std::string fn = (boost::format("tree_t%u_i%u_c%c.png") % t % i % c).str();
					engine->getSourceArchive(t)->saveImage(ADDR_I1B + Ns + i * Nc + c, fn);
				}
			}
		}
	}
	std::cout << "1";

	// 2. Evaluate evaluators.
	if ( conf.numThreads <= 1 ) {
		WATCH_DO(20, for (auto t = 0u; t < Nt; t++) {
			engine->getPPE()->execute(
					engine->getEvaluatorExecutable(),
					engine->getRawResultArchive(t),
					engine->getRawFitnessArchive(t));
		});
	}
	else {
		// Multi-thread version.
		WATCH_DO(20, {
			JobQueue jobQueue;
			for (auto t = 0u; t < Nt; t++) {
				Job* job = new EvaluateTaskJob(
						engine->getPPEList(),
						engine->getEvaluatorExecutable(),
						engine->getRawResultArchive(t),
						engine->getRawFitnessArchive(t));
				jobQueue.push(job);
			}
			_threadPool->start(jobQueue);
			_threadPool->wait();
		});
	}
	WATCH_DO(21, for (auto t = 0u; t < Nt; t++) {
		for (auto i = 0u; i < Ni; i++) {
			std::vector<double> values;
			for (auto r = 0u; r < Nr; r++) {
				values.push_back(engine->getRawFitnessArchive(t)->getValue(ADDR_V1D + i * Nr + r));
			}
		}
	});
	if ( isTrace() ) {
		std::cout << "----- ----- ----- -----\n";
		std::cout << "Raw Fitness: row=(i,t), col=(r)\n";
		for (auto i = 0u; i < Ni; i++) {
			std::cout << boost::format("Individual %u\n") % i;
			for (auto t = 0u; t < Nt; t++) {
				ArchivePtr rawFitArchive = engine->getRawFitnessArchive(t);
				std::vector<double> values;
				for (auto r = 0u; r < Nr; r++) {
					values.push_back(rawFitArchive->getValue(ADDR_V1D + i * Nr + r));

					std::string fn = (boost::format("rawfit_i%u_t%u_r%u.png") % i % t % r).str();
					rawFitArchive->saveImage(ADDR_I1B + i * Nr + r, fn);
				}
				std::cout << boost::format("  Task%u: (") % t
					      << print_range(values.begin(), values.end(), ", ")
				          << ")\n";
			}
		}
		std::cout << std::flush;
	}
	std::cout << "2";

	// 3. Preprocess and relocate task results for archives
	std::vector<uint32_t> distCollins(Ni * Ni);
	WATCH_DO(31, {
		Garnet::CollinsKernel<Tree::GraphType> ck;
		for (auto i1 = 0u; i1 < Ni; i1++) {
			for (auto i2 = i1 + 1; i2 < Ni; i2++) {
				uint32_t value = 0;
				for (auto c = 0u; c < Nc; c++) {
					value += ck.execute(
							individuals[i1].chromosome[c].getGraph(),
							individuals[i2].chromosome[c].getGraph());
				}
				distCollins[i1 * Ni + i2] = value;
				distCollins[i2 * Ni + i1] = value;
			}
		}
	});
	std::cout << "3";

	WATCH_DO(30, for (auto i = 0u; i < Ni; i++) {
		double value;
		ArchivePtr fvectorArchive = engine->getFitnessSourceArchive(i);

		// RawFit
		for (auto r = 0u; r < Nr; r++) {
			Vector<double>& rawFitness = fvectorArchive->arrayAt(ADDR_A1D + r);
			rawFitness.clear();
			for (auto t = 0u; t < Nt; t++) {
				ArchivePtr rawFitArchive = engine->getRawFitnessArchive(t);
				rawFitness.push_back(rawFitArchive->getValue(ADDR_V1D + i * Nr + r));
			}
			fvectorArchive->setValue(ADDR_V1D + r, rawFitness.average());
		}

		// Nodes
		value = 0;
		for (auto c = 0u; c < Nc; c++) {
			value += individuals[i].chromosome[c].getNumVertices();
		}
		fvectorArchive->setValue(ADDR_V1D + Nr + 0, value);

		// ExecTime
		value = 0;
		for (auto c = 0u; c < Nc; c++) {
			const Tree& tree = individuals[i].chromosome[c];
			for (auto vrange = boost::vertices(tree.getGraph()); vrange.first != vrange.second; vrange.first++) {
				Tree::GraphType  g = tree.getGraph();
				Tree::VertexDesc u = *vrange.first;
				if ( !Text::startsWith(g[u].label, "Src") && !Text::startsWith(g[u].label, "Sink") ) {
					const IVInstruction& inst = Dictionary::findByIVName(g[u].label);
					uint16_t opcode;
					opcode = engine->getPPE()->getOpCode(inst.pp[0].label);
					if ( opcode != 0xffffu ) value += engine->getTreeExecutable()->statistics[opcode].getAverage();
					opcode = engine->getPPE()->getOpCode(inst.pp[1].label);
					if ( opcode != 0xffffu ) value += engine->getTreeExecutable()->statistics[opcode].getAverage();
				}
			}
		}
		fvectorArchive->setValue(ADDR_V1D + Nr + 1, value);

		// DistCollins
		value = 0;
		for (auto i2 = 0u; i2 < Ni; i2++) {
			if ( distCollins[i * Ni + i2] > value ) value = distCollins[i * Ni + i2];
		}
		fvectorArchive->setValue(ADDR_V1D + Nr + 2, value);

		// DistKashima
		value = 0;
		fvectorArchive->setValue(ADDR_V1D + Nr + 3, value);
	});
	std::cout << "3";

	// 4. Calculate fintess vector elements.
	if ( true ) {//if ( conf.numThreads <= 1 ) {
		WATCH_DO(40, for (auto i = 0u; i < Ni; i++) {
			engine->getPPE()->execute(
					engine->getFitnessExecutable(),
					engine->getFitnessSourceArchive(i),
					engine->getFitnessVectorArchive(i));
		});
	}
	else {
		// Multi-thread version.
		WATCH_DO(40, {
			JobQueue jobQueue;
			for (auto i = 0u; i < Ni; i++) {
				Job* job = new EvaluateTaskJob(
						engine->getPPEList(),
						engine->getFitnessExecutable(),
						engine->getFitnessSourceArchive(i),
						engine->getFitnessVectorArchive(i));
				jobQueue.push(job);
			}
			_threadPool->start(jobQueue);
			_threadPool->wait();
		});
	}
	std::cout << "4";

	// 5. Construct fitness vectors.
	WATCH_DO(50, for (auto i = 0u; i < Ni; i++) {
		PicturePerfect::ArchivePtr archive = engine->getFitnessVectorArchive(i);
		individuals[i].fitness.resize(Nf);
		for (auto f = 0u; f < Nf; f++) {
			individuals[i].fitness[f] = archive->getValue(ADDR_V1D + f);
		}
	});
	std::cout << "5";

	// Show information.
	if ( isTrace() ) {
		std::cout << boost::format("[%s] Gather Trees               %.6fs\n") % __FUNCTION__ % watches[11].get();
		std::cout << boost::format("[%s] Compile Trees              %.6fs\n") % __FUNCTION__ % watches[12].get();
		std::cout << boost::format("[%s] Evaluate Trees             %.6fs\n") % __FUNCTION__ % watches[13].get();
		std::cout << boost::format("[%s] Evaluate Raw Fitness       %.6fs\n") % __FUNCTION__ % watches[20].get();
		std::cout << boost::format("[%s] Gather Raw Fitness         %.6fs\n") % __FUNCTION__ % watches[30].get();
		std::cout << boost::format("[%s]   Collins Distance         %.6fs\n") % __FUNCTION__ % watches[31].get();
		std::cout << boost::format("[%s] Calculate Fitness Vectors  %.6fs\n") % __FUNCTION__ % watches[40].get();
		std::cout << boost::format("[%s] Construct Fitness Vectors  %.6fs\n") % __FUNCTION__ % watches[50].get();
		std::cout << std::flush;
	}
}

//---------------------------------------------------------
//---------------------------------------------------------

Garnet::UnifiedFitnessStrategy::UnifyingSorter::UnifyingSorter(const std::string& type) 
{
	// Parse Scoring Method
    if ( type.compare("Ave") == 0 ) _scoringMethod = SCORE_AVERAGE;
    else if ( type.compare("Average") == 0 ) _scoringMethod = SCORE_AVERAGE;
    else if ( type.compare("Max") == 0 ) _scoringMethod = SCORE_MAXIMUM;
    else if ( type.compare("Maximum") == 0 ) _scoringMethod = SCORE_MAXIMUM;
    else if ( type.compare("Min") == 0 ) _scoringMethod = SCORE_MINIMUM;
    else if ( type.compare("Minimum") == 0 ) _scoringMethod = SCORE_MINIMUM;
    else if ( type.compare("Med") == 0 ) _scoringMethod = SCORE_MEDIAN;
    else if ( type.compare("Median") == 0 ) _scoringMethod = SCORE_MEDIAN;
    else if ( type.compare("LowerSigma") == 0 ) _scoringMethod = SCORE_LOWERSIGMA;
    else if ( type.compare("TFAverage") == 0 ) _scoringMethod = SCORE_TF_AVERAGE;
    else if ( type.compare("TFMinimum") == 0 ) _scoringMethod = SCORE_TF_MINIMUM;
    else if ( type.compare("TFCorrectRate") == 0 ) _scoringMethod = SCORE_TF_CORRECT_RATE;
    else if ( type.compare("CorrelationTo1") == 0 ) _scoringMethod = SCORE_CORRELATION_TO_1;
    else if ( type.compare("Cluster2") == 0 ) _scoringMethod = SCORE_CLUSTER_2;
    else throw "Unknown Scoring Method.";
}

void Garnet::UnifiedFitnessStrategy::UnifyingSorter::execute(const UniversalEngine* engine, IndividualList& individuals) const 
{
	using namespace Garnet;
	using namespace PicturePerfect;


#if 1
    // Sort in descending order of fintess[0].
	std::stable_sort(
		individuals.begin(), individuals.end(), 
		[](const Individual& x, const Individual& y) -> bool {
			return x.fitness[0] > y.fitness[0];
	});
#else
	// 5.2 Calculate 
    //   A final fitness for an individual is calculated by averaging
    //   (or max/min of) chromosome's scores in objective space.
	for (size_t n = 0; n < individuals.size(); n++) {
		Individual& individual = individuals[n];
		const Garnet::ChromosomeList& chromosomes = individual.chromosome;

		// Calculate standardized fitness.
		for (size_t nc = 0; nc < chromosomes.size(); nc++) {
			const Garnet::Chromosome& chromosome = chromosomes[nc];
			const std::vector<double>& score = chromosome.score;
			double fitness;

			switch ( _scoringMethod ) {
			case SCORE_AVERAGE :
				fitness = std::accumulate(score.begin(), score.end(), 0.0) / score.size();
				break;
			case SCORE_MINIMUM :
				fitness = *min_element(score.begin(), score.end());
				break;
			case SCORE_MAXIMUM :
				fitness = *max_element(score.begin(), score.end());
				break;
			case SCORE_MEDIAN :
				{
					std::vector<double> temp = score;
					std::sort(temp.begin(), temp.end());
					fitness = (temp.size() % 2) == 0 ? (temp[temp.size()/2-1] + temp[temp.size()/2]) / 2 : temp[temp.size()/2];
				}
				break;
			case SCORE_LOWERSIGMA :
				{
					// (1/n) * sqrt(Sum(f^2) - 2 * Sum(f) * g + g * g)
					double sum = std::accumulate(score.begin(), score.end(), 0.0);
					double ave = sum / score.size();
					double sqsum = 0.0;
					for (unsigned int m = 0; m < score.size(); m++) {
						sqsum += (score[m] - ave) * (score[m] - ave);
					}
					double sigma = sqrt(sqsum / score.size());
					fitness = std::max(0.0, ave - sigma);
					//individuals[n].fitness = ave - sigma;
					if ( engine->isTrace() ) {
						engine->getTraceOut()
								<< inline_sprintf("# Individual %d", n) << std::endl
								<< inline_sprintf("#   Average %.16f", ave) << std::endl
								<< inline_sprintf("#   SqSum %.16f", sqsum) << std::endl
								<< inline_sprintf("#   Sigma %.16f", sigma) << std::endl;
					}
				}
				break;
			case SCORE_TF_AVERAGE :
				fitness = 0.0;
				for (unsigned int m = 0; m < score.size(); m++) {
					fitness += std::max(0.0, (score[m] - 0.5) * 2.0);
				}
				fitness /= score.size();
				break;
			case SCORE_TF_MINIMUM :
				fitness = 1.0;
				for (unsigned int m = 0; m < score.size(); m++) {
					double temp = std::max(0.0, (score[m] - 0.5) * 2.0);
					if ( temp < fitness ) fitness = temp;
				}
				break;
			case SCORE_TF_CORRECT_RATE :
				{
					int count = 0;
					double THRESHOLD = 0.7;
					for (unsigned int m = 0; m < score.size(); m++) {
						if ( score[m] >= THRESHOLD ) count++;
					}
					fitness = (double)count / score.size();
				}
				break;
			case SCORE_CORRELATION_TO_1 :
				{
					std::vector<double> a;
					std::vector<double> b;
					for (unsigned int t = 0; t < engine->getConfiguration().tasks.size(); t++) {
						a.push_back(score[t]);
						b.push_back(engine->getConfiguration().tasks[t].properties[1]); // Properties of source 1.
					}
					fitness = calcCorrelation(a, b);
					if ( fitness < 0 ) fitness = 0;
				}
				break;
			case SCORE_CLUSTER_2 :
				{
					// Wider gap between clusters, higher score.
					//   FinalScore = (Min of cluster A) - (Max of cluster B),
					//   where A and B are V of Src1. A > B.
					//   Raw fitness must be between 0.0 and 1.0.
					//   Return zero if A < B.
					std::pair<double, double> value[2]; // (V1,V2)[X]=(min fitness, max fitness) of cluster X.
					value[0].first  = std::numeric_limits<double>::max();
					value[0].second = std::numeric_limits<double>::min();
					value[1].first  = std::numeric_limits<double>::max();
					value[1].second = std::numeric_limits<double>::min();
					for (unsigned int t = 0; t < engine->getConfiguration().tasks.size(); t++) {
						//std::cout << "Add Value " << score[t] << " to Cluster " << _conf.tasks[t].properties[1] << std::endl;
						double  fitness = score[t];
						double& minimum = value[int(engine->getConfiguration().tasks[t].properties[1])].first;
						double& maximum = value[int(engine->getConfiguration().tasks[t].properties[1])].second;
						if ( fitness < minimum ) minimum = fitness;
						if ( fitness > maximum ) maximum = fitness;
					}
					fitness = value[1].first - value[0].second;
					if ( fitness <= 0 ) fitness = 0;
				}
				break;
			}
			// Apply penalty by the number of filters.
			if ( engine->getConfiguration().filterCountPenaltyWeight > 0 ) {
				// subtract 1 to make a penalty 0 if the length of filter is 1.
				fitness *= exp(-1 
						* engine->getConfiguration().filterCountPenaltyWeight 
						* (chromosome.getNumVertices() - 1));
			}

			// Store.
			individual.fitness[nc] = fitness;
		}
        // Log fitness.
		if ( engine->isTrace() ) {
            std::ofstream fout(inline_sprintf("%s/%s.%05u.%05u.individual.txt",  _conf.dir.c_str(), _conf.title.c_str(), getGeneration(), n).c_str(), std::ios::app);
            fout << inline_sprintf("# Fitness %.16f (Before modified %.16f)", individuals[n].fitness, originalFitness) << std::endl;
			fout.close();
		}
    }

    // Sort
	std::stable_sort(individuals.begin(), individuals.end(), compareIndividualsByFitness);
#endif
}

std::pair<Garnet::IndividualList, Garnet::IndividualList> Garnet::UnifiedFitnessStrategy::RouletteSelector::execute(const UniversalEngine* engine, const IndividualList& individuals) const {   
	std::pair<Garnet::IndividualList, Garnet::IndividualList> ret;
	ret.second = individuals;
	return ret;
}

bool Garnet::UnifiedFitnessStrategy::Terminator::isFinished(const UniversalEngine* engine, const IndividualList& individuals, int generation) const {
	if ( generation >= engine->getConfiguration().maxGeneration ) {
		return true;
	}
	return false;
}

std::vector<std::pair<size_t, size_t> > Garnet::UnifiedFitnessStrategy::PairMaker::execute(const UniversalEngine* engine, IndividualList& individuals) const {
	std::vector<std::pair<size_t, size_t> > ret;
	const uint32_t Ni = engine->getConfiguration().populationSize;
	for (auto i = 0u; i < Ni; i++) {
		ret.push_back(std::make_pair(randInteger(Ni), randInteger(Ni)));
	}
	return ret;
}

Garnet::IndividualList Garnet::UnifiedFitnessStrategy::OnePointCrossover::execute(const UniversalEngine* engine, const IndividualList& individuals, const std::vector<std::pair<size_t, size_t> > parents) const {
	return individuals;
}

void Garnet::UnifiedFitnessStrategy::RandomMutation::execute(const UniversalEngine* engine, IndividualList& individuals) const {
}
