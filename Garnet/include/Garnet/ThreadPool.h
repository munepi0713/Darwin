//
//  Copyright(c) 2009-2014 Munehiro Doi
//
#ifndef GARNET__THREAD_POOL__H__
#define GARNET__THREAD_POOL__H__

#include <memory>

namespace Garnet {

class Job;

class JobHandle;
typedef std::shared_ptr<JobHandle> JobHandlePtr;

/**
*  A thread pool executes threads in order of FIFO basis.
*/
class ThreadPool {
public:
    ThreadPool(int numThreads) {};
    virtual ~ThreadPool() = 0;
    virtual JobHandlePtr  enqueue(Job* job) = 0;
    virtual int           getNumThreads() const = 0;

protected :
    bool                     contains(JobHandlePtr handle, Job* job);
    void                     run(JobHandlePtr handle, int threadId);
};

}//end of namespace Garnet

#endif//GARNET__THREAD_POOL__H__

