#include "Common.h"
#include "EntityAttributeModel.h"
#include "Editor/EditorMgr.h"
#include "Editor/EditorGUI.h"
#include "Editor/HierarchyWindow.h"

using namespace Editor;

Editor::EntityAttributeModel::EntityAttributeModel(const EntitySystem::EntityHandle& entity, eAttributeType type): mEntity(entity), mType(type)
{
}

string Editor::EntityAttributeModel::GetName() const
{
	switch(mType)
	{
		case TYPE_ID: return "ID";
		case TYPE_NAME: return "Name";
		case TYPE_TAG: return "Tag";
		case TYPE_PROTOTYPE: return "Prototype";
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
		case TYPE_TAG: return "User tag of the entity.";
		case TYPE_PROTOTYPE: return "Linked prototype.";
		default: OC_NOT_REACHED();
	}
	return "";
}

bool Editor::EntityAttributeModel::IsValid() const
{
	return mEntity.IsValid() && mEntity.Exists();
}

bool Editor::EntityAttributeModel::IsReadOnly() const
{
	switch(mType)
	{
		case TYPE_ID: return true;
		case TYPE_NAME: return false;
		case TYPE_TAG: return false;
		case TYPE_PROTOTYPE: return true;
		default: OC_NOT_REACHED();
	}
	return false;
}

bool EntityAttributeModel::IsRemovable() const
{
	if (mType != TYPE_PROTOTYPE) return false;
	return gEntityMgr.IsEntityLinkedToPrototype(mEntity);
}

void EntityAttributeModel::Remove()
{
	OC_ASSERT(mType == TYPE_PROTOTYPE);
	if (gEntityMgr.IsEntityLinkedToPrototype(mEntity))
		gEntityMgr.UnlinkEntityFromPrototype(mEntity);
}

string Editor::EntityAttributeModel::GetValue() const
{
	switch(mType)
	{
		case TYPE_ID: return StringConverter::ToString(mEntity.GetID());
		case TYPE_NAME: return gEntityMgr.GetEntityName(mEntity);
		case TYPE_TAG: return StringConverter::ToString(gEntityMgr.GetEntityTag(mEntity));
		case TYPE_PROTOTYPE:
			if (gEntityMgr.IsEntityLinkedToPrototype(mEntity))
			{
				return gEntityMgr.GetEntityPrototype(mEntity).GetName() + " (" + 
						Utils::StringConverter::ToString(gEntityMgr.GetEntityPrototype(mEntity).GetID()) + ")";
			}
			else
			{
				return "";
			}
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
		case TYPE_TAG:
			gEntityMgr.SetEntityTag(mEntity, StringConverter::FromString<EntitySystem::EntityTag>(newValue));
			gEditorMgr.PropertyValueChanged(); // to propagate the value from prototypes
			break;
		default: OC_NOT_REACHED();
	}

	if (gEditorMgr.IsEditingPrototype())
	{
		gEditorMgr.RefreshPrototypeWindow();
	}

	gEditorMgr.GetHierarchyWindow()->RefreshEntity(mEntity);
}