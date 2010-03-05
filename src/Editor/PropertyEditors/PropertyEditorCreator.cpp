#include "Common.h"
#include "PropertyEditorCreator.h"
#include "EntityAttributeModel.h"
#include "AbstractValueEditor.h"
#include "StringEditor.h"
#include "ArrayStringEditor.h"


using namespace Editor;

IValueEditor* Editor::CreatePropertyEditor(const PropertyHolder& property)
{
	switch (property.GetType())
	{

	// We generate cases for all Array types here. ArrayStringEditor is used for such properties.
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter) case typeID##_ARRAY: \
	return new Editor::ArrayStringEditor<PropertyHolder, typeClass>(property);
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	default:
		// For everything else use StringEditor.
		return new Editor::StringEditor<PropertyHolder>(property);
	}
	return 0;
}

Editor::IValueEditor* Editor::CreateEntityIDEditor(const EntitySystem::EntityHandle& entity)
{
	return new Editor::StringEditor<EntityAttributeModel>(EntityAttributeModel(entity, Editor::EntityAttributeModel::TYPE_ID));
}

Editor::IValueEditor* Editor::CreateEntityNameEditor(const EntitySystem::EntityHandle& entity)
{
	return new Editor::StringEditor<EntityAttributeModel>(EntityAttributeModel(entity, Editor::EntityAttributeModel::TYPE_NAME));
}
