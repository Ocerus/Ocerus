#include "Common.h"
#include "CmpPlatformParams.h"

using namespace EntitySystem;


void EntitySystem::CmpPlatformParams::Init( ComponentDescription& desc )
{
}

void EntitySystem::CmpPlatformParams::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpPlatformParams::HandleMessage( const EntityMessage& msg )
{

	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpPlatformParams::RegisterReflection()
{
	RegisterProperty<EntityHandle>("Material", &GetMaterial, &SetMaterial, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("MaxHitpoints", &GetMaxHitpoints, &SetMaxHitpoints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("NumSlots", &GetNumSlots, &SetNumSlots, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("NumLinkSlots", &GetNumLinkSlots, &SetNumLinkSlots, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Area", &GetArea, &SetArea, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Mass", &GetMass, &SetMass, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("BaseDetachingChance", &GetBaseDetachingChance, &SetBaseDetachingChance, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);

}