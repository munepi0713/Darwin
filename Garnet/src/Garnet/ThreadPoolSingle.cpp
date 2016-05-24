
//
//  Copyright(c) 2009-2014 Munehiro Doi
//
#include <Garnet/ThreadPoolSingle.h>

#include <Garnet/ThreadManager.h>

//---------------------------------------------------------

namespace {

class ThreadHandleSingle : public Garnet::JobHandle {
public:
    ThreadHandleSingle(Garnet::ThreadPoolSingle* pool, Garnet::Job* job);
    virtual bool  join();

protected:
    virtual bool  contains(Garnet::Job* job);
    virtual void  run(int threadId);
    virtual void  finish();

private:
    Garnet::ThreadPoolSingle* _pool;
    Garnet::Job*              _job;
    bool                      _isFinished;
};

ThreadHandleSingle::ThreadHandleSingle(Garnet::ThreadPoolSingle* pool, Garnet::Job* job) {
    _pool       = pool;
    _job        = job;
    _isFinished = false;
}

bool ThreadHandleSingle::join() {
    if (_job) {
        if (!_isFinished) {
            _pool->join(_job);
            // Pool may call finish() of this handle.
        }
        delete _job;
        _job = 0;
        return true;
    }
    else {
        return false;
    }
}

bool ThreadHandleSingle::contains(Garnet::Job* job) {
    return _job == job; 
}

void ThreadHandleSingle::run(int threadId) {
    _job->run(threadId);
    _isFinished = true;
}

}//end of local namespace


Garnet::JobHandlePtr Garnet::ThreadPoolSingle::enqueue(Job* job) {
    JobHandlePtr handle = JobHandlePtr(new ThreadHandleSingle(this, job));
    _queue.push(handle);
}

void Garnet::ThreadPoolSingle::join(Job* job) {
    for (;;) {
        JobHandlePtr handle = _queue.front();
        if (contains(handle, job)) {
            run(handle, 0);
        }
        _queue.pop();
    }
}
