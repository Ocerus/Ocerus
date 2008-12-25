#include "Common.h"
#include "CmpEngineParams.h"

using namespace EntitySystem;

#define HITPOINTS_RATIO 100.0f
#define POWER_RATIO 100.0f

void EntitySystem::CmpEngineParams::Init( void )
{
	mMaterial.Invalidate();
	mArcAngle = 0.0f;
	mStabilizationRatio = 0;
	mTextureScale = 1.0f;
	mTextureAngle = 0.0f;
	mThrustEffectScale = 1.0f;
	mThrustEffectDisplacement = 0.0f;
	mThrustEffectPowerScale = 10.0f;

	mMaxHitpoints = 0;
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
	case EntityMessage::TYPE_GET_MAX_HITPOINTS:
		assert(msg.data);
		*(uint32*)msg.data = GetMaxHitpoints();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_ARC_ANGLE:
		assert(msg.data);
		*(float32*)msg.data = GetArcAngle();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_MAX_POWER:
		assert(msg.data);
		*(uint32*)msg.data = GetMaxPower();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_MATERIAL:
		assert(msg.data);
		*(EntityHandle*)msg.data = mMaterial;
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_STABILIZATION_RATIO:
		assert(msg.data);
		*(uint32*)msg.data = GetStabilizationRatio();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_TEXTURE:
		assert(msg.data);
		*(StringKey*)msg.data = GetTexture();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_TEXTURE_ANGLE:
		assert(msg.data);
		*(float32*)msg.data = GetTextureAngle();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_TEXTURE_SCALE:
		assert(msg.data);
		*(float32*)msg.data = GetTextureScale();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_EFFECT:
		assert(msg.data);
		*(StringKey*)msg.data = GetThrustEffect();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_EFFECT_SCALE:
		assert(msg.data);
		*(float32*)msg.data = GetThrustEffectScale();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_RESOURCE_GROUP:
		assert(msg.data);
		*(StringKey*)msg.data = GetResourceGroup();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpEngineParams::RegisterReflection( void )
{
	RegisterProperty<EntityHandle>("Material", &GetMaterial, &SetMaterial, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("MaxHitpoints", &GetMaxHitpoints, &SetMaxHitpoints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("MaxPower", &GetMaxPower, &SetMaxPower, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("ArcAngle", &GetArcAngle, &SetArcAngle, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("StabilizationRatio", &GetStabilizationRatio, &SetStabilizationRatio, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("TextureAngle", &GetTextureAngle, &SetTextureAngle, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("TextureScale", &GetTextureScale, &SetTextureScale, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<StringKey>("Texture", &GetTexture, &SetTexture, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("ThrustEffectScale", &GetThrustEffectScale, &SetThrustEffectScale, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<StringKey>("ThrustEffect", &GetThrustEffect, &SetThrustEffect, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<StringKey>("ResourceGroup", &GetResourceGroup, &SetResourceGroup, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("ThrustEffectDisplacement", &GetThrustEffectDisplacement, &SetThrustEffectDisplacement, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("ThrustEffectPowerScale", &GetThrustEffectPowerScale, &SetThrustEffectPowerScale, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
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