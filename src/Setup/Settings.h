/// @file
/// Platform specific settings of common data types and operators needed in the project.

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "Platform.h"

// Disable boost exceptions.
#define BOOST_NO_TYPEID

// We need to link CEGUI static to overload it's operator new.
#define CEGUI_STATIC

// These must come first!
#include "../Utils/SmartAssert.h"
#include "Memory_pre.h"



#include "BasicTypes.h"
#include "ComplexTypes.h"
#include "../Utils/MathConsts.h"

template<typename T, typename U>
inline void swap(T& first, U& second) { std::swap(first, second); } 




// These must come last!
#include "Memory_post.h"


namespace Utils {}
using namespace Utils;
namespace Reflection {}
using namespace Reflection;
namespace LogSystem {}
using namespace LogSystem;


#endif
