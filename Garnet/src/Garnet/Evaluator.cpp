#pragma warning(disable: 4819)

#include <Garnet/Affinity.h>
#include <Garnet/ThreadPool.h>
#include <Garnet/Configuration.h>
#include <Garnet/Evaluator.h>
#include <PicturePerfectEngine/PicturePerfectEngine.h>

namespace Garnet {


//---------------------------------------------------------
//---------------------------------------------------------
class EvaluateTaskJob : public Job
{
public :
    EvaluateTaskJob(Garnet::Configuration* conf, const Garnet::Task* task, std::stringstream* ppscript, synchronized<std::vector<std::vector<double> > >* fitnesses);
	~EvaluateTaskJob();

    virtual void run();

private :
    Garnet::Configuration*                            _conf;
    const Garnet::Task*                               _task;
    std::stringstream*                                _ppscript;
    synchronized<std::vector<std::vector<double> > >* _fitnesses;
};

EvaluateTaskJob::EvaluateTaskJob(Garnet::Configuration* conf, const Garnet::Task* task, std::stringstream* ppscript, synchronized<std::vector<std::vector<double> > >* fitnesses)
{
    _conf    = conf;
    _task      = task;
    _ppscript  = ppscript;
    _fitnesses = fitnesses;
}

EvaluateTaskJob::~EvaluateTaskJob()
{
}

void EvaluateTaskJob::run()
{
    int numMaxCPUs = _conf->numMaxCPUs;

    if ( _conf->trace ) {
        std::cerr << "Worker thread started." << std::endl;
    }

    // Create own PPE object.
    PicturePerfect::PicturePerfectEngine* ppe;
	ppe = PicturePerfect::PicturePerfectEngine::createInstance();

    // Set affinity
    if ( numMaxCPUs != _conf->numMaxCPUs ) {
        setThreadAffinity(numMaxCPUs = _conf->numMaxCPUs);
    }

    // Execute PPE.
    ppe->execute(*_ppscript);

    // Put result.
	_fitnesses->lock();
    for (int n = 0; n < _conf->populationSize; n++) {
	    double fitness = ppe->getPacket(inline_sprintf("_Eval%d", n)).value;
		(*_fitnesses)[n].push_back(_task->a * fitness + _task->b);
	}
	_fitnesses->unlock();

    // Destroy PPE object.
    ppe->destroy();

	// Destroy script object.
	delete _ppscript;

	std::cout << ".";
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------

class  DefaultEvaluator : public Evaluator
{
public :
	DefaultEvaluator();
	virtual ~DefaultEvaluator();

	virtual void  execute(const Configuration& conf, const Trees& trees, const Aliases& aliases, RegisterFile& srcRegs, RegisterFile& dstRegs);

private :
	TreeSerializer*        _ts;

	ThreadPool* _threadPool;
};


//-------------------------------------------------------------------
//-------------------------------------------------------------------

Evaluator::Evaluator()
{
}

Evaluator::~Evaluator()
{
}

Evaluator*  Evaluator::createInstance(const std::string& type = "")
{
	return new DefaultEvaluator();
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------


DefaultEvaluator::DefaultEvaluator()
{

    _ts = TreeSerializer::createInstance();
	_threadPool = ThreadPool::createInstance(conf.numThreads);
}

DefaultEvaluator::~DefaultEvaluator()
{
 
    //
	_ts->destroy();

    //
    delete _threadPool;
}

void  DefaultEvaluator::execute(const Configuration& conf, const Trees& trees, const Aliases& aliases, RegisterFile& srcRegs, RegisterFile& dstRegs)
{
	//---------------------------------------------------------------
	// STEP1. Serialize trees into ppscript by TreeSerializer.
	//---------------------------------------------------------------

	std::stringstream tsscript;
    std::stringstream commonppscript;

    // 1. Generate TreeSerializer script.
    std::cout << "  Generate Script ... " << std::flush;
    //   The script includes processing for all individuals for all tasks.
    // Configurations
    tsscript << "ImageWidth " << _conf.imageWidth << std::endl;
    tsscript << "ImageHeight " << _conf.imageHeight << std::endl;
    tsscript << "LeafCache " << (_conf.leafCache ? "True" : "False") << std::endl;
    tsscript << "SubtreeReduction " << (_conf.subtreeReduction ? "True" : "False") << std::endl;
    tsscript << "SubtreeReductionDepth " << _conf.subtreeReductionDepth << std::endl;
    tsscript << "SubtreeReductionType " << _conf.subtreeReductionType << std::endl;
    tsscript << "SubtreeCache " << (_conf.subtreeCache ? "True" : "False") << std::endl;
    tsscript << "SubtreeCacheDepth " << _conf.subtreeCacheDepth << std::endl;
    tsscript << "SubtreeCacheLife " << _conf.subtreeCacheLife << std::endl;
    // Leaf
    for (unsigned int l = 0; l < _conf.tasks[0].images.size(); l++) { // Assumes all tasks have same number of images.
        //tsscript << inline_sprintf("Leaf Src%d, \"%s\", %.16f", l, task.images[l].c_str(), _conf.tasks[k].properties[l]) << std::endl;
        tsscript << inline_sprintf("Leaf Src%d, @Src%dName, @Src%dValue", l, l, l) << std::endl;
    }
    // Alias : User defined
    for (ConfigLoader::StringMap::const_iterator it = _conf.aliases.begin(); it != _conf.aliases.end(); it++) {
        std::string tree = it->second;
        tsscript << "Alias " << it->first << ", " << it->second << std::endl;
    }
    // Alias : Chromosome tree
    for (unsigned int n = 0; n < gen.individuals.size(); n++) {
        for (unsigned int m = 0; m < gen.individuals[n].chromosome.size(); m++) {
            #if 0 // for debug
            std::cerr << "Chromosome : " << gen.individuals[n].chromosome[m] << std::endl;
            #endif
            tsscript << inline_sprintf("Alias (_Tree%d_%d), ", n, m) << gen.individuals[n].chromosome[m] << std::endl;
        }
    }
    // Tree : Evaluation function
    for (unsigned int n = 0; n < gen.individuals.size(); n++) {
        std::string tree;
        tree = _conf.evaluationFunction;
        for (unsigned int m = 0; m < gen.individuals[n].chromosome.size(); m++) {
            std::string keyword = inline_sprintf("(_Tree%d)", m);
            std::string::size_type pos;
            while ( (pos = tree.find(keyword)) != std::string::npos ) {
                tree = tree.replace(pos, keyword.size(), inline_sprintf("(_Tree%d_%d)", n, m));
            }
        }
        tsscript << inline_sprintf("Tree _Eval%d, ", n) << tree << std::endl;
    }
    tsscript << std::flush;
    if ( _conf.trace ) {
        std::ofstream fout(inline_sprintf("%s/%s.%05u.tsscript.txt",  _conf.dir.c_str(), _conf.title.c_str(), gen.getGeneration()).c_str());
        fout << tsscript.str();
        fout.close();
    }
    std::cout << "OK." << std::endl;

    // Run TreeSerializer.
    std::cout << "  Serialize ... " << std::flush;
    _ts->execute(tsscript, commonppscript);
    commonppscript << std::flush;
    std::cout << "OK." << std::endl;

	//---------------------------------------------------------------
	// STEP2. Perform image processing by PicturePerfectEngine.
	//---------------------------------------------------------------
    
    // Generate ppscripts for each task.
	std::vector<std::stringstream*> ppscripts;
    for (unsigned int k = 0; k < _conf.tasks.size(); k++) {
	    const Garnet::Task& task = _conf.tasks[k];
	    std::stringstream* ppscript_ptr = new std::stringstream();
	    std::stringstream& ppscript = *ppscript_ptr;

		// Leaf
		for (unsigned int l = 0; l < task.images.size(); l++) {
			ppscript << inline_sprintf("Const @Src%uName, \"%s\"", l, task.images[l].c_str()) << std::endl;
			ppscript << inline_sprintf("Const @Src%uValue, %.16f", l, task.properties[l]) << std::endl;
		}

		// Common part
		ppscript << commonppscript.str();

		// Save PPScript
		if ( _conf.trace ) {
			std::ofstream fout(inline_sprintf("%s/%s.%05u.%05u.ppscript.txt",  _conf.dir.c_str(), _conf.title.c_str(), gen.getGeneration(), k).c_str());
			fout << ppscript.str();
			fout.close();
		}
		
		// Add to list.
		ppscripts.push_back(ppscript_ptr);
	}

    //
    //std::vector<std::vector<double> > fitnesses(gen.individuals.size());
    synchronized<std::vector<std::vector<double> > > fitnesses;
    fitnesses.assign(gen.individuals.size(), std::vector<double>());

    // Create Job List.
    // Job:
    //   If a thread become to be able to process a job, invoke run().
    // Input:
    //   task information, commonscript.
    // Output:
    //   fitnesses.
    //   NOTE: PicturePerfectEngine object is not preserved.
    JobQueue jobQueue;
    for (unsigned int k = 0; k < ppscripts.size(); k++) {
	    const Garnet::Task& task = _conf.tasks[k];
        Job* job = new EvaluateTaskJob(this, &task, ppscripts[k], &fitnesses);
        jobQueue.push(job);
    }
    
    // Process jobs.
	std::cout << "  Process Task " << std::flush;
    _threadPool->start(jobQueue);
    _threadPool->wait();
	std::cout << std::endl;
}

}//end of namespace Garnet
