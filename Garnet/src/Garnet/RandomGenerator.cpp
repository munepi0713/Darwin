#include <Garnet/RandomGenerator.h>

#include <math.h>
#include <stdlib.h>

#include <boost/format.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace impl {

class CLib : public Garnet::RandomGenerator
{
public :
    void   initialize(int64_t seed) {
        srand((unsigned int)seed);
    }
        
    double generate() {
        double ret = (double)rand() / RAND_MAX;
        return ret;
    }

    int generateInteger(const int maximum) {
        int ret;
        do {
            ret = rand();
        } while ( ret >= maximum );
        return ret;
    }
};


class MT19937 : public Garnet::RandomGenerator
{
private :
	boost::random::mt19937  _gen;

public :
        
    void   initialize(int64_t seed) {
        _gen.seed(static_cast<uint32_t>(seed));
    }
       
	// Return [0, 1)
    double generate() {
		return boost::random::uniform_01<double>()(_gen);
    }

    int generateInteger(const int maximum) {
		return boost::random::uniform_int_distribution<int>(0, maximum - 1)(_gen);
    }
};

}

Garnet::RandomGenerator::~RandomGenerator()
{
}

Garnet::RandomGenerator* Garnet::RandomGenerator::create(Type type)
{
	switch ( type ) {
	case C :
		return new impl::CLib();
	case MT19937 :
		return new impl::MT19937();
	default :
		return 0;
	}
}


namespace impl {
Garnet::RandomGenerator* _randGenInstance = Garnet::RandomGenerator::create(Garnet::RandomGenerator::MT19937);
}

void  Garnet::srand(Garnet::RandomGenerator::Type type, int64_t seed)
{
	if ( type != Garnet::RandomGenerator::NOT_CHANGE ) {
		delete impl::_randGenInstance;
		impl::_randGenInstance = Garnet::RandomGenerator::create(type);
	}
	impl::_randGenInstance->initialize(seed);
}
double Garnet::rand01()
{
	return impl::_randGenInstance->generate();
}
int    Garnet::randInteger(int maximum)
{
	return impl::_randGenInstance->generateInteger(maximum);
}
