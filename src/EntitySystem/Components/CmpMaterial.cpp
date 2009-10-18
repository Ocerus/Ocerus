#include "Common.h"
#include "CmpMaterial.h"

void EntityComponents::CmpMaterial::Create( void )
{
	mDurabilityRatio = 1.0f;
	mDensity = 1.0f;
}

void EntityComponents::CmpMaterial::Destroy( void ) {}

EntityMessage::eResult EntityComponents::CmpMaterial::HandleMessage( const EntityMessage& msg )
{
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::CmpMaterial::RegisterReflection()
{
    RegisterProperty<float32>("Density", &CmpMaterial::GetDensity, &CmpMaterial::SetDensity, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<float32>("DurabilityRatio", &CmpMaterial::GetDurabilityRatio, &CmpMaterial::SetDurabilityRatio, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ);
}
