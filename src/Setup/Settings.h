#ifndef _SETTINGS_H_
#define _SETTINGS_H_


#include "Memory.h"
#include "SmartAssert.h"
#include "BasicTypes.h"
#include "ComplexTypes.h"
#include "MathUtils.h"

//TODO a proc ze tohle nemuze bejt v PropertyTypes.h ?
/// @name A little hack for the property system - I need to define this type somewhere else instead inside the system.
//@{
typedef uint8 PropertyAccessFlags;
#define FULL_PROPERTY_ACCESS_FLAGS 0xff
//@}


#endif