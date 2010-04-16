/// @file
/// Platform specific setting of basic (primitive) data types.

#ifndef BasicTypes_h__
#define BasicTypes_h__

#include <cmath>
#include <climits>
#include <stddef.h>
#include <boost/cstdint.hpp>
#include <boost/limits.hpp>

typedef boost::int8_t int8;
typedef boost::int16_t int16;
typedef boost::int32_t int32;
typedef boost::int64_t int64;

typedef boost::uint8_t uint8;
typedef boost::uint16_t uint16;
typedef boost::uint32_t uint32;
typedef boost::uint64_t uint64;

/// @name Floating point number types
/// @deprecated These typedefs are deprecated as there is no such thing as float32 and float64. We should use float, double and long double instead.
//@{
	typedef float float32;
	typedef double float64;
//@}

typedef uintptr_t uintptr;
typedef std::size_t size_t;

/// Template structure that holds numeric limits.
template<typename T>
struct numeric_limits: public std::numeric_limits<T> {};

#endif // BasicTypes_h__
