//
//  Copyright(c) 2009-2014 Munehiro Doi
//
#ifndef GARNET__THREAD_POOL_SINGLE__H__
#define GARNET__THREAD_POOL_SINGLE__H__

#include <memory>
#include <queue>

#include <Garnet/ThreadPool.h>

namespace Garnet {

/** 
 *  A thread pool instance for a single thread operation.
 *  Actually, this class does not create any thread. It processes all jobs sequentially
 *  in the same context which calls enqueue() method.
 */
class ThreadPoolSingle : public ThreadPool {
public :
    ThreadPoolSingle(int numThreads) : ThreadPool(numThreads) {}
	virtual ~ThreadPoolSingle() {}

    virtual JobHandlePtr  enqueue(Job* job);
    virtual int           getNumThreads() const { return 1; }

    void                  join(Job* job);

private :
    std::queue<JobHandlePtr>  _queue;
};


}//end of namespace Garnet

#endif//GARNET__THREAD_POOL_SINGLE__H__

