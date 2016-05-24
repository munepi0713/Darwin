//
//  Copyright(c) 2009-2014 Munehiro Doi
//

#include <Garnet/TraceLevel.h>

#include <bitset>

namespace {
static std::bitset<4> _traceFlags;
static std::ostream* _traceStream = &std::cerr;
}

bool  Garnet::isTrace()
{
	return _traceFlags.any();
}

bool  Garnet::isTrace(int type)
{
	return _traceFlags[type];
}

void  Garnet::setTrace(int type, bool isTrace)
{
	_traceFlags[type] = isTrace;
}

void  Garnet::setTrace(const std::string& bits)
{
	_traceFlags = std::bitset<4>(bits);
}

void  Garnet::upgradeTraceLevel()
{
	int level = (int)_traceFlags.to_ullong();
	if      ( level < 0x1 ) level = 0x1;
	else if ( level < 0x2 ) level = 0x3;
	else if ( level < 0x4 ) level = 0x7;
	else if ( level < 0x8 ) level = 0xF;
	_traceFlags = std::bitset<4>(level);
}


std::ostream& Garnet::getTraceStream()
{
	return *_traceStream;
}

void Garnet::setTraceStream(std::ostream* os)
{
	if ( os == 0 ) {
		_traceStream = &std::cerr;
	}
	else {
		_traceStream = os;
	}
}

FILE* Garnet::Trace::logfile = NULL;

void Garnet::Trace::putLog(FILE* console, FILE* file, const char* format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);

    if (console) {
        fputc('\n', console);
        fputs(buf, console);
    }
    if (file) {
        fputc('\n', file);
        fputs(buf, file);
    }
}
