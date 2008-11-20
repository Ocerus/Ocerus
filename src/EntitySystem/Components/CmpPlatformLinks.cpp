#include "Common.h"
#include "CmpPlatformLinks.h"

using namespace EntitySystem;


void EntitySystem::CmpPlatformLinks::Init( ComponentDescription& desc )
{
	SetFirstPlatform(desc.GetNextItem()->GetEntityHandle());
	SetSecondPlatform(desc.GetNextItem()->GetEntityHandle());
	ComputeDetachingChance();
	//TODO vytvorit linky
}

void EntitySystem::CmpPlatformLinks::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpPlatformLinks::HandleMessage( const EntityMessage& msg )
{
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpPlatformLinks::RegisterReflection()
{
	RegisterProperty<EntityHandle>("FirstPlatform", &GetFirstPlatform, &SetFirstPlatform, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("SecondPlatform", &GetSecondPlatform, &SetSecondPlatform, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("DetachingChance", &GetDetachingChance, &SetDetachingChance, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("NumLinks", &GetNumLinks, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

void EntitySystem::CmpPlatformLinks::ComputeDetachingChance( void )
{

}