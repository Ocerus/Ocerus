#include "Common.h"
#include "CmpEngineParams.h"

using namespace EntitySystem;

#define POWER_RATIO 100.0f

void EntitySystem::CmpEngineParams::Init( void )
{
	mArcAngle = 0.0f;
	mStabilizationRatio = 0;
	mThrustEffectScale = 1.0f;
	mThrustEffectDisplacement = 0.0f;
	mThrustEffectPowerScale = 10.0f;
	mThrustEffect = "";

	mMaxPower = 0;
}

void EntitySystem::CmpEngineParams::Clean( void )
{

}

EntityMessage::eResult EntitySystem::CmpEngineParams::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_POST_INIT:
		ComputeParams();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_ARC_ANGLE:
		assert(msg.data);
		*(float32*)msg.data = GetArcAngle();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_MAX_POWER:
		assert(msg.data);
		*(uint32*)msg.data = GetMaxPower();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_STABILIZATION_RATIO:
		assert(msg.data);
		*(uint32*)msg.data = GetStabilizationRatio();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_EFFECT:
		assert(msg.data);
		*(StringKey*)msg.data = GetThrustEffect();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_EFFECT_SCALE:
		assert(msg.data);
		*(float32*)msg.data = GetThrustEffectScale();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpEngineParams::RegisterReflection( void )
{
	RegisterProperty<uint32>("MaxPower", &GetMaxPower, &SetMaxPower, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("ArcAngle", &GetArcAngle, &SetArcAngle, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("StabilizationRatio", &GetStabilizationRatio, &SetStabilizationRatio, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("ThrustEffectScale", &GetThrustEffectScale, &SetThrustEffectScale, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<StringKey>("ThrustEffect", &GetThrustEffect, &SetThrustEffect, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("ThrustEffectDisplacement", &GetThrustEffectDisplacement, &SetThrustEffectDisplacement, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("ThrustEffectPowerScale", &GetThrustEffectPowerScale, &SetThrustEffectPowerScale, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);

	AddComponentDependency(CT_PLATFORM_ITEM_PARAMS);
}

void EntitySystem::CmpEngineParams::ComputeParams( void )
{
	EntityHandle material = ((PropertyHolder)GetProperty("Material")).GetValue<EntityHandle>();
	float32 density = ((PropertyHolder)material.GetProperty("Density")).GetValue<float32>();
	mMaxPower = MathUtils::Round(POWER_RATIO * density);
}