#include "Common.h"
#include "PropertyEditorCreator.h"
#include "EntityAttributeModel.h"
#include "AbstractValueEditor.h"
#include "StringEditor.h"
#include "Vector2Editor.h"
//#include "ArrayStringEditor.h"


using namespace Editor;

IValueEditor* Editor::CreatePropertyEditor(const PropertyHolder& property)
{
	switch (property.GetType())
	{
/*
	// We generate cases for all Array types here. ArrayStringEditor is used for such properties.
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) case typeID##_ARRAY: \
	return new Editor::ArrayStringEditor<PropertyHolder, typeClass>(property);
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE
*/
	case PT_VECTOR2:
		return new Editor::Vector2Editor<PropertyHolder>(property);
	default:
		break;
	}
	// For everything else use StringEditor.
	return new Editor::StringEditor<PropertyHolder>(property);
}

Editor::IValueEditor* Editor::CreateEntityIDEditor(const EntitySystem::EntityHandle& entity)
{
	return new Editor::StringEditor<EntityAttributeModel>(EntityAttributeModel(entity, Editor::EntityAttributeModel::TYPE_ID));
}

Editor::IValueEditor* Editor::CreateEntityNameEditor(const EntitySystem::EntityHandle& entity)
{
	return new Editor::StringEditor<EntityAttributeModel>(EntityAttributeModel(entity, Editor::EntityAttributeModel::TYPE_NAME));
}
