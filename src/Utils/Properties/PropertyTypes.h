/// @file
/// Here are definitions of types for all properties in the system. The macro used for definition is inside PropertyEnums.h.

PROPERTY_TYPE(PT_BOOL, bool, false)
PROPERTY_TYPE(PT_INT8, int8, 0)
PROPERTY_TYPE(PT_INT16, int16, 0)
PROPERTY_TYPE(PT_INT32, int32, 0)
PROPERTY_TYPE(PT_INT64, int64, 0)
PROPERTY_TYPE(PT_UINT8, uint8, 0)
PROPERTY_TYPE(PT_UINT16, uint16, 0)
PROPERTY_TYPE(PT_UINT32, uint32, 0)
PROPERTY_TYPE(PT_UINT64, uint64, 0)
PROPERTY_TYPE(PT_FLOAT32, float32, 0.0f)
PROPERTY_TYPE(PT_VECTOR2, Vector2, Vector2_Zero)
PROPERTY_TYPE(PT_VECTOR2_ARRAY, Vector2*, 0)
PROPERTY_TYPE(PT_STRING, char*, 0)
PROPERTY_TYPE(PT_STRING_KEY, Utils::StringKey, Utils::StringKey::Null)
PROPERTY_TYPE(PT_ENTITYHANDLE, EntitySystem::EntityHandle, EntitySystem::EntityHandle::Null)
PROPERTY_TYPE(PT_ENTITYHANDLE_ARRAY, EntitySystem::EntityHandle*, 0)
PROPERTY_TYPE(PT_COLOR, GfxSystem::Color, GfxSystem::Color::NullColor)
PROPERTY_TYPE(PT_FUNCTION_PARAMETER, PropertyFunctionParameters, 0)
	

