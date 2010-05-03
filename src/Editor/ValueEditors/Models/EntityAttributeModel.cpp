#include "Common.h"
#include "EntityAttributeModel.h"
#include "Editor/EditorMgr.h"

using namespace Editor;

Editor::EntityAttributeModel::EntityAttributeModel(const EntitySystem::EntityHandle& entity, eAttributeType type): mEntity(entity), mType(type)
{
}

string Editor::EntityAttributeModel::GetName() const
{
	switch(mType)
	{
		case TYPE_ID: return "Id";
		case TYPE_NAME: return "Name";
		default: OC_NOT_REACHED();
	}
	return "";
}

string Editor::EntityAttributeModel::GetTooltip() const
{
	switch(mType)
	{
		case TYPE_ID: return "ID of the entity.";
		case TYPE_NAME: return "Name of the entity.";
		default: OC_NOT_REACHED();
	}
	return "";
}

bool Editor::EntityAttributeModel::IsValid() const
{
	return mEntity.IsValid();
}

bool Editor::EntityAttributeModel::IsReadOnly() const
{
	switch(mType)
	{
		case TYPE_ID: return true;
		case TYPE_NAME: return false;
		default: OC_NOT_REACHED();
	}
	return false;
}

string Editor::EntityAttributeModel::GetValue() const
{
	switch(mType)
	{
		case TYPE_ID: return StringConverter::ToString(mEntity.GetID());
		case TYPE_NAME: return gEntityMgr.GetEntityName(mEntity);
		default: OC_NOT_REACHED();
	}
	return "";
}

void Editor::EntityAttributeModel::SetValue(const string& newValue)
{
	switch(mType)
	{
		case TYPE_NAME: 
			gEntityMgr.SetEntityName(mEntity, newValue);
			break;
		default: OC_NOT_REACHED();
	}

	if (gEditorMgr.IsEditingPrototype())
	{
		gEditorMgr.RefreshPrototypeWindow();
	}
}