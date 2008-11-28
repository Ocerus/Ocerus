#include "Common.h"
#include "CmpMaterial.h"

using namespace EntitySystem;


void EntitySystem::CmpMaterial::Init( ComponentDescription& desc )
{
	SetDurabilityRatio(desc.GetNextItem()->GetData<float32>());
	SetDensity(desc.GetNextItem()->GetData<float32>());
}

void EntitySystem::CmpMaterial::Deinit( void ) {}

EntityMessage::eResult EntitySystem::CmpMaterial::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_GET_DURABILITY_RATIO:
		assert(msg.data);
		*(float32*)msg.data = GetDurabilityRatio();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_DENSITY:
		assert(msg.data);
		*(float32*)msg.data = GetDensity();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpMaterial::RegisterReflection()
{
	RegisterProperty<float32>("Density", &GetDensity, &SetDensity, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("DurabilityRatio", &GetDensity, &SetDensity, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}