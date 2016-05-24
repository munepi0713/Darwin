#ifndef RANDOM_GENERATOR__H__
#define RANDOM_GENERATOR__H__

#include <stdint.h>
#include <limits>

#if defined(max)
#undef max
#endif

namespace Garnet
{

class RandomGenerator
{
public :
	/** Generator type */
	const enum Type {
		NOT_CHANGE,
		C,
		MT19937,
	};

	virtual ~RandomGenerator() = 0;

    /** Initialize random generator with a seed. */
    virtual void initialize(int64_t seed) = 0;
        
    /** Generate a real number on the range [0, 1). */
    virtual double generate() = 0;

    /** Generate an integer number on the range [0, maximum). */
    virtual int generateInteger(const int maximum) = 0;

	/** Create an instance of specified random generator. */
	static RandomGenerator* create(Type type);

protected :
	RandomGenerator() {}
};

void   srand(RandomGenerator::Type type, int64_t seed);
double rand01();
int    randInteger(int maximum = std::numeric_limits<int>::max());

}//end of namespace Garnet


#endif//RANDOM_GENERATOR__H__
