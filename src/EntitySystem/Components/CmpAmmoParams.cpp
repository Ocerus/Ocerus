#include "Common.h"
#include "CmpAmmoParams.h"

using namespace EntitySystem;


void EntitySystem::CmpAmmoParams::Init( void )
{
	mMaterial.Invalidate();
	mDisableRatio = 0;
	mPowerRatio = 1;
	mKnockbackRatio = 1;
	mDistanceRatio = 1;
	mSpeedRatio = 1;
	mTrailEffectScale = 1;
	mExplodeEffectScale = 1;
}

void EntitySystem::CmpAmmoParams::Clean( void )
{

}

EntityMessage::eResult EntitySystem::CmpAmmoParams::HandleMessage( const EntityMessage& msg )
{
	return EntityMessage::RESULT_OK;
}

void EntitySystem::CmpAmmoParams::RegisterReflection( void )
{
	RegisterProperty<EntityHandle>("Material", &GetMaterial, &SetMaterial, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("Size", &GetSize, &SetSize, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("DisableRatio", &GetDisableRatio, &SetDisableRatio, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("PowerRatio", &GetPowerRatio, &SetPowerRatio, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("KnockbackRatio", &GetKnockbackRatio, &SetKnockbackRatio, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("DistanceRatio", &GetDistanceRatio, &SetDistanceRatio, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("SpeedRatio", &GetSpeedRatio, &SetSpeedRatio, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("TrailEffectScale", &GetTrailEffectScale, &SetTrailEffectScale, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<StringKey>("TrailEffect", &GetTrailEffect, &SetTrailEffect, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("ExplodeEffectScale", &GetExplodeEffectScale, &SetExplodeEffectScale, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<StringKey>("ExplodeEffect", &GetExplodeEffect, &SetExplodeEffect, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<StringKey>("ResourceGroup", &GetResourceGroup, &SetResourceGroup, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}