/// @file
/// Platform specific setting of basic (simple) data types.

#ifndef BasicTypes_h__
#define BasicTypes_h__

#include <math.h>
#include <limits.h>
#include <boost/cstdint.hpp>

typedef boost::int8_t int8;
typedef boost::int16_t int16;
typedef boost::int32_t int32;
typedef boost::uint8_t uint8;
typedef boost::uint16_t uint16;
typedef boost::uint32_t uint32;
typedef float float32;
typedef double float64;
typedef uintptr_t uintptr;
#define	FLOAT32_MAX	FLT_MAX
#define	FLOAT32_MIN	FLT_MIN
#define	FLOAT32_EPSILON	FLT_EPSILON
#define INT32_MAX INT_MAX
#define INT32_MIN INT_MIN

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 uint64;
typedef signed __int64 int64;
#else
typedef unsigned long long int uint64;
typedef signed long long int int64;
#endif

#endif // BasicTypes_h__
