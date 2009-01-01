#include "Common.h"
#include "CmpPlatformItemParams.h"

using namespace EntitySystem;

#define HITPOINTS_RATIO 100.0f

void EntitySystem::CmpPlatformItemParams::Init( void )
{
	mMaterial.Invalidate();
	mTextureScale = 1.0f;
	mTextureAngle = 0.0f;

	mMaxHitpoints = 0;
}

void EntitySystem::CmpPlatformItemParams::Clean( void )
{

}

EntityMessage::eResult EntitySystem::CmpPlatformItemParams::HandleMessage( const EntityMessage& msg )
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
	case EntityMessage::TYPE_GET_MATERIAL:
		assert(msg.data);
		*(EntityHandle*)msg.data = mMaterial;
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
	case EntityMessage::TYPE_GET_RESOURCE_GROUP:
		assert(msg.data);
		*(StringKey*)msg.data = GetResourceGroup();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpPlatformItemParams::RegisterReflection( void )
{
	RegisterProperty<EntityHandle>("Material", &GetMaterial, &SetMaterial, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("MaxHitpoints", &GetMaxHitpoints, &SetMaxHitpoints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("TextureAngle", &GetTextureAngle, &SetTextureAngle, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("TextureScale", &GetTextureScale, &SetTextureScale, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<StringKey>("Texture", &GetTexture, &SetTexture, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<StringKey>("ResourceGroup", &GetResourceGroup, &SetResourceGroup, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

void EntitySystem::CmpPlatformItemParams::ComputeParams( void )
{
	float32 durabilityRatio;
	mMaterial.PostMessage(EntityMessage::TYPE_GET_DURABILITY_RATIO, &durabilityRatio);
	mMaxHitpoints = MathUtils::Round(HITPOINTS_RATIO * durabilityRatio);
}