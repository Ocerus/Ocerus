#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <assert.h>

/// Global basic types. Only these should be used.
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
#define	FLOAT32_MAX	FLT_MAX
#define	FLOAT32_MIN	FLT_MIN
#define	FLOAT32_EPSILON	FLT_EPSILON
//@}

#include "../Source/Common/b2Math.h"

typedef b2Vec2 Vector2;

#define DYN_NEW new
#define DYN_NEW_T(T) new T
#define DYN_DELETE delete
#define DYN_DELETE_ARRAY delete[]

#include <string>
#include <ios>
//TODO zmenit na math.h a double nahradit float32
#include <cmath>

inline uint32 round(double num) {
  return static_cast<uint32>(floor(num+0.5));
}

inline uint32 round(float32 num) {
  return static_cast<uint32>(floor(num+0.5));
}

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