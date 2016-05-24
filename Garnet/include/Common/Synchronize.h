
#ifndef SYNCHRONIZE__H__
#define SYNCHRONIZE__H__

#if defined(_WIN32)
#include <Windows.h>
#else
#include <pthread.h>
#endif

/**
 *
 */
#if defined(_WIN32)
template <typename T>
class synchronized : public T
{
public :
    synchronized() : T() { _mutex = CreateMutex(NULL, FALSE, NULL); }
    virtual ~synchronized() { CloseHandle(_mutex); }

    void  lock() { WaitForSingleObject(_mutex, INFINITE); }
    void  unlock() { ReleaseMutex(_mutex); }
private :
    HANDLE _mutex;
};

#else
template <typename T>
class synchronized : public T
{
public :
    synchronized() : T() { pthread_mutex_init(&_mutex, NULL); }
    virtual ~synchronized() { pthread_mutex_destroy(&_mutex); }

    void  lock() { pthread_mutex_lock(&_mutex); }
    void  unlock() { pthread_mutex_unlock(&_mutex); }
private :
    pthread_mutex_t _mutex;
};
#endif

/**
 *  Mutex
 */
struct _DummyObjectForMutex
{
	int a;
};
typedef synchronized<_DummyObjectForMutex>  Mutex;

#endif//SYNCHRONIZE__H__
