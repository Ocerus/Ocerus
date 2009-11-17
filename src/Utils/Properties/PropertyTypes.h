/// @file
/// Here are definitions of types for all properties in the system. The macro used for definition is inside PropertyEnums.h.
/// @remarks
/// Note that array types are not presented here. Instead, they are generated automatically. As an example imagine type
/// PROPERTY_TYPE(PT_BOOL, bool, false, "bool")
/// The corresponding array would be
/// PROPERTY_TYPE(PT_BOOL_ARRAY, Array<bool>*, 0, "Array<bool>*")

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
PROPERTY_TYPE(PT_VECTOR2, Vector2, Vector2_Zero, "Vector2")
PROPERTY_TYPE(PT_STRING, string, 0, "string")
PROPERTY_TYPE(PT_STRING_KEY, Utils::StringKey, Utils::StringKey::Null, "StringKey")
PROPERTY_TYPE(PT_ENTITYHANDLE, EntitySystem::EntityHandle, EntitySystem::EntityHandle::Null, "EntityHandle")
PROPERTY_TYPE(PT_COLOR, GfxSystem::Color, GfxSystem::Color::NullColor, "Color")
#ifndef SCRIPT_ONLY
PROPERTY_TYPE(PT_BOOL_ARRAY, Array<bool>*, 0, "Array<bool>*")
PROPERTY_TYPE(PT_INT8_ARRAY, Array<int8>*, 0, "Array<int8>*")
PROPERTY_TYPE(PT_INT16_ARRAY, Array<int16>*, 0, "Array<int16>*")
PROPERTY_TYPE(PT_INT32_ARRAY, Array<int32>*, 0, "Array<int32>*")
PROPERTY_TYPE(PT_INT64_ARRAY, Array<int64>*, 0, "Array<int64>*")
PROPERTY_TYPE(PT_UINT8_ARRAY, Array<uint8>*, 0, "Array<uint8>*")
PROPERTY_TYPE(PT_UINT16_ARRAY, Array<uint16>*, 0, "Array<uint16>*")
PROPERTY_TYPE(PT_UINT32_ARRAY, Array<uint32>*, 0, "Array<uint32>*")
PROPERTY_TYPE(PT_UINT64_ARRAY, Array<uint64>*, 0, "Array<uint64>*")
PROPERTY_TYPE(PT_FLOAT32_ARRAY, Array<float32>*, 0, "Array<float>*")
PROPERTY_TYPE(PT_VECTOR2_ARRAY, Array<Vector2>*, 0, "Array<Vector2>*")
PROPERTY_TYPE(PT_STRING_ARRAY, Array<string>*, 0, "Array<string>*")
PROPERTY_TYPE(PT_STRING_KEY_ARRAY, Array<Utils::StringKey>*, 0, "Array<StringKey>*")
PROPERTY_TYPE(PT_ENTITYHANDLE_ARRAY, Array<EntitySystem::EntityHandle>*, 0, "Array<EntityHandle>*")
PROPERTY_TYPE(PT_COLOR_ARRAY, Array<GfxSystem::Color>*, 0, "Array<Color>*")
PROPERTY_TYPE(PT_FUNCTION_PARAMETER, PropertyFunctionParameters, 0, "PropertyFunctionParameters")
#endif

