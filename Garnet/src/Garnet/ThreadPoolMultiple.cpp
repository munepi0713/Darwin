
//
//  Copyright(c) 2009-2014 Munehiro Doi
//
#include <Garnet/ThreadPoolMultiple.h>

#include <Garnet/ThreadManager.h>

//---------------------------------------------------------

namespace {

class ThreadHandleMultiple : public Garnet::JobHandle {
public:
    ThreadHandleMultiple(Garnet::ThreadPoolMultiple* pool, Garnet::Job* job);
    virtual bool  join();

protected:
    virtual bool  contains(Garnet::Job* job);
    virtual void  run(int threadId);
    virtual void  finish();

private:
    Garnet::ThreadPoolMultiple* _pool;
    Garnet::Job*                _job;
    bool                        _isFinished;
    std::mutex                  _mutex;
    std::condition_variable     _cond;
};

ThreadHandleMultiple::ThreadHandleMultiple(Garnet::ThreadPoolMultiple* pool, Garnet::Job* job) {
    _pool = pool;
    _job = job;
    _isFinished = false;
}

bool ThreadHandleMultiple::join() {
    std::unique_lock<std::mutex> lock(_mutex);
    if (_job) {
        // Wait for the job finished.
        while (!_isFinished) {
            _cond.wait(lock);
        }
        // Release the job resources.
        delete _job;
        _job = 0;

        return true;
    }
    else {
        // This handle has been already joined.
        return false;
    }
}

bool ThreadHandleMultiple::contains(Garnet::Job* job) {
    std::unique_lock<std::mutex> lock(_mutex);
    return _job == job;
}

void ThreadHandleMultiple::run(int threadId) {
    std::unique_lock<std::mutex> lock(_mutex);
    _job->run(threadId);
    _isFinished = true;
    _cond.notify_all();
}

}//end of local namespace

//---------------------------------------------------------

Garnet::ThreadPoolMultiple::ThreadPoolMultiple(int numThreads)
: ThreadPool(numThreads) {
    _isTerminated = false;

    // Start threads.
    for (int i = 0; i < getNumThreads(); ++i) {
        _threads.push_back(std::thread(threadFunc, this, i));
    }
}

Garnet::ThreadPoolMultiple::~ThreadPoolMultiple() {
    // Make all threads escape from their infinite loops.
    std::unique_lock<std::mutex> lock(_mutex);
    _isTerminated = true;
    lock.unlock();

    // Join all threads.
    for (auto it = _threads.begin(); it != _threads.end(); ++it) {
        it->join();
    }
    _threads.clear();
}

Garnet::JobHandlePtr  Garnet::ThreadPoolMultiple::enqueue(Job* job) {
    std::unique_lock<std::mutex> lock(_mutex);
    JobHandlePtr handle = JobHandlePtr(new ThreadHandleMultiple(this, job));
    _queue.push(handle);
    _cond.notify_all();
}

int  Garnet::ThreadPoolMultiple::getNumThreads() const {
    return static_cast<int>(_threads.size());
}

/**
 * 
 */
void Garnet::ThreadPoolMultiple::threadFunc(ThreadPoolMultiple* pool, int threadId) {
    for (;;) {
        // Retrieve a new job, or wait if no jobs are queued.
        std::unique_lock<std::mutex> lock(pool->_mutex);
        while (!pool->_isTerminated && pool->_queue.empty()) {
            pool->_cond.wait(lock);
        }
        if (pool->_isTerminated) {
            // |lock| will be unlocked when the lock goes out of scope.
            break;
        }

        // Pop a job handle.
        JobHandlePtr handle = pool->_queue.front();
        pool->_queue.pop();
        lock.unlock();

        // Execute the job.
        pool->run(handle, threadId);
    }
}
