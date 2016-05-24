
#include <Garnet/IVTreeStrategyClasses.h>

#include <algorithm>
#include <numeric>
#include <iomanip>
#include <array>

#include <boost/format.hpp>

#include <Common/Synchronize.h>
#include <Common/GetTime.h>
#include <Common/PrintRange.h>

#include <Garnet/Garnet.h>
#include <Garnet/TraceLevel.h>
#include <Garnet/ThreadManager.h>
#include <Garnet/Affinity.h>
#include <Garnet/Dictionary.h>
#include <Garnet/RandomGenerator.h>
#include <Garnet/TreeKernelCollins.h>
#include <Garnet/TreeKernelKashima.h>
#include <Garnet/IVGraphProcessor.h>

using namespace PicturePerfect;

namespace {

#if 0
class DistCollinsJob : public Job
{
public :
	DistCollinsJob(
			const Garnet::IndividualList& individuals, uint32_t first, uint32_t last, std::vector<uint32_t>& distCollins)
			: _individuals(&individuals), _first(first), _last(last), _distCollins(&distCollins) {}
	DistCollinsJob(const DistCollinsJob& x)
			: _individuals(x._individuals), _first(x._first), _last(x._last), _distCollins(x._distCollins) {}
	~DistCollinsJob() {}

    virtual void run() {
		const uint32_t Ni = (*_individuals).size();
		const uint32_t Nc = (*_individuals)[0].chromosome.size();

		for (auto i1 = _first; i1 < _last; i1++) {
			for (auto i2 = i1 + 1; i2 < Ni; i2++) {
				uint32_t value = 0;
				for (auto c = 0u; c < Nc; c++) {
					value += _ck.execute(
							(*_individuals)[i1].chromosome[c].getGraph(),
							(*_individuals)[i2].chromosome[c].getGraph());
				}
				(*_distCollins)[i1 * Ni + i2] = value;
				(*_distCollins)[i2 * Ni + i1] = value;
			}
		}
		std::cout << ".";
	}

private :
	Garnet::CollinsKernel<Garnet::Tree::GraphType> _ck;
	uint32_t  _first;
	uint32_t  _last;
	const Garnet::IndividualList* _individuals;
	std::vector<uint32_t>* _distCollins;
};

class DistKashimaJob : public Job
{
public :
	DistKashimaJob(
			const Garnet::IndividualList& individuals, uint32_t first, uint32_t last, std::vector<uint32_t>& dist)
			: _individuals(&individuals), _first(first), _last(last), _dist(&dist) {}
	DistKashimaJob(const DistKashimaJob& x)
			: _individuals(x._individuals), _first(x._first), _last(x._last), _dist(x._dist) {}
	~DistKashimaJob() {}

    virtual void run() {
		const uint32_t Ni = (*_individuals).size();
		const uint32_t Nc = (*_individuals)[0].chromosome.size();

		for (auto i1 = _first; i1 < _last; i1++) {
			for (auto i2 = i1 + 1; i2 < Ni; i2++) {
				uint32_t value = 0;
				for (auto c = 0u; c < Nc; c++) {
					value += _k.execute(
							(*_individuals)[i1].chromosome[c].getGraph(),
							(*_individuals)[i2].chromosome[c].getGraph());
				}
				(*_dist)[i1 * Ni + i2] = value;
				(*_dist)[i2 * Ni + i1] = value;
			}
		}
		std::cout << ".";
	}

private :
	Garnet::KashimaKernel<Garnet::Tree::GraphType> _k;
	uint32_t  _first;
	uint32_t  _last;
	const Garnet::IndividualList* _individuals;
	std::vector<uint32_t>* _dist;
};
#endif

}//end of local namespace 

template <typename Watch, typename Func>
void WATCH_DO(Watch& watch, Func func) {
	watch.reset();
	func();
	watch.stop();
}

//---------------------------------------------------------
//  IVTreeEvaluator
//---------------------------------------------------------
Garnet::IVTreeStrategy::IVTreeEvaluator::IVTreeEvaluator() {
    // TODO: Initialize internal variables.
}
/**
 *
 */
bool Garnet::IVTreeStrategy::IVTreeEvaluator::initialize(const Configuration& conf)
{
	StopWatch watch;

	const size_t Nt = conf.tasks.size();
	const size_t Nu = conf.tests.size();
	const size_t Nr = conf.evaluator.size();
	const size_t Nf = conf.fitness.size();
	const size_t Ns = conf.tasks.front().images.size();
	size_t Nk = 0; // constant

	std::vector<std::string>      evalSexp;
	std::vector<std::string>      fvecSexp;
	std::vector<std::string>      constList;

	// 0. Configure graph processor.
	const std::array<const char*, 3> group = { "forest", "dag", "default" };
	const std::array<const char*, 3> toposort = { "bgl", "sarkar", "default" };
	switch (std::find(group.begin(), group.end(), Text::toLower(conf.groupingMethod)) - group.begin()) {
	case 0 : // forest
		_ivp.setInformation<IVGraphProcessor::GroupMethod>(IVGraphProcessor::GROUP_METHOD, IVGraphProcessor::GROUP_AS_FOREST);
		break;
	case 1 : // dag
		_ivp.setInformation<IVGraphProcessor::GroupMethod>(IVGraphProcessor::GROUP_METHOD, IVGraphProcessor::GROUP_AS_DAG);
		break;
	default :
		_ivp.setInformation<IVGraphProcessor::GroupMethod>(IVGraphProcessor::GROUP_METHOD, IVGraphProcessor::GROUP_DEFAULT);
	}
	_ivp.setInformation<uint32_t>(IVGraphProcessor::GROUP_SIZE, conf.numVerticesLimit);
	switch (std::find(toposort.begin(), toposort.end(), Text::toLower(conf.topologicalSortMethod)) - toposort.begin()) {
	case 0 : // normal
		_ivp.setInformation<IVGraphProcessor::TopoSortMethod>(IVGraphProcessor::TOPOSORT_METHOD, IVGraphProcessor::TOPOSORT_BGL);
		break;
	case 1 : // sarkar
		_ivp.setInformation<IVGraphProcessor::TopoSortMethod>(IVGraphProcessor::TOPOSORT_METHOD, IVGraphProcessor::TOPOSORT_SARKAR);
		break;
	default :
		_ivp.setInformation<IVGraphProcessor::TopoSortMethod>(IVGraphProcessor::TOPOSORT_METHOD, IVGraphProcessor::TOPOSORT_DEFAULT);
	}

	// Alias
	for (auto it = conf.aliases.begin(); it != conf.aliases.end(); it++) {
		_aliases[Tree(it->first).getRoot()->label] = it->second;
	}

	// 1. Constant ----------------------------------------
	// Search Const_N nodes from building blocks, evaluator def, and fitness vector defs.
	// Const_N are treated as source tuple so those are added to sources.
	auto constFinder = [](const Tree& t, std::vector<std::string>& constList) {
		for (auto k = 0u; k != t.getNumVertices(); k++) {
			auto u = t.getVertex(k);
			if (Text::startsWith(u->label, "Const_")) {
				if (std::find(constList.begin(), constList.end(), u->label) == constList.end()) {
					constList.push_back(u->label);
				}
			}
		}
	};

	// 1.1 Const_N in building blocks.
	for (auto it = conf.units.cbegin(); it != conf.units.cend(); it++) {
		for (auto jt = it->second.cbegin(); jt != it->second.cend(); jt++) {
			constFinder(resolveAliases(Tree(*jt)), constList);
		}
	}
	// 1.2 Const_N in evaluator definitions.
	for (auto r = 0u; r < Nr; r++) {
		constFinder(resolveAliases(Tree(conf.evaluator[r])), constList);
	}
	// 1.3 Const_N in fitness vector definitions.
	for (auto f = 0u; f < Nf; f++) {
		constFinder(resolveAliases(Tree(conf.fitness[f])), constList);
	}
	// 1.4 Remove duplications.
	std::sort(constList.begin(), constList.end());
	std::unique(constList.begin(), constList.end());
	Nk = constList.size();


	// 2 Add aliases.
	// 2.1 (Const_N) to (_LoadColorN) (Stage A, B, C)
	for (auto k = 0u; k < Nk; k++) {
		_aliases[(boost::format("%s") % constList[k]).str()] = (boost::format("(_LoadColor%u)") % k).str();
	}
	// 2.2 (SrcN) to (_LoadColorN) (Stage A, B)
	for (auto s = 0u; s < Ns; s++) {
		_aliases[(boost::format("Src%u") % s).str()] = (boost::format("(_LoadColor%u)") % (Nk + s)).str();
	}
	// 2.3 (RawFitN) to (_LoadN). (Stage C)
	for (auto r = 0u; r < Nr; r++) {
		_aliases[(boost::format("RawFit%u") % r).str()] = (boost::format("(_Load%u)") % (Nk + r)).str();
	}
	// 2.4 FVector operators. (Stage C)
	_aliases["Nodes"] = (boost::format("(_Load%u)") % (Nk + Nr + 0)).str();
	_aliases["ExecTime"] = (boost::format("(_Load%u)") % (Nk + Nr + 1)).str();
	_aliases["DistCollins"] = (boost::format("(_Load%u)") % (Nk + Nr + 2)).str();
	_aliases["DistKashima"] = (boost::format("(_Load%u)") % (Nk + Nr + 3)).str();
	_aliases["DistNSGA2"] = (boost::format("(_Load%u)") % (Nk + Nr + 4)).str();

	// 3. Compile evaluator definitions.
	//  NOTE: Evaluator will be compiled each generation.

	// 4. Compile fitness vector definitions.
	_enableExecTime = false;
	_enableDistCollins = false;
	_enableDistKashima = false;
	_enableDistNSGA2 = false;
	// Prepare trees.
	std::vector<Garnet::Tree> fvectors;
	for (auto f = 0u; f < Nf; f++) {
		// Expand aliases.
		std::string sexp = conf.fitness[f];

		// Append the root node.
		sexp = (boost::format("(_VAStore%u%s)") % f % sexp).str();

		// Check special operators.
		if (sexp.find("(ExecTime)") != std::string::npos) _enableExecTime = true;
		if (sexp.find("(DistCollins)") != std::string::npos) _enableDistCollins = true;
		if (sexp.find("(DistKashima)") != std::string::npos) _enableDistKashima = true;
		if (sexp.find("(DistNSGA2)") != std::string::npos) _enableDistNSGA2 = true;

		// Convert S-expression to Tree and store it.
		fvectors.push_back(resolveAliases(Tree(sexp)));
	}
	if (isProcessTrace()) {
		for (auto f = 0u; f < Nf; f++) {
			std::cout << boost::format("\n[%s] Fitness[%u]: %s") % __FUNCTION__ % f % fvectors[f].getSExpression();
		}
	}
	// Generate executable.
	watch.reset();
	_fitnessExecutables = _ivp.compile(fvectors);
	watch.stop();

	// 5. Create archive and fill it with data.
	_archive = Garnet::IVPairArchive::createInstance(cv::Size(conf.imageWidth, conf.imageHeight));

	// Load constants.
	for (auto k = 0u; k < Nk; k++) {
		_archive->pairAt(0x000000u + k).loadColor(cv::Scalar(0, 0, 0), stod(constList[k].substr(6)), PicturePerfect::Vector<double>());
	}
	// Load task sources.
	for (auto t = 0u; t < Nt; t++) {
		const Task& task = conf.tasks[t];
		for (auto s = 0u; s < Ns; s++) {
			_archive->pairAt(0x004000u + t * Ns + s).loadColor(task.images[s], task.properties[s], PicturePerfect::Vector<double>());
		}
	}
	// Load test sources.
	for (auto u = 0u; u < Nu; u++) {
		const Task& task = conf.tests[u];
		for (auto s = 0u; s < Ns; s++) {
			_archive->pairAt(0x004000u + (Nt + u) * Ns + s).loadColor(task.images[s], task.properties[s], PicturePerfect::Vector<double>());
		}
	}

	if (isStorageTrace()) {
		for (auto k = 0u; k < Nk; k++) {
			std::string fn = (boost::format("%sconst_%u.png") % conf.dir % k).str();
			_archive->pairAt(0x000000u + k).saveColorImage(fn);
		}
		for (auto t = 0u; t < Nt; t++) {
			for (auto s = 0u; s < Ns; s++) {
				std::string fn = (boost::format("%ssrc_t%u_s%u.png") % conf.dir % t % s).str();
				_archive->pairAt(0x004000u + t * Ns + s).saveColorImage(fn);
			}
		}
	}

	// 6. Save constants.
	_Nc = conf.numChromosomes;
	_Nt = conf.tasks.size();
	_Nu = conf.tests.size();
	_Nr = conf.evaluator.size();
	_Nf = conf.fitness.size();
	_Ns = conf.tasks.front().images.size();
	_Nk = Nk;
	_Na = 5;

	// Show information.
	if (Garnet::isLocationTrace()) {
		std::cout << boost::format("\n[%s] Compile Fitness V   %.6fs") % __FUNCTION__ % watch.get();
		std::cout << boost::format("\n[%s] Number of Operators %.6u") % __FUNCTION__ % _fitnessExecutables[0]->getNumberOfOperators();
		std::cout << boost::format("\n[%s] Number of Data      %.6u") % __FUNCTION__ % _fitnessExecutables[0]->getNumberOfData();
		std::cout << std::flush;
	}

    return true;
}

/**
*  Size
*    0 < Nindividuals.  (Do NOT assume it as same as the population size.)
*/
void Garnet::IVTreeStrategy::IVTreeEvaluator::execute(GPEngine* engine, IndividualList& individuals)
{
    uint32_t Nt;
    if (boost::any_cast<bool>(engine->getParameter(GPEngine::EVALUATE_TESTS))) {
        Nt = _Nt + _Nu;
    }
    else {
        Nt = _Nt;
    }

    // 1. Evaluate tree.
    evaluateTree(engine, individuals, Nt);

    // 2. Evaluate raw fitness.
    evaluateRawFitness(engine, individuals, Nt);

    // 3. Evaluate fitness vector.
    evaluateFitnessVector(engine, individuals);

    GTRACE("Evaluation");
    GTRACE_F("  %u registers.", _archive->getArchive()->getNumSlots());
}


Garnet::IVTreeStrategy::IVTreeEvaluator::~IVTreeEvaluator()
{
#if 1
#else
	// Save statistics.
	std::ofstream fout("PicturePerfectEngineStatistics.txt");
	{
		fout << "OpCode,OpName";
		for (auto i = 0u; i < _ppe.size(); i++) {
			fout << ",Call,AccTime[s]";
		}
	}
	for (uint16_t op = 0; op <= _ppe[0]->getMaxOpCode(); op++) {
		fout << "\n" << op << "," << _ppe[0]->getOpName(op);
		for (auto i = 0u; i < _ppe.size(); i++) {
			const Statistics& stats = _ppe[i]->getStatistics(op);
			fout << "," << stats.getCount() << "," << stats.getAverage();
		}
	}
	fout.close();
#endif
}

Garnet::Tree& Garnet::IVTreeStrategy::IVTreeEvaluator::resolveAliases(Garnet::Tree& tree)
{
	return Garnet::resolveAliases(tree, _aliases.cbegin(), _aliases.cend());
}

void Garnet::IVTreeStrategy::IVTreeEvaluator::evaluateTree(GPEngine* engine, IndividualList& individuals, uint32_t Nt)
{
	using namespace Garnet;
	using namespace PicturePerfect;

	const size_t Ni = individuals.size();

	std::vector<Tree> trees;

	// 1. Gather trees.
	for (auto i = 0u; i < Ni; i++) {
		for (auto c = 0u; c < _Nc; c++) {
			aliases[(boost::format("_Tree%u") % (i * _Nc + c)).str()] = individuals[i].chromosome[c];
			aliases[(boost::format("Tree%u") % c).str()] = (boost::format("(_Tree%u)") % (i * _Nc + c)).str();
			aliases[(boost::format("_RawTree%u") % (i * _Nc + c)).str()] = (boost::format("(_IVStore%u(Tree%u))") % (i * _Nc + c) % c).str();
			trees.push_back((boost::format("_RawTree%u") % (i * _Nc + c)).str());
		}

		for (auto r = 0u; r < _Nr; r++) {
			std::string rawFit = (boost::format("_RawFit%u") % (i * _Nr + r)).str();
			aliases[rawFit] = (boost::format("(_IVStore%u(Evaluator%u))") % (i * _Nr + r) % r).str();
			trees.push_back(rawFit);
		}
	}

	// 2. Trees to Graphs.
	graphs = convertTreesToGraphs(trees);

	std::map<uint32_t, StopWatch> watches;

	// 1. Evaluate all trees for each task.	
	//    Results will be stored for each task separetely,
	//    because it is convenient to reuse an executable.
	std::vector<ExecutablePtr> executables;
	std::vector<double> fitness;

	// 1. Resolve alias.
	std::cout << "1";
	WATCH_DO(watches[11], [&]() {
		for (auto c = 0u; c < _Nc; c++) {
			for (auto i = 0u; i < Ni; i++) {
				// 1. Append a sink node on the top of the root node.
				//    The sink node will store the result of this tree to the storage.
				std::string sexp = (boost::format("(_IVStore%u%s)") % (i * _Nc + c) % individuals[i].chromosome[c].getSExpression()).str();

				// 2. Resolve aliases.
				Tree tree(sexp);
				resolveAliases(tree);
				if (isProcessTrace()) {
					std::cout << boost::format("\n[%s] Tree%u[%u]: %s") % __FUNCTION__ % i % c % tree.getSExpression();
				}

				// 3. Add the tree.
				trees.push_back(tree);
			}
		}
	});

	// 2. Compile tree.
	//     Executable is saved for later reference.
	std::cout << "2";
	executables = _ivp.compile(trees, engine->getThreadPool());

	// [Log] Save graph information to individuals.
	auto lastGraphInfo = _ivp.getLastGraphInfo(); // Information per graph.
	for (auto i = 0u; i < lastGraphInfo.size(); i++) {
		GTRACE_F("Group %u:", i);
		GTRACE_F("  %u IVTree vertices.", lastGraphInfo[i].getNumVertices());
		GTRACE_F("  %u IVGraph vertices. (%.3f %%)", boost::num_vertices(lastGraphInfo[i].iv) % (100.0 * boost::num_vertices(lastGraphInfo[i].iv) / lastGraphInfo[i].getNumVertices()));
		GTRACE_F("  %u PPEGraph vertices. (%.3f %%)", boost::num_vertices(lastGraphInfo[i].pg) % (50.0 * boost::num_vertices(lastGraphInfo[i].pg) / lastGraphInfo[i].getNumVertices()));
	}

#if 0 // DEBUG
	{
		for (auto g = 0u; g < treeGroups.size(); g++) {
			std::cout << boost::format("\nGroup[%u]") % g;
			for (auto t = 0u; t < treeGroups[g].size(); t++) {
				std::cout << boost::format("\n  Tree[%u] ") % t << treeGroups[g][t].getSExpression();
			}
			std::cout << "\n  -----";
			std::cout << "\n" << executables[g]->getScript();
			std::cout << "\n  -----";
			auto begin = executables[g]->getEntryPoint(), it = begin;
			do {
				if ((it - begin) % 16 == 0) std::cout << "\n  ";
				std::cout << boost::format("%04x ") % *it;
			} while (*it++ != 0);
			std::cout << "\n=====";
		}
	}
#endif

	// 3. Execute.
	// Touch result space.
	for (auto i = 0u; i < Nt * Ni * _Nc; i++) {
		_archive->getPair(0x100000u + i);
	}
	// Execute
	std::cout << "3";
	_ivp.execute(executables, getAArchives(Ni, Nt), engine->getThreadPool());

	// [Log] Save graph information to individuals.
	auto lastTreeInfo = _ivp.getLastTreeInfo();   // Information per tree.
	for (auto i = 0u; i < lastGraphInfo.size(); i++) {
		individuals._scripts.push_back(lastGraphInfo[i].script);
	}
	for (auto i = 0u; i < Ni; i++) {
		auto& ind = individuals[i];
		for (auto c = 0u; c < _Nc; c++) {
			auto& chr = ind.chromosome[c];
			auto intermediate = chr.getIntermediate<IVTreeIntermediate>();

			intermediate->igraph = lastTreeInfo[i * _Nc + c].igraph;
			intermediate->vgraph = lastTreeInfo[i * _Nc + c].vgraph;
			intermediate->agraph = lastTreeInfo[i * _Nc + c].agraph;

			for (auto t = 0u; t < Nt; t++) {
				intermediate->iv.push_back(_archive->pairAt(0x100000u + t * (Ni * _Nc) + i * _Nc + c));
			}
		}
	}

	if (isStorageTrace()) {
		for (auto t = 0u; t < Nt; t++) {
			for (auto s = 0u; s < _Ns; s++) {
				std::string fn = (boost::format("source_t%u_s%u.png") % t % s).str();
				getAArchive(t, Ni)->saveImage(ADDR_SI3B + s, fn); //!
			}
		}

		for (auto t = 0u; t < Nt; t++) {
			for (auto i = 0u; i < Ni; i++) {
				for (auto c = 0u; c < _Nc; c++) {
					std::string fn = (boost::format("tree_t%u_i%u_c%c.png") % t % i % c).str();
					getAArchive(t, Ni)->saveImage(ADDR_SI1B + _Ns + i * _Nc + c, fn); //!
				}
			}
		}
	}

	// Show information.
	if (isLocationTrace()) {
		std::cout << boost::format("[%s] Gather Trees               %.6fs\n") % __FUNCTION__ % watches[11].get();
		std::cout << boost::format("[%s] Compile Trees              %.6fs\n") % __FUNCTION__ % watches[12].get();
		std::cout << boost::format("[%s] Evaluate Trees             %.6fs\n") % __FUNCTION__ % watches[13].get();
		std::cout << boost::format("[%s] Evaluate Raw Fitness       %.6fs\n") % __FUNCTION__ % watches[21].get();
		std::cout << std::flush;
	}
}

void Garnet::IVTreeStrategy::IVTreeEvaluator::evaluateRawFitness(GPEngine* engine, IndividualList& individuals, uint32_t Nt)
{
	using namespace Garnet;
	using namespace PicturePerfect;

	const Configuration& conf = engine->getConfiguration();

	const size_t Ni = individuals.size();

	std::map<uint32_t, StopWatch> watches;

	// 1. Compile
	std::vector<PicturePerfect::ExecutablePtr> executables;
	std::vector<Tree> evaluators;
	for (auto r = 0u; r < _Nr; r++) {
		Tree tree(conf.evaluator[r]);

		// Expand aliases.
		resolveAliases(tree);

		for (auto i = 0u; i < Ni; i++) {
			Tree treeLocal = tree;

			//ConfigLoader::StringMap aliases;
			std::map<std::string, Tree> aliases;
			for (auto c = 0u; c < _Nc; c++) {
				aliases[(boost::format("Tree%u") % c).str()] = (boost::format("(_Load%u)") % (_Nk + _Ns + i * _Nc + c)).str();
			}

			// Expand special aliases and append the root node.
			Garnet::resolveAliases(treeLocal, aliases.cbegin(), aliases.cend());
			Tree root = (boost::format("(_IVStore%u(!))") % (i * _Nr + r)).str();
			root.replace("!", treeLocal);

			// Convert S-expression to Tree and store it.
			evaluators.push_back(root);
		}
	}
	if (isProcessTrace()) {
		for (auto r = 0u; r < _Nr; r++) {
			std::cout << boost::format("\n[%s] Evaluator0[%u]: %s") % __FUNCTION__ % r % evaluators[Ni * r].getSExpression();
		}
	}
	// Generate executable.
	StopWatch watch;
	executables = _ivp.compile(evaluators);
	watch.stop();
	//std::cout << std::endl << info.script;

	// Show information.
	if (Garnet::isLocationTrace()) {
		std::cout << boost::format("\n[%s] Compile Evaluator   %.6fs") % __FUNCTION__ % watch.get();
		std::cout << boost::format("\n[%s] Number of Operators %.6u") % __FUNCTION__ % executables[0]->getNumberOfOperators();
		std::cout << boost::format("\n[%s] Number of Data      %.6u") % __FUNCTION__ % executables[0]->getNumberOfData();
		std::cout << std::flush;
	}



	// 2. Evaluate evaluators.
	std::vector<PicturePerfect::ArchivePtr> archives = getBArchives(Ni, Nt);
	// 2.1 Evaluation.
	// Touch result space.
	for (auto i = 0u; i < Nt * Ni * _Nr; i++) {
		_archive->getPair(0x500000u + i);
	}
	// Execute
	std::cout << "2";
	_ivp.execute(executables, archives, engine->getThreadPool());

	// 2.2 Save raw fitnesses to individuals.
	std::cout << "2";
	WATCH_DO(watches[22], [&]() {
		for (auto i = 0u; i < Ni; i++) {
			Garnet::Individual::RawFitness& rawFitness = individuals[i].rawFitness;
			rawFitness.resize(_Nr);
			for (auto r = 0u; r < _Nr; r++) {
				rawFitness[r].resize(Nt);
				for (auto t = 0u; t < Nt; t++) {
					const ArchivePtr& archive = archives[t]; //!
					rawFitness[r][t] = archive->getValue(ADDR_RV1D + i * _Nr + r);
				}
			}
		}
	});
	if (isProcessTrace()) {
		std::cout << "----- ----- ----- -----\n";
		std::cout << "Raw Fitness: row=(i,t), col=(r)\n";
		for (auto i = 0u; i < Ni; i++) {
			std::cout << boost::format("Individual %u\n") % i;
			for (auto t = 0u; t < Nt; t++) {
				ArchivePtr rawFitArchive = getBArchive(t, Ni); //!
				std::vector<double> values;
				for (auto r = 0u; r < _Nr; r++) {
					values.push_back(rawFitArchive->getValue(ADDR_RV1D + i * _Nr + r));

					if (isStorageTrace()) {
						std::string fn = (boost::format("rawfit_i%u_t%u_r%u.png") % i % t % r).str();
						rawFitArchive->saveImage(ADDR_RI1B + i * _Nr + r, fn);
					}
				}
				std::cout << boost::format("  Task%u: (") % t
					<< print_range(values.begin(), values.end(), ", ")
					<< ")\n";
			}
		}
		std::cout << std::flush;
	}

	// Show information.
	if (isLocationTrace()) {
		std::cout << boost::format("[%s] Gather Trees               %.6fs\n") % __FUNCTION__ % watches[11].get();
		std::cout << boost::format("[%s] Compile Trees              %.6fs\n") % __FUNCTION__ % watches[12].get();
		std::cout << boost::format("[%s] Evaluate Trees             %.6fs\n") % __FUNCTION__ % watches[13].get();
		std::cout << boost::format("[%s] Evaluate Raw Fitness       %.6fs\n") % __FUNCTION__ % watches[21].get();
		std::cout << std::flush;
	}
}

/**
 *  Size
 *    0 < Nindividuals.  (Do NOT assume it as same as the population size.)
 */
void Garnet::IVTreeStrategy::IVTreeEvaluator::evaluateFitnessVector(GPEngine* engine, IndividualList& individuals)
{
	using namespace Garnet;
	using namespace PicturePerfect;

	const size_t Ni = individuals.size();

	std::map<uint32_t, StopWatch> watches;

	std::vector<PicturePerfect::ArchivePtr>  archives = getCArchives(Ni);

	// 3. Preprocess and relocate task results for archives
	std::vector<uint32_t> distCollins(Ni * Ni);
	std::vector<uint32_t> distKashima(Ni * Ni);
	std::vector<double>   distNSGA2(Ni);
	std::cout << "3";
#if 0
	watches[31].reset();
	if ( engine->isDistCollinsEnabled() ) {
		JobQueue jobQueue;
		uint32_t count = 0, st = 0u;
		for (auto i1 = 0u; i1 < Ni; i1++) {
			count += Ni - (i1 + 1);
			if ( count >= 900 || i1 == Ni - 1) {
				Job* job = new DistCollinsJob(
						individuals, st, i1 + 1, distCollins);
				jobQueue.push(job);
				count = 0;
				st = i1 + 1;
			}
		}
		engine->getThreadPool()->start(jobQueue);
		engine->getThreadPool()->wait();
	}
	if ( engine->isDistKashimaEnabled() ) {
		JobQueue jobQueue;
		uint32_t count = 0, st = 0u;
		for (auto i1 = 0u; i1 < Ni; i1++) {
			count += Ni - (i1 + 1);
			if ( count >= 900 || i1 == Ni - 1) {
				Job* job = new DistKashimaJob(
						individuals, st, i1 + 1, distKashima);
				jobQueue.push(job);
				count = 0;
				st = i1 + 1;
			}
		}
		engine->getThreadPool()->start(jobQueue);
		engine->getThreadPool()->wait();
	}
	watches[31].stop();
#endif

	// 3.2 
	std::cout << "3";
	watches[30].reset();
	for (auto i = 0u; i < Ni; i++) {
		double value;

		// RawFit
		for (auto r = 0u; r < _Nr; r++) {
			archives[i]->arrayAt(ADDR_SA1D + _Nk + r) = individuals[i].rawFitness[r];
			archives[i]->valueAt(ADDR_SV1D + _Nk + r) = individuals[i].rawFitness[r].average();
		}

		// Nodes
		value = 0;
		for (auto c = 0u; c < _Nc; c++) {
			value += individuals[i].chromosome[c].getNumVertices();
		}
		archives[i]->setValue(ADDR_SV1D + _Nk + _Nr + 0, value);

		// ExecTime
		value = 0;
#if 0
		if ( engine->isExecTimeEnabled() ) {
			for (auto c = 0u; c < Nc; c++) {
				const Tree& tree = trees[i * Nc + c]; // Alias-resolved trees.
				for (auto vrange = boost::vertices(tree.getGraph()); vrange.first != vrange.second; vrange.first++) {
					Tree::GraphType  g = tree.getGraph();
					Tree::VertexDesc u = *vrange.first;
					if ( !Text::startsWith(g[u].label, "_Src") && !Text::startsWith(g[u].label, "Sink") ) {
						const IVInstruction& inst = Dictionary::findByIVName(g[u].label);
						uint16_t opcode;
						opcode = engine->getPPE()->getOpCode(inst.pp[0].label);
						if ( opcode != 0xffffu ) value += engine->getTreeExecutable(0)->statistics[opcode].getAverage();
						opcode = engine->getPPE()->getOpCode(inst.pp[1].label);
						if ( opcode != 0xffffu ) value += engine->getTreeExecutable(0)->statistics[opcode].getAverage();
					}
				}
			}
		}
#endif
		archives[i]->setValue(ADDR_SV1D + _Nk + _Nr + 1, value);

		// DistCollins
		//   max kernel number, propotioned by num of vertices.
		value = 0;
#if 0
		if ( engine->isDistCollinsEnabled() ) {
			uint32_t i2_max = 0u;
			for (auto i2 = 0u; i2 < Ni; i2++) {
				if ( distCollins[i * Ni + i2] > value ) {
					value = distCollins[i * Ni + i2];
					i2_max = i2;
				}
			}

			uint32_t size = 0;
			for (auto c = 0u; c < Nc; c++) {
				size += individuals[i].chromosome[c].getNumVertices();
			}
			value /= size;
			size = 0;
			for (auto c = 0u; c < Nc; c++) {
				size += individuals[i2_max].chromosome[c].getNumVertices();
			}
			value /= size;
		}
#endif
		archives[i]->setValue(ADDR_SV1D + _Nk + _Nr + 2, value);

		// DistKashima
		value = 0;
#if 0
		if ( engine->isDistKashimaEnabled() ) {
			for (auto i2 = 0u; i2 < Ni; i2++) {
				if ( distKashima[i * Ni + i2] > value ) {
					value = distKashima[i * Ni + i2];
				}
			}
		}
#endif
		archives[i]->setValue(ADDR_SV1D + _Nk + _Nr + 3, value);

		// DistNSGA2
		//   DistNSGA2 will be calculated after fitness vector has fixed.
		value = 0;
		archives[i]->setValue(ADDR_SV1D + _Nk + _Nr + 4, value);
	};
	watches[30].stop();

	// 4. Calculate fintess vector elements.
	// Touch result space.
	for (auto i = 0u; i < Ni * _Nf; i++) {
		_archive->getPair(0xD0C000u + i);
	}
	// Execute
	std::cout << "4";
	_ivp.execute(_fitnessExecutables, getCArchives(Ni), engine->getThreadPool());

	// 5. Construct fitness vectors.
	std::cout << "5";
	WATCH_DO(watches[50], [&]() {
		for (auto i = 0u; i < Ni; i++) {
			individuals[i].fitness.resize(_Nf);
			for (auto f = 0u; f < _Nf; f++) {
				individuals[i].fitness[f] = archives[i]->valueAt(ADDR_RV1D + f);
			}
		}
	});

	// 6. Assign values calculated in object space.
	watches[60].reset();
	if ( isDistNSGA2Enabled() ) {
		// Calculate NGSA-II Crowding-distance-assignment
		{//if ( conf.numThreads <= 1 ) {
			NSGA2Strategy::calculateCrowdedDistanceAssignment(individuals.begin(), individuals.end());
		}
#if 0
		// Multi-thread version.
		else {
			JobQueue jobQueue;
			Garnet::CollinsKernel<Tree::GraphType> ck;
			uint32_t count = 0, st = 0u;
			for (auto i1 = 0u; i1 < Ni; i1++) {
				count += Ni - (i1 + 1);
				if ( count >= 150 || i1 == Ni - 1) {
					Job* job = new DistCollinsJob(
							individuals, st, i1 + 1, distCollins);
					jobQueue.push(job);
					count = 0;
					st = i1 + 1;
				}
			}
			engine->getThreadPool()->start(jobQueue);
			engine->getThreadPool()->wait();
		}
#endif
	}
	watches[60].stop();

	// Show information.
	if ( isLocationTrace() ) {
		std::cout << boost::format("[%s] Gather Raw Fitness         %.6fs\n") % __FUNCTION__ % watches[30].get();
		std::cout << boost::format("[%s]   Collins Distance         %.6fs\n") % __FUNCTION__ % watches[31].get();
		std::cout << boost::format("[%s] Calculate Fitness Vectors  %.6fs\n") % __FUNCTION__ % watches[40].get();
		std::cout << boost::format("[%s] Construct Fitness Vectors  %.6fs\n") % __FUNCTION__ % watches[50].get();
		std::cout << std::flush;
	}
}

std::vector<PicturePerfect::ArchivePtr>  Garnet::IVTreeStrategy::IVTreeEvaluator::getAArchives(uint32_t Ni, uint32_t Nt)
{
	std::vector<PicturePerfect::ArchivePtr> archives;
	for (auto t = 0u; t < Nt; t++) {
		archives.push_back(getAArchive(t, Ni));
	}
	return archives;
}

std::vector<PicturePerfect::ArchivePtr>  Garnet::IVTreeStrategy::IVTreeEvaluator::getBArchives(uint32_t Ni, uint32_t Nt)
{
	std::vector<PicturePerfect::ArchivePtr> archives;
	for (auto t = 0u; t < Nt; t++) {
		archives.push_back(getBArchive(t, Ni));
	}
	return archives;
}

std::vector<PicturePerfect::ArchivePtr>  Garnet::IVTreeStrategy::IVTreeEvaluator::getCArchives(uint32_t Ni)
{
	std::vector<PicturePerfect::ArchivePtr> archives;
	for (auto i = 0u; i < Ni; i++) {
		archives.push_back(getCArchive(i));
	}
	return archives;
}

PicturePerfect::ArchivePtr  Garnet::IVTreeStrategy::IVTreeEvaluator::getAArchive(unsigned task, uint32_t Ni)
{
	PicturePerfect::StorageMapper::Blocks blocks(9);
	auto archive = _archive->getArchive();

	// Constant
	blocks[0].set(ADDR_SV1D, archive, ADDR_V1D + 0x000000u, _Nk);
	blocks[1].set(ADDR_SI3B, archive, ADDR_I3B + 0x000000u, _Nk);
	blocks[2].set(ADDR_SA1D, archive, ADDR_A1D + 0x000000u, _Nk);
	// Source (Task)
	blocks[3].set(ADDR_SV1D + _Nk, archive, ADDR_V1D + 0x004000u + task * _Ns, _Ns);
	blocks[4].set(ADDR_SI3B + _Nk, archive, ADDR_I3B + 0x004000u + task * _Ns, _Ns);
	blocks[5].set(ADDR_SA1D + _Nk, archive, ADDR_A1D + 0x004000u + task * _Ns, _Ns);
	// Tree (Task)
	blocks[6].set(ADDR_RV1D, archive, ADDR_V1D + 0x100000u + task * Ni *_Nc, Ni * _Nc);
	blocks[7].set(ADDR_RI1B, archive, ADDR_I1B + 0x100000u + task * Ni *_Nc, Ni * _Nc);
	blocks[8].set(ADDR_RA1D, archive, ADDR_A1D + 0x100000u + task * Ni *_Nc, Ni * _Nc);

	return PicturePerfect::Archive::createInstance(PicturePerfect::StorageMapper::createInstance(blocks));
}

PicturePerfect::ArchivePtr  Garnet::IVTreeStrategy::IVTreeEvaluator::getBArchive(unsigned task, uint32_t Ni)
{
	std::vector<PicturePerfect::StorageMapper::Block> blocks(12);
	auto archive = _archive->getArchive();

	// Constant
	blocks[ 0].set(ADDR_SV1D, archive, ADDR_V1D + 0x000000u, _Nk);
	blocks[ 1].set(ADDR_SI3B, archive, ADDR_I3B + 0x000000u, _Nk);
	blocks[ 2].set(ADDR_SA1D, archive, ADDR_A1D + 0x000000u, _Nk);
	// Source (Task)
	blocks[ 3].set(ADDR_SV1D + _Nk, archive, ADDR_V1D + 0x004000u + task * _Ns, _Ns);
	blocks[ 4].set(ADDR_SI3B + _Nk, archive, ADDR_I3B + 0x004000u + task * _Ns, _Ns);
	blocks[ 5].set(ADDR_SA1D + _Nk, archive, ADDR_A1D + 0x004000u + task * _Ns, _Ns);
	// Tree (Task)
	blocks[ 6].set(ADDR_SV1D + _Nk + _Ns, archive, ADDR_V1D + 0x100000u + task * Ni *_Nc, Ni * _Nc);
	blocks[ 7].set(ADDR_SI1B + _Nk + _Ns, archive, ADDR_I1B + 0x100000u + task * Ni *_Nc, Ni * _Nc);
	blocks[ 8].set(ADDR_SA1D + _Nk + _Ns, archive, ADDR_A1D + 0x100000u + task * Ni *_Nc, Ni * _Nc);
	// RawFit (Task)
	blocks[ 9].set(ADDR_RV1D, archive, ADDR_V1D + 0x500000u + task * Ni *_Nr, Ni * _Nr);
	blocks[10].set(ADDR_RI1B, archive, ADDR_I1B + 0x500000u + task * Ni *_Nr, Ni * _Nr);
	blocks[11].set(ADDR_RA1D, archive, ADDR_A1D + 0x500000u + task * Ni *_Nr, Ni * _Nr);

	return PicturePerfect::Archive::createInstance(PicturePerfect::StorageMapper::createInstance(blocks));
}

PicturePerfect::ArchivePtr  Garnet::IVTreeStrategy::IVTreeEvaluator::getCArchive(unsigned ind)
{
	std::vector<PicturePerfect::StorageMapper::Block> blocks(12);
	auto archive = _archive->getArchive();

	// Constant
	blocks[ 0].set(ADDR_SV1D, archive, ADDR_V1D + 0x000000u, _Nk);
	blocks[ 1].set(ADDR_SI3B, archive, ADDR_I3B + 0x000000u, _Nk);
	blocks[ 2].set(ADDR_SA1D, archive, ADDR_A1D + 0x000000u, _Nk);
	// RawFit List (Task)
	blocks[ 3].set(ADDR_SV1D + _Nk, archive, ADDR_V1D + 0xD00000u + ind *_Nr, _Nr);
	blocks[ 4].set(ADDR_SI1B + _Nk, archive, ADDR_I1B + 0xD00000u + ind *_Nr, _Nr);
	blocks[ 5].set(ADDR_SA1D + _Nk, archive, ADDR_A1D + 0xD00000u + ind *_Nr, _Nr);
	// Nodes, etc. (Individual)
	blocks[ 6].set(ADDR_SV1D + _Nk + _Nr, archive, ADDR_V1D + 0xD08000u + ind * _Na, _Na);
	blocks[ 7].set(ADDR_SI1B + _Nk + _Nr, archive, ADDR_I1B + 0xD08000u + ind * _Na, _Na);
	blocks[ 8].set(ADDR_SA1D + _Nk + _Nr, archive, ADDR_A1D + 0xD08000u + ind * _Na, _Na);
	// Fitness
	blocks[ 9].set(ADDR_RV1D, archive, ADDR_V1D + 0xD0C000u + ind * _Nf, _Nf);
	blocks[10].set(ADDR_RI1B, archive, ADDR_I1B + 0xD0C000u + ind * _Nf, _Nf);
	blocks[11].set(ADDR_RA1D, archive, ADDR_A1D + 0xD0C000u + ind * _Nf, _Nf);

	return PicturePerfect::Archive::createInstance(PicturePerfect::StorageMapper::createInstance(blocks));
}
