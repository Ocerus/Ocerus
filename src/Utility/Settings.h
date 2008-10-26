#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <assert.h>

typedef signed char	int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;
typedef float float32;
#define	FLOAT32_MAX	FLT_MAX
#define	FLOAT32_MIN	FLT_MIN
#define	FLOAT32_EPSILON	FLT_EPSILON

#define DYN_NEW new
#define DYN_NEW_T(T) new T
#define DYN_DELETE delete
#define DYN_DELETE_ARRAY delete[]

#include <string>
#include <ios>
typedef std::string string;
typedef std::istream InputStream;

#endif