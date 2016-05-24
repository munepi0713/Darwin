
//
//  Copyright(c) 2009-2014 Munehiro Doi
//
#include <Garnet/ThreadPool.h>

#include <cassert>
#include <queue>
#include <thread>

#include <Garnet/ThreadManager.h>

bool Garnet::ThreadPool::contains(JobHandlePtr handle, Job* job) {
    return handle->contains(job);
}

void Garnet::ThreadPool::run(Garnet::JobHandlePtr handle, int threadId) {
    handle->run(threadId);
}
