/*********************************************************
 *  Garnet
 *  Author: Munehiro Doi.
 *********************************************************/
#ifndef GARNET__GET_TIME__H__
#define GARNET__GET_TIME__H__

#include <stdint.h>

#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>

static inline
uint64_t getTickCount()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
    return (uint64_t)counter.QuadPart;
}

static inline
uint64_t getTickFreq()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
    return (uint64_t)freq.QuadPart;
}

static inline
double getSec()
{
	static LARGE_INTEGER _freq;
	if ( _freq.QuadPart == 0 ) QueryPerformanceFrequency(&_freq);

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	return (double)counter.QuadPart / _freq.QuadPart;
}
static inline
double getMsec()
{
	static LARGE_INTEGER _freq;
	if ( _freq.QuadPart == 0 ) QueryPerformanceFrequency(&_freq);

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	return (counter.QuadPart * 1000.0) / _freq.QuadPart;
}
#else
#include <sys/time.h>

static inline
uint64_t getTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (uint64_t)tv.tv_sec * 1000000  + tv.tv_usec;
}

static inline
uint64_t getTickFreq()
{
    return (uint64_t)1000000;
}

static inline
double getSec()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}

static inline
double getMsec()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}
#endif

/**
 *  Stop watch utility. This returns a time in seconds.
 */
class StopWatch
{
public :
    StopWatch() {
        reset();
    }
    void reset() {
        _ed = _st = getSec();
    }
    void start() {
        _st += _ed - _st;
    }
    void stop() {
        _ed = getSec();
    }
    double get() {
        return _ed - _st;
    }
    double lap() {
        return getSec() - _st;
    }

private :
    double _st;
    double _ed;
};


template <typename Func>
double WATCH_DO(Func func) {
    StopWatch watch;
    watch.reset();
    func();
    watch.stop();
    return watch.get();
}

#endif//GET_TIME__H__
