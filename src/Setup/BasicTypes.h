/// @file
/// Platform specific setting of basic (simple) data types.

#ifndef BasicTypes_h__
#define BasicTypes_h__

#include <math.h>
#include <limits.h>
typedef signed char	int8;
typedef signed short int16;
typedef signed int int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef float float32;
typedef double float64;
#define	FLOAT32_MAX	FLT_MAX
#define	FLOAT32_MIN	FLT_MIN
#define	FLOAT32_EPSILON	FLT_EPSILON
#define INT32_MAX INT_MAX
#define INT32_MIN INT_MIN

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 uint64;
typedef signed __int64 int64;
#else
typedef unsigned long long uint64;
typedef signed long long int64;
#endif

#endif // BasicTypes_h__
