#include "Common.h"
#include "CmpPlatformStats.h"

using namespace EntitySystem;

void CmpPlatformStats::Init(ComponentDescription& desc)
{
	SetBlueprints(desc.GetNextItem()->GetData<EntityHandle>());
	mBlueprints.PostMessage(EntityMessage::TYPE_GET_MAX_HITPOINTS, &mHitpoints);
	SetParentShip(desc.GetNextItem()->GetData<EntityHandle>());
}

void CmpPlatformStats::Deinit() 
{

}

EntityMessage::eResult CmpPlatformStats::HandleMessage(const EntityMessage& msg)
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
	}
	return EntityMessage::RESULT_IGNORED;
}

void CmpPlatformStats::RegisterReflection()
{
	RegisterProperty<uint32>("Hitpoints", &GetHitpoints, &SetHitpoints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("Blueprints", &GetBlueprints, &SetBlueprints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("ParentShip", &GetParentShip, &SetParentShip, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}