#ifndef _SETTINGS_H_
#define _SETTINGS_H_

typedef signed char	int8;
typedef signed short int16;
typedef signed int int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef float float32;
#define	FLOAT32_MAX	FLT_MAX
#define	FLOAT32_MIN	FLT_MIN
#define	FLOAT32_EPSILON	FLT_EPSILON

#define NEW new
#define NEW_T(T) new T
#define DELETE delete
#define DELETE_ARRAY delete[]

#include <string>
#include <ios>
typedef std::string string;
typedef std::istream istream;

#include <vector>
#include <map>
// hack cos the compiler can't typedef templates
#define vector std::vector
#define map std::map

#endif