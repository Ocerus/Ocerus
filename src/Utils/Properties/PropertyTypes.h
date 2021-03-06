/// @file
/// Here are definitions of types for all properties in the system.
/// @remarks
/// The format of the type definition is as following:
///		enum type ID, C++ type, null value, script name, script type, cloning
/// Note that array types are not presented here. Instead, they are generated automatically. As an example imagine type
/// PROPERTY_TYPE(PT_BOOL, bool, false, "bool", Byte, SHALLOW)
/// The corresponding array would be
/// PROPERTY_TYPE(PT_BOOL_ARRAY, Array<bool>*, 0, "Array<bool>*", Object, SHALLOW)

PROPERTY_TYPE(PT_BOOL, bool, false, "bool", Byte, SHALLOW)
PROPERTY_TYPE(PT_INT8, int8, 0, "int8", Byte, SHALLOW)
PROPERTY_TYPE(PT_INT16, int16, 0, "int16", Word, SHALLOW)
PROPERTY_TYPE(PT_INT32, int32, 0, "int32", DWord, SHALLOW)
PROPERTY_TYPE(PT_INT64, int64, 0, "int64", QWord, SHALLOW)
PROPERTY_TYPE(PT_UINT8, uint8, 0, "uint8", Byte, SHALLOW)
PROPERTY_TYPE(PT_UINT16, uint16, 0, "uint16", Word, SHALLOW)
PROPERTY_TYPE(PT_UINT32, uint32, 0, "uint32", DWord, SHALLOW)
PROPERTY_TYPE(PT_UINT64, uint64, 0, "uint64", QWord, SHALLOW)
PROPERTY_TYPE(PT_FLOAT32, float32, 0.0f, "float32", Float, SHALLOW)
PROPERTY_TYPE(PT_VECTOR2, Vector2, Vector2_Zero, "Vector2", Object, SHALLOW)
PROPERTY_TYPE(PT_POINT, GfxSystem::Point, GfxSystem::Point::Point_Zero, "Point", Object, SHALLOW)
PROPERTY_TYPE(PT_STRING, string, string(), "string", Object, SHALLOW)
PROPERTY_TYPE(PT_STRING_KEY, Utils::StringKey, Utils::StringKey::Null, "StringKey", Object, SHALLOW)
PROPERTY_TYPE(PT_ENTITYHANDLE, EntitySystem::EntityHandle, EntitySystem::EntityHandle::Null, "EntityHandle", Object, SHALLOW)
PROPERTY_TYPE(PT_COLOR, GfxSystem::Color, GfxSystem::Color::Null, "Color", Object, SHALLOW)
PROPERTY_TYPE(PT_KEYCODE, InputSystem::eKeyCode, InputSystem::KC_UNASSIGNED, "eKeyCode", DWord, SHALLOW)
#ifndef SCRIPT_ONLY
PROPERTY_TYPE(PT_RESOURCE, ResourceSystem::ResourcePtr, 0, "ResourcePtr", Object, SHALLOW)
PROPERTY_TYPE(PT_FUNCTION_PARAMETER, PropertyFunctionParameters, PropertyFunctionParameters::Null, "PropertyFunctionParameters", Object, SHALLOW)
PROPERTY_TYPE(PT_PHYSICAL_BODY, PhysicalBody*, 0, "PhysicalBody", Object, DEEP)
PROPERTY_TYPE(PT_PHYSICAL_SHAPE, PhysicalShape*, 0, "PhysicalShape", Object, DEEP)
#endif

