/// @file
/// Definition of all types a value editor can have.

#ifndef _EDITOR_VALUEEDITORTYPES_H_
#define _EDITOR_VALUEEDITORTYPES_H_

namespace Editor
{
	/// Types a value editor can have.
	enum eValueEditorType
	{
		VET_PT_BOOL = 0, ///< Corresponds to BoolEditor.
		VET_PT_POINT, ///< Corresponds to PointEditor.
		VET_PT_RESOURCE, ///< Corresponds to ResourceEditor.
		VET_PT_STRING, ///< Corresponds to StringEditor.
		VET_PT_VECTOR2, ///< Corresponds to Vector2Editor.
		VET_PT_PROTOTYPE, ///< Corresponds to PrototypeEditor.

		// Array editors:
		#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) VET_##typeID##_ARRAY,
		#include "Utils/Properties/PropertyTypes.h"
		#undef PROPERTY_TYPE

		NUM_VALUE_EDITOR_TYPES ///< Total number of types.
	};
}

#endif // _EDITOR_VALUEEDITORTYPES_H_