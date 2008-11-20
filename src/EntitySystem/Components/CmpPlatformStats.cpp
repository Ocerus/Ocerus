#include "Common.h"
#include "CmpPlatformStats.h"

using namespace EntitySystem;

void CmpPlatformStats::Init(ComponentDescription& desc)
{
}

void CmpPlatformStats::Deinit()
{

}

EntityMessage::eResult CmpPlatformStats::HandleMessage(const EntityMessage& msg)
{
	return EntityMessage::RESULT_OK;
}

void CmpPlatformStats::RegisterReflection()
{
	RegisterProperty<uint32>("Hitpoints", &GetHitpoints, &SetHitpoints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}