//
//  Copyright(c) 2009-2014 Munehiro Doi
//
#ifndef GARNET__THREAD_POOL_MULTIPLE__H__
#define GARNET__THREAD_POOL_MULTIPLE__H__

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include <Garnet/ThreadPool.h>

namespace Garnet {

/**
 *  A thread pool instance for a multiple thread operation.
 */
class ThreadPoolMultiple : public ThreadPool {
public :
    ThreadPoolMultiple(int numThreads);
    virtual ~ThreadPoolMultiple();
    virtual JobHandlePtr  enqueue(Job* job);
    virtual int           getNumThreads() const;

    Job*      getNextJob();
    bool      waitActive();
	void      complete(Job* job);
private :
    std::queue<JobHandlePtr>  _queue;
    bool                      _isTerminated;
    std::vector<std::thread>  _threads;
    std::mutex                _mutex;
    std::condition_variable   _cond;

    static void threadFunc(ThreadPoolMultiple* owner, int threadId);
};

}//end of namespace Garnet

#endif//GARNET__THREAD_POOL_MULTIPLE__H__

