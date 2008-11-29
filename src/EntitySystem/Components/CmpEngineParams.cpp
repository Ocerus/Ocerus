#include "Common.h"
#include "CmpEngineParams.h"

using namespace EntitySystem;

#define HITPOINTS_RATIO 100.0f
#define POWER_RATIO 100.0f

void EntitySystem::CmpEngineParams::Init( ComponentDescription& desc )
{
	SetMaterial(desc.GetNextItem()->GetData<EntityHandle>());
	SetMinAngle(desc.GetNextItem()->GetData<float32>());
	SetMaxAngle(desc.GetNextItem()->GetData<float32>());
	SetAngularSpeed(desc.GetNextItem()->GetData<float32>());
	ComputeParams();
}

void EntitySystem::CmpEngineParams::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpEngineParams::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_GET_MAX_HITPOINTS:
		assert(msg.data);
		*(uint32*)msg.data = GetMaxHitpoints();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_MAX_POWER:
		assert(msg.data);
		*(uint32*)msg.data = GetMaxPower();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_MATERIAL:
		assert(msg.data);
		*(EntityHandle*)msg.data = mMaterial;
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpEngineParams::RegisterReflection( void )
{
	RegisterProperty<EntityHandle>("Material", &GetMaterial, &SetMaterial, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("MaxHitpoints", &GetMaxHitpoints, &SetMaxHitpoints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("MaxPower", &GetMaxPower, &SetMaxPower, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("MinAngle", &GetMinAngle, &SetMinAngle, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("MaxAngle", &GetMaxAngle, &SetMaxAngle, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("AngularSpeed", &GetAngularSpeed, &SetAngularSpeed, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

void EntitySystem::CmpEngineParams::ComputeParams( void )
{
	float32 durabilityRatio;
	mMaterial.PostMessage(EntityMessage::TYPE_GET_DURABILITY_RATIO, &durabilityRatio);
	mMaxHitpoints = MathUtils::Round(HITPOINTS_RATIO * durabilityRatio);
	float32 density;
	mMaterial.PostMessage(EntityMessage::TYPE_GET_DENSITY, &density);
	mMaxPower = MathUtils::Round(POWER_RATIO * density);
}