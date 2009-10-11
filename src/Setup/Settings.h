/// @file
/// Platform specific settings of common data types and operators needed in the project.

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#ifdef _WIN32
    #define __WIN__
#else
    #define __UNIX__
#endif


#define BOOST_NO_TYPEID


// These must come first!
#include "SmartAssert.h"
#include "Memory_pre.h"




#include "BasicTypes.h"
#include "ComplexTypes.h"
#include "MathConsts.h"

template<typename T, typename U>
inline void swap(T& first, U& second) { std::swap(first, second); } 




// These must come last!
#include "Memory_post.h"


namespace Utils {}
using namespace Utils;
namespace Reflection {}
using namespace Reflection;


#endif
