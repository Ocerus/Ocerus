#include "Common.h"
#include "Editor/PropertyEditors/EntityAttributeModel.h"

using namespace Editor;


Editor::EntityAttributeModel::EntityAttributeModel(const EntitySystem::EntityHandle& entity, eAttrType type): mEntity(entity), mType(type)
{

}

PropertyAccessFlags Editor::EntityAttributeModel::GetAccessFlags() const
{
	switch(mType)
	{
		case TYPE_ID: return PA_EDIT_READ;
		case TYPE_NAME: return PA_EDIT_READ | PA_EDIT_WRITE;
		default: OC_NOT_REACHED();
	}
	return 0;
}


StringKey EntityAttributeModel::GetKey() const
{
	switch(mType)
	{
		case TYPE_ID: return StringKey("id");
		case TYPE_NAME: return StringKey("name");
		default: OC_NOT_REACHED();
	}
	return "";
}

string EntityAttributeModel::GetValueString()
{
	switch(mType)
	{
		case TYPE_ID: return StringConverter::ToString(mEntity.GetID());
		case TYPE_NAME: return gEntityMgr.GetEntityName(mEntity);
		default: OC_NOT_REACHED();
	}
	return "";
}

void EntityAttributeModel::SetValueFromString(const string& str)
{
	switch(mType)
	{
		case TYPE_ID: OC_NOT_REACHED(); return;
		case TYPE_NAME: return gEntityMgr.SetEntityName(mEntity, str);
		default: OC_NOT_REACHED();
	}
}

const char* EntityAttributeModel::GetName() const
{
	switch(mType)
	{
		case TYPE_ID: return "Id";
		case TYPE_NAME: return "Name";
		default: OC_NOT_REACHED();
	}
	return "";
}

string EntityAttributeModel::GetComment() const
{
	switch(mType)
	{
		case TYPE_ID: return "ID of the entity.";
		case TYPE_NAME: return "Name of the entity.";
		default: OC_NOT_REACHED();
	}
	return "";
}

bool EntityAttributeModel::IsValid() const
{
	return mEntity.IsValid();
}
