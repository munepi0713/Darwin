#ifndef COMPILER__H__
#define COMPILER__H__

#include <cstring>

// Common interfaces for compiler-dependent functions.

/**
 *  STRTOULL
 *    64bit integer version of strtoul.
 */
#if defined(__GNUC__)
    #define STRTOULL(str, base, pos)  strtoull((str), (base), (pos))
#else
    #define STRTOULL(str, base, pos)  _strtoui64((str), (base), (pos))
#endif


#endif//COMPILER__H__
