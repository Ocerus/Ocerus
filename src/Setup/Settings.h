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
//TODO neodstranit mathutils odtud?
#include "MathUtils.h"


// These must come last!
#include "Memory_post.h"


namespace Utils {}
using namespace Utils;
namespace Reflection {}
using namespace Reflection;


//TODO a proc ze tohle nemuze bejt v PropertyTypes.h ?
/// A little hack for the property system - I need to define this type somewhere else instead inside the system.
typedef uint8 PropertyAccessFlags;
#define FULL_PROPERTY_ACCESS_FLAGS 0xff


#endif