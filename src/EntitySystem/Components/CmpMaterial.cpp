#include "Common.h"
#include "CmpMaterial.h"

using namespace EntitySystem;


void EntitySystem::CmpMaterial::Init( ComponentDescription& desc )
{
}

void EntitySystem::CmpMaterial::Deinit( void ) {}

EntityMessage::eResult EntitySystem::CmpMaterial::HandleMessage( const EntityMessage& msg )
{
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpMaterial::RegisterReflection()
{
	RegisterProperty<float32>("Density", &GetDensity, &SetDensity, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("DurabilityRatio", &GetDensity, &SetDensity, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}