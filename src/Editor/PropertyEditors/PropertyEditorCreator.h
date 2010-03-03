/// @file
/// Defines property editors for different property types.

#ifndef _PROPERTYEDITORCREATOR_H_
#define _PROPERTYEDITORCREATOR_H_

#include "Base.h"
#include "AbstractPropertyEditor.h"

namespace Editor
{
	AbstractPropertyEditor* CreatePropertyEditor(const PropertyHolder&);
}

#endif // _PROPERTYEDITORCREATOR_H_
