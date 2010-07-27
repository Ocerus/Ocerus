#include "Common.h"
#include "PropertyEditorCreator.h"
#include "Editor/EditorMgr.h"

// Models
#include "Models/EntityAttributeModel.h"
#include "Models/PropertyModel.h"

// Editors
#include "AbstractValueEditor.h"
#include "StringEditor.h"
#include "BoolEditor.h"
#include "Vector2Editor.h"
#include "PointEditor.h"
#include "ArrayEditor.h"
#include "ResourceEditor.h"
#include "LayerEditor.h"

namespace Editor
{
	AbstractValueEditor* CreatePropertyEditor(const PropertyHolder& property)
	{
		PROFILE_FNC();

		switch (property.GetType())
		{

		// We generate cases for all Array types here. ArrayEditor is used for such properties.
		#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) case typeID##_ARRAY: \
			return new Editor::ArrayEditor<typeClass>(new PropertyModel<Array<typeClass>*>(property));
		#include "Utils/Properties/PropertyTypes.h"
		#undef PROPERTY_TYPE

		case PT_BOOL:
		  return new Editor::BoolEditor(new PropertyModel<bool>(property));
		case PT_INT32:
			if (string(property.GetName()) == "Layer")
				return new Editor::LayerEditor(new PropertyModel<EntitySystem::LayerID>(property));
			else
				return new Editor::StringEditor(new StringPropertyModel(property));
			
		case PT_VECTOR2:
			return new Editor::Vector2Editor(new PropertyModel<Vector2>(property));
		case PT_POINT:
			return new Editor::PointEditor(new PropertyModel<GfxSystem::Point>(property));
		case PT_RESOURCE:
			return new Editor::ResourceEditor(new PropertyModel<ResourceSystem::ResourcePtr>(property));
		default:
			return new Editor::StringEditor(new StringPropertyModel(property));
		}		
	}

	AbstractValueEditor* CreateEntityIDEditor(const EntitySystem::EntityHandle& entity)
	{
		return new Editor::StringEditor(new EntityAttributeModel(entity, Editor::EntityAttributeModel::TYPE_ID));
	}

	AbstractValueEditor* CreateEntityNameEditor(const EntitySystem::EntityHandle& entity)
	{
		return new Editor::StringEditor(new EntityAttributeModel(entity, Editor::EntityAttributeModel::TYPE_NAME));
	}

	AbstractValueEditor* CreateEntityTagEditor( const EntitySystem::EntityHandle& entity )
	{
		return new Editor::StringEditor(new EntityAttributeModel(entity, Editor::EntityAttributeModel::TYPE_TAG));
	}
}

// Template implementations
#include "ArrayEditorImpl.h"
#include "Models/ArrayElementModelImpl.h"
