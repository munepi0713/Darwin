//
//  Copyright(c) 2009-2014 Munehiro Doi
//
#ifndef GARNET__TRACE_LEVEL__H__
#define GARNET__TRACE_LEVEL__H__

#include <cstdio>
#include <iostream>
#include <string>

#include <Garnet/Configuration.h>

namespace Garnet {

const int TRACE_LOCATION = 0;
const int TRACE_SCRIPT = 1;
const int TRACE_STORAGE = 2;
const int TRACE_PROCESS = 3;

/**
* Trace flag.
*   0: No trace.
*   1: Location trace. Show executing location and its time.
*   2: Script trace. Save script files to files.
*   3: Storage trace. Save storage contents to files.
*   4: Graph trace. Show IVGraph, PPEGraph and Line Graph.
*   Each flag can be separately controlled.
*   isTrace() returns true if trace level is greater than 0.
*/
bool isTrace();
bool isTrace(int type);
inline bool isLocationTrace() { return isTrace(TRACE_LOCATION); }
inline bool isScriptTrace() { return isTrace(TRACE_SCRIPT); }
inline bool isStorageTrace() { return isTrace(TRACE_STORAGE); }
inline bool isProcessTrace() { return isTrace(TRACE_PROCESS); }
void setTrace(int type, bool isTrace);
void setTrace(const std::string& bits);
void upgradeTraceLevel();

std::ostream& getTraceStream();
void setTraceStream(std::ostream* os);

namespace Trace {

bool initialize(const Configuration& conf);

void putErrorLog(const std::string& message);
void putWarnLog(const std::string& message);
void putInfoLog(const std::string& message);
void putDebugLog(const std::string& message);
void putVerboseLog(const std::string& message);

void putLog(FILE* console, FILE* file, const char* format, ...);
extern FILE* logfile;

}

}//end of namespace Garnet

// User information
//   Error:   Unrecoverable error.
//   Warn:    Recoverable error.
//   Info:    Useful information for users. ex. possible actions to improve results, time, etc.
// Developer information
//   Debug:   Debug information.
//   Vrebose: Any other notes.

#define GLOG__(stream, keyword, format, ...) Garnet::Trace::putLog(stream, Garnet::Trace::logfile, keyword "[%s:%d]" format, __FILE__, __LINE__, __VA_ARGS__)

#if defined(_NDEBUG)
#define ELOG(...)  GLOG__(stderr, "(E)", __VA_ARGS__, "")
#define WLOG(...)  GLOG__(stderr, "(W)", __VA_ARGS__, "")
#define ILOG(...)  GLOG__(stdout, "(I)", __VA_ARGS__, "")
#define DLOG(...)  GLOG__(stdout, "(D)", __VA_ARGS__, "")
#define VLOG(...)  GLOG__(stdout, "(V)", __VA_ARGS__, "")
#else
#define ELOG(...)  GLOG__(stderr, "(E)", __VA_ARGS__, "")
#define WLOG(...)  GLOG__(stderr, "(W)", __VA_ARGS__, "")
#define ILOG(...)  GLOG__(stdout, "(I)", __VA_ARGS__, "")
#define DLOG(...)  GLOG__(NULL,        "(D)", __VA_ARGS__, "")
#define VLOG(...)
#endif

#endif//GARNET__TRACE_LEVEL__H__
