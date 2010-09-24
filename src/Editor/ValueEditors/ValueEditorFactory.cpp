#include "ValueEditorFactory.h"

#include "Editor/ValueEditors/ArrayEditor.h"
#include "Editor/ValueEditors/BoolEditor.h"
#include "Editor/ValueEditors/StringEditor.h"
#include "Editor/ValueEditors/ResourceEditor.h"
#include "Editor/ValueEditors/TwoDimEditor.h"

#include "Editor/ValueEditors/Models/PropertyModel.h"
#include "Editor/ValueEditors/Models/StringValueEditorModel.h"
#include "GUISystem/CEGUICommon.h"

using namespace Editor;

ValueEditorFactory::ValueEditorFactory()
{
	mEditorCache.reserve(NUM_VALUE_EDITOR_TYPES);
	for (size_t i = 0; i < NUM_VALUE_EDITOR_TYPES; ++i)
	{
		mEditorCache.push_back(ValueEditorList());
	}
}

ValueEditorFactory::~ValueEditorFactory()
{
	for (size_t i = 0; i < NUM_VALUE_EDITOR_TYPES; ++i)
	{
		for (ValueEditorList::const_iterator it = mEditorCache[i].begin(); it != mEditorCache[i].end(); ++it)
		{
			delete *it;
		}
	}
}

void Editor::ValueEditorFactory::StoreValueEditor(AbstractValueEditor* editor)
{
	editor->DestroyModel();
	editor->ResetWidget();
	eValueEditorType type = editor->GetType();
	mEditorCache[type].push_back(editor);
}

Editor::AbstractValueEditor* ValueEditorFactory::GetValueEditorForProperty(const Reflection::PropertyHolder& property, const EntitySystem::EntityHandle& entity)
{
	switch (property.GetType())
	{
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) \
		case ARRAY_PROPERTY_TYPE_ID(typeID): \
			return GetValueEditorWithModel<ArrayEditor<typeClass> >(new PropertyModel<ARRAY_PROPERTY_TYPE_CLASS(typeClass)>(property, entity));
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE
	case PT_BOOL:
		return GetValueEditorWithModel<BoolEditor>(new PropertyModel<bool>(property, entity));
	case PT_INT32:
		if (string(property.GetName()) == "Layer")
			return GetValueEditorWithModel<StringEditor>(new LayerPropertyModel(property, entity));
		else
			return GetValueEditorWithModel<StringEditor>(new StringPropertyModel(property, entity));
	case PT_VECTOR2:
		return GetValueEditorWithModel<Vector2Editor>(new PropertyModel<Vector2>(property, entity));
	case PT_POINT:
		return GetValueEditorWithModel<PointEditor>(new PropertyModel<GfxSystem::Point>(property, entity));
	case PT_RESOURCE:
		return GetValueEditorWithModel<ResourceEditor>(new StringValueEditorModel<ResourceSystem::ResourcePtr>(new PropertyModel<ResourceSystem::ResourcePtr>(property, entity)));
	default:
		return GetValueEditorWithModel<StringEditor>(new StringPropertyModel(property, entity));
	}
}

AbstractValueEditor* ValueEditorFactory::GetValueEditorForEntityAttribute(EntitySystem::EntityHandle entity, EntityAttributeModel::eAttributeType type)
{
	return GetValueEditorWithModel<StringEditor>(new EntityAttributeModel(entity, type));
}