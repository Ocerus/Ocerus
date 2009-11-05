/// @file
/// Here are definitions of types for all properties in the system. The macro used for definition is inside PropertyEnums.h.

PROPERTY_TYPE(PT_BOOL, bool, false, "bool")
PROPERTY_TYPE(PT_INT8, int8, 0, "int8")
PROPERTY_TYPE(PT_INT16, int16, 0, "int16")
PROPERTY_TYPE(PT_INT32, int32, 0, "int32")
PROPERTY_TYPE(PT_INT64, int64, 0, "int64")
PROPERTY_TYPE(PT_UINT8, uint8, 0, "uint8")
PROPERTY_TYPE(PT_UINT16, uint16, 0, "uint16")
PROPERTY_TYPE(PT_UINT32, uint32, 0, "uint32")
PROPERTY_TYPE(PT_UINT64, uint64, 0, "uint64")
PROPERTY_TYPE(PT_FLOAT32, float32, 0.0f, "float")
#ifndef SCRIPT_ONLY
PROPERTY_TYPE(PT_VECTOR2, Vector2, Vector2_Zero, "Vector2")
PROPERTY_TYPE(PT_VECTOR2_ARRAY, Vector2*, 0, "Vector2[]")
PROPERTY_TYPE(PT_STRING, char*, 0, "string")
PROPERTY_TYPE(PT_STRING_KEY, Utils::StringKey, Utils::StringKey::Null, "StringKey")
PROPERTY_TYPE(PT_ENTITYHANDLE, EntitySystem::EntityHandle, EntitySystem::EntityHandle::Null, "EntityHandle")
PROPERTY_TYPE(PT_ENTITYHANDLE_ARRAY, EntitySystem::EntityHandle*, 0, "EntityHandle[]")
PROPERTY_TYPE(PT_COLOR, GfxSystem::Color, GfxSystem::Color::NullColor, "Color")
PROPERTY_TYPE(PT_FUNCTION_PARAMETER, PropertyFunctionParameters, 0, "PropertyFunctionParameters")
#endif

