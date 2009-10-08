/// @file
/// Platform specific settings of common data types and operators needed in the project.

#ifndef _SETTINGS_H_
#define _SETTINGS_H_


#define BOOST_NO_TYPEID


// These must come first!
#include "SmartAssert.h"
#include "Memory_pre.h"


#include "BasicTypes.h"
#include "ComplexTypes.h"
#include "MathConsts.h"


// These must come last!
#include "Memory_post.h"


namespace Utils {}
using namespace Utils;
namespace Reflection {}
using namespace Reflection;


#endif