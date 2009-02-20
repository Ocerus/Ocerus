#include "Common.h"
#include "CmpMaterial.h"

using namespace EntitySystem;


void EntitySystem::CmpMaterial::Init( void )
{
	mDurabilityRatio = 1.0f;
	mDensity = 1.0f;
}

void EntitySystem::CmpMaterial::Clean( void ) {}

EntityMessage::eResult EntitySystem::CmpMaterial::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_GET_DURABILITY_RATIO:
		DASSERT(msg.data);
		*(float32*)msg.data = GetDurabilityRatio();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_DENSITY:
		DASSERT(msg.data);
		*(float32*)msg.data = GetDensity();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpMaterial::RegisterReflection()
{
	RegisterProperty<float32>("Density", &GetDensity, &SetDensity, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("DurabilityRatio", &GetDurabilityRatio, &SetDurabilityRatio, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}