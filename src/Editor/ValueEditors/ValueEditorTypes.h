#ifndef _EDITOR_VALUEEDITORTYPES_H_
#define _EDITOR_VALUEEDITORTYPES_H_

namespace Editor
{
	enum eValueEditorType
	{
		VET_PT_BOOL = 0,
		VET_PT_POINT,
		VET_PT_RESOURCE,
		VET_PT_STRING,
		VET_PT_VECTOR2,
		VET_PT_PROTOTYPE,

		// Array editors:
		#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) VET_##typeID##_ARRAY,
		#include "Utils/Properties/PropertyTypes.h"
		#undef PROPERTY_TYPE

		NUM_VALUE_EDITOR_TYPES
	};
}

#endif // _EDITOR_VALUEEDITORTYPES_H_