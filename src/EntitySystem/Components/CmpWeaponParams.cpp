#include "Common.h"
#include "CmpWeaponParams.h"

using namespace EntitySystem;


void EntitySystem::CmpWeaponParams::Init( void )
{
	mFiringDistance = 0;
	mSpread = 0;
	mReloadTime = 1;
	mProjectileType = 0;
	mProjectileSize = 0;
	mFiringSpeed = 0.5;
	mArcAngle = 0;
	mFireEffectScale = 1;
	mFireEffectDisplacement = 0;
}

void EntitySystem::CmpWeaponParams::Clean( void )
{

}

EntityMessage::eResult EntitySystem::CmpWeaponParams::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_POST_INIT:
		ComputeParams();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpWeaponParams::RegisterReflection( void )
{
	RegisterProperty<float32>("FiringDistance", &GetFiringDistance, &SetFiringDistance, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Spread", &GetSpread, &SetSpread, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("ProjectileType", &GetProjectileType, &SetProjectileType, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("ProjectileSize", &GetProjectileSize, &SetProjectileSize, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("FiringSpeed", &GetFiringSpeed, &SetFiringSpeed, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("ArcAngle", &GetArcAngle, &SetArcAngle, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("ReloadTime", &GetReloadTime, &SetReloadTime, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("FireEffectScale", &GetFireEffectScale, &SetFireEffectScale, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<StringKey>("FireEffect", &GetFireEffect, &SetFireEffect, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("FireEffectDisplacement", &GetFireEffectDisplacement, &SetFireEffectDisplacement, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);

	AddComponentDependency(CT_PLATFORM_ITEM_PARAMS);
}

void EntitySystem::CmpWeaponParams::ComputeParams( void )
{
	//TODO asi by se tu melo neco pocitat :)
	mFiringDistance = 20.0f;
	mSpread = 0.08f;

}