//
//  Copyright(c) 2009-2014 Munehiro Doi
//
#ifndef GARNET__THREAD_MANAGER__H__
#define GARNET__THREAD_MANAGER__H__

#include <memory>
#include <mutex>
#include <condition_variable>

class ThreadPool;

namespace Garnet {

/**
 *  A job running as a thread must be a instance of (a sub-class of) Job.
 *  getThreadId() returns a thread id where the thread runs on. It is usable to determine a resource the thread should use.
 *
 *  handle = Threads::enqueue(job);
 *  handle.join();
 */
class Job
{
public :
	virtual ~Job() {};
    virtual void run(int threadId) = 0;
};

template <typename T>
class JobAdapter : public Job
{
public :
	JobAdapter(T func) : _func(func) {}
	virtual ~JobAdapter() {}

    virtual void run(int threadId) {
        _func->run(int threadId);
	}
private :
	T   _func;
};

template <typename T>
JobAdapter<T>* makeJob(T func) {
	return new JobAdapter<T>(func);
}

class JobHandle {
public:
    virtual bool  join() = 0;

protected:
    virtual bool  contains(Job* job) = 0;
    virtual void  run(int threadId) = 0;
    virtual void  finish() = 0;

private :
    friend class ThreadPool;
};
typedef std::shared_ptr<JobHandle> JobHandlePtr;

/**
 *  A thread pool executes threads in order of FIFO basis.
 */
class ThreadManager
{
public :
    static JobHandlePtr  enqueue(Job* job);
    static int           getNumThreads();
    static void          initialize(int numThreads);
    static void          terminate();
};

}//end of namespace Garnet

#endif//GARNET__THREAD_MANAGER__H__

