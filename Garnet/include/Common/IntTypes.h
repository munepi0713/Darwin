#ifndef GARNET__INT_TYPES__H__
#define GARNET__INT_TYPES__H__

#ifdef _MSC_VER
typedef unsigned __int16  uint16_t;
typedef unsigned __int32  uint32_t;
typedef unsigned __int64  uint64_t;
typedef __int16  int16_t;
typedef __int32  int32_t;
typedef __int64  int64_t;
#define PRId64   "I64d"
#define PRIu64   "I64u"
#define PRIx64   "I64x"
#else
#include <stdint.h>
#include <inttypes.h>
#endif

#endif//GARNET__INT_TYPES__H__
