/// @file
/// Platform specific settings of common data types and operators needed in the project.

#ifndef _SETTINGS_H_
#define _SETTINGS_H_


#define BOOST_NO_TYPEID

#include "SmartAssert.h"
#include "Memory.h"
#include "BasicTypes.h"
#include "ComplexTypes.h"

#include "MathUtils.h"

namespace Utils {}
using namespace Utils;
namespace Reflection {}
using namespace Reflection;

//TODO a proc ze tohle nemuze bejt v PropertyTypes.h ?
/// A little hack for the property system - I need to define this type somewhere else instead inside the system.
typedef uint8 PropertyAccessFlags;
#define FULL_PROPERTY_ACCESS_FLAGS 0xff


/// @brief We are disabling malloc and free as it's an old C-style way of allocating memory.
/// @remarks We must define these macros here to prevent conflicts with standard libraries included before this line.
#define malloc MALLOC_IS_DISABLED
#define free FREE_IS_DISABLED


#endif