#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "SmartAssert.h"
#include "../Source/Common/b2Math.h"

/// @name Global basic types. Only these should be used.
//@{
typedef signed char	int8;
typedef signed short int16;
typedef signed int int32;
typedef signed __int64 int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned __int64 uint64;
typedef float float32;
typedef double float64;
#define	FLOAT32_MAX	FLT_MAX
#define	FLOAT32_MIN	FLT_MIN
#define	FLOAT32_EPSILON	FLT_EPSILON
typedef b2Vec2 Vector2;
extern const Vector2 Vector2_Zero;
extern Vector2 Vector2_Dummy;
typedef b2Mat22 Matrix22;
extern const Matrix22 Matrix22_Identity;
typedef b2XForm XForm;
//@}

/// @name A little hack for the property system - I need to define this type somewhere else instead inside the system.
//@{
typedef uint8 PropertyAccessFlags;
#define FULL_PROPERTY_ACCESS_FLAGS 0xff
//@}

/// @name Allocation.
#define DYN_NEW new
#define DYN_NEW_T(T) new T
#define DYN_DELETE delete
#define DYN_DELETE_ARRAY delete[]


/// @name Math.
#include "MathUtils.h"


/// @name Streams.
#include <string>
#include <ios>
typedef std::string string;
typedef std::istream InputStream;
enum eInputStreamMode { ISM_TEXT, ISM_BINARY };
inline std::ios_base::openmode InputStreamMode(eInputStreamMode mode)
{
	switch (mode)
	{
	case ISM_TEXT:
		return std::ios_base::in;
	case ISM_BINARY:
		return std::ios_base::in | std::ios_base::binary;
	}
	return std::ios_base::in | std::ios_base::binary;
}


#endif