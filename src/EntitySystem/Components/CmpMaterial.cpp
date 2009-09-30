#include "Common.h"
#include "CmpMaterial.h"

void EntityComponents::CmpMaterial::Init( void )
{
	mDurabilityRatio = 1.0f;
	mDensity = 1.0f;
}

void EntityComponents::CmpMaterial::Clean( void ) {}

EntityMessage::eResult EntityComponents::CmpMaterial::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_GET_DURABILITY_RATIO:
		BS_DASSERT(msg.data);
		*(float32*)msg.data = GetDurabilityRatio();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_DENSITY:
		BS_DASSERT(msg.data);
		*(float32*)msg.data = GetDensity();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::CmpMaterial::RegisterReflection()
{
	RegisterProperty<float32>("Density", &GetDensity, &SetDensity, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("DurabilityRatio", &GetDurabilityRatio, &SetDurabilityRatio, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}