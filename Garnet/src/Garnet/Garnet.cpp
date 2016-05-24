
#include <Garnet/Garnet.h>
#include <Common/Synchronize.h>

namespace {
	// Global configurations.
	Mutex _globalMutex;
	bool  _isCompressionEnabled = true;
}

void Garnet::Settings::load(const Configuration& conf)
{
	_isCompressionEnabled = conf.isCompressionEnabled;
}

bool Garnet::Settings::isCompressionEnabled()
{
	return _isCompressionEnabled;
}
