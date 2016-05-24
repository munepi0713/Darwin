#ifndef COMMON__MATHEMATICAL__H__
#define COMMON__MATHEMATICAL__H__


#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <functional>
#include <cstdint>

namespace Mathematical {

/**
 * Select K numbers from N numbers. where size < N.
 * This is based on Reservoir sampling algorithm.
 * http://en.wikipedia.org/wiki/Reservoir_sampling
 *
 * @param[in] random  A function random(max) which returns integer value between [0, max)
 */
template <typename Int, typename Size, typename Rand>
std::vector<Int>  selectN(const std::vector<Int>& source, Size K, Rand random)
{
	std::vector<Int> ret(K);
	Size N = source.size();
	Size i = 0u;
	for (; i < K; i++) {
		ret[i] = source[i];
	}
	for (; i < N; i++) {
		Size j = random(i);
		if (j < K) {
			ret[j] = source[i];
		}
	}
	return ret;
}

/**
 *  Select K numbers from [0:N) integer numbers.
 */
template <typename Int, typename Size, typename Rand>
std::vector<Int>  selectN(Int N, Size K, Rand random)
{
	std::vector<Int> source(N);
	for (Int i = 0; i < N; i++) {
		source[i] = i;
	}
	return selectN(source, K, random);
}

}//end of namespace Mathematical

#endif//COMMON__MATHEMATICAL__H__
