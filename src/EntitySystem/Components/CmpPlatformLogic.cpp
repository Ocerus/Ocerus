#include "Common.h"
#include "CmpPlatformLogic.h"

using namespace EntitySystem;

void CmpPlatformLogic::Init(ComponentDescription& desc)
{
	SetBlueprints(desc.GetNextItem()->GetData<EntityHandle>());
	mBlueprints.PostMessage(EntityMessage::TYPE_GET_MAX_HITPOINTS, &mHitpoints);
	SetParentShip(desc.GetNextItem()->GetData<EntityHandle>());
}

void CmpPlatformLogic::Deinit() 
{

}

EntityMessage::eResult CmpPlatformLogic::HandleMessage(const EntityMessage& msg)
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_GET_BLUEPRINTS:
		assert(msg.data);
		*(EntityHandle*)msg.data = GetBlueprints();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_PARENT:
		assert(msg.data);
		*(EntityHandle*)msg.data = GetParentShip();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_HITPOINTS:
		assert(msg.data);
		*(uint32*)msg.data = GetHitpoints();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW:
		for (EntityList::iterator i=mItems.begin(); i!=mItems.end(); ++i)
			i->PostMessage(EntityMessage::TYPE_DRAW_INNER);
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_ADD_PLATFORM_ITEM:
		assert(msg.data);
		mItems.push_back(*(EntityHandle*)msg.data);
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void CmpPlatformLogic::RegisterReflection()
{
	RegisterProperty<uint32>("Hitpoints", &GetHitpoints, &SetHitpoints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("Blueprints", &GetBlueprints, &SetBlueprints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("ParentShip", &GetParentShip, &SetParentShip, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}