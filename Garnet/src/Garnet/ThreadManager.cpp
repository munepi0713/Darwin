
//
//  Copyright(c) 2009-2014 Munehiro Doi
//
#include <Garnet/ThreadManager.h>

#include <cassert>

#include <Garnet/ThreadPoolSingle.h>
#include <Garnet/ThreadPoolMultiple.h>

namespace {
Garnet::ThreadPoolSingle _defaultInstance(1);
Garnet::ThreadPool*      _instance = &_defaultInstance;
}

Garnet::JobHandlePtr Garnet::ThreadManager::enqueue(Job* job) {
    assert(_instance != NULL);
    return _instance->enqueue(job);
}

int Garnet::ThreadManager::getNumThreads() {
    assert(_instance != NULL);
    return _instance->getNumThreads();
}

void Garnet::ThreadManager::initialize(int numThreads) {
    if (_instance && _instance != &_defaultInstance) {
        delete _instance;
    }
	if (numThreads == 1) {
        _instance = &_defaultInstance;
	}
	else {
        _instance = new ThreadPoolMultiple(numThreads);
	}
}

void Garnet::ThreadManager::terminate() {
    delete _instance;
    _instance = NULL;
}
