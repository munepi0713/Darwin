/*********************************************************
 *  Garnet
 *  Author: Munehiro Doi.
 *********************************************************/
#ifndef GARNET__STATISTICS__H__
#define GARNET__STATISTICS__H__

#include <numeric>
#include <stdint.h>

#if defined(min)
#undef min
#undef max
#endif

/**
 *
 */
class Statistics
{
public :
	Statistics() {
		_average = 0;
		_sqsum   = 0;
		_count   = 0;
		_max     = -std::numeric_limits<double>::max();
		_min     =  std::numeric_limits<double>::max();
	}
	void   put(double value) {
		// update min/max
		if ( value < _min ) _min = value;
		if ( value > _max ) _max = value;
		// update average/stdev
		_count++;
		value -= _average;
		_average += value / _count;
		_sqsum += (_count - 1) * value * value / _count;
	}
	template <typename It>
	void   put(It first, It last) {
		for (It it = first; it != last; it++) {
			double value = *it;
			// update min/max
			if ( value > _max ) _max = value;
			if ( value < _min ) _min = value;
			// update average/stdev
			_count++;
			value -= _average;
			_average += value / _count;
			_sqsum += (_count - 1) * value * value / _count;
		}
	}
	double getMax() const {
		return _max;
	}
	double getMin() const {
		return _min;
	}
	double getTotal() const {
		return _average * _count;
	}
	uint32_t getCount() const {
		return _count;
	}
	double getAverage() const {
		return _average;
	}
	double getStdev() const {
		return _count > 0 ? sqrt(_sqsum / (_count - 1)) : 0;
	}
private :
	double   _average;
	double   _sqsum;
	double   _max;
	double   _min;
	uint32_t _count;
};

#endif//GARNET__STATISTICS__H__
