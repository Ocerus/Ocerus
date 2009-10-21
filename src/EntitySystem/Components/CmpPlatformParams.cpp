#include "Common.h"
#include "CmpPlatformParams.h"
#include "DataContainer.h"

using namespace EntityComponents;

#define HITPOINTS_RATIO 1000.0f
#define SLOTS_RATIO 1.0f
#define LINKSLOTS_RATIO 1.0f

void EntityComponents::CmpPlatformParams::Create( void )
{
	mMaterial.Invalidate();
	mShapeLength = 0;
	mShape = 0;
	mResourceGroup = "";
	mExplodeEffect = "";
	mExplodeEffectScale = 1;
	mFillColor = GfxSystem::Color::NullColor;

	mArea = 0.0f;
	mBaseDetachingChance = 0.0f;
	mMass = 0.0f;
	mMaxHitpoints = 0;
	mNumLinkSlots = 0;
	mNumSlots = 0;
}

void EntityComponents::CmpPlatformParams::Destroy( void )
{
	if (mShape)
	{
		delete[] mShape;
		mShape = 0;
	}
}

EntityMessage::eResult EntityComponents::CmpPlatformParams::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::INIT:
		ComputeParams();
		return EntityMessage::RESULT_OK;
	default:
		break;
	}
	return EntityMessage::RESULT_IGNORED;
}

void CmpPlatformParams::ComputeParams()
{
	EntityHandle material = GetProperty("Material").GetValue<EntityHandle>();
	mArea = MathUtils::ComputePolygonArea(mShape, mShapeLength);
	float32 density = material.GetProperty("Density").GetValue<float32>();
	mMass = mArea * density;
	float32 durabilityRatio = material.GetProperty("DurabilityRatio").GetValue<float32>();;
	mMaxHitpoints = MathUtils::Round(HITPOINTS_RATIO * durabilityRatio * MathUtils::Sqrt(mArea));
	mNumSlots = MathUtils::Round(SLOTS_RATIO * mMass);

	// compute perimeter of the BS of the platform
	Vector2 center(0, 0);
	for (uint32 i=0; i<mShapeLength; ++i)
		center += mShape[i];
	center *= 1.0f/mShapeLength;
	float32 perimeter = 0.0f;
	for (uint32 i=0; i<mShapeLength; ++i)
	{
		float32 dist = MathUtils::Distance(mShape[i], center);
		if (dist > perimeter)
			perimeter = dist;
	}
	mNumLinkSlots = MathUtils::Round(LINKSLOTS_RATIO * perimeter);
	mBaseDetachingChance = 0.5f;
}

void EntityComponents::CmpPlatformParams::SetShape(Vector2* shape)
{
	if (mShape)
		delete[] mShape;
	mShape = new Vector2[mShapeLength];
	for (uint32 i=0; i<mShapeLength; ++i)
		mShape[i] = shape[i];
}

void EntityComponents::CmpPlatformParams::RegisterReflection()
{
	RegisterProperty<EntityHandle>("Material", &CmpPlatformParams::GetMaterial, &CmpPlatformParams::SetMaterial, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<uint32>("MaxHitpoints", &CmpPlatformParams::GetMaxHitpoints, &CmpPlatformParams::SetMaxHitpoints, PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<uint32>("NumSlots", &CmpPlatformParams::GetNumSlots, &CmpPlatformParams::SetNumSlots, PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<uint32>("NumLinkSlots", &CmpPlatformParams::GetNumLinkSlots, &CmpPlatformParams::SetNumLinkSlots, PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<float32>("Area", &CmpPlatformParams::GetArea, &CmpPlatformParams::SetArea, PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<float32>("Mass", &CmpPlatformParams::GetMass, &CmpPlatformParams::SetMass, PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<float32>("BaseDetachingChance", &CmpPlatformParams::GetBaseDetachingChance, &CmpPlatformParams::SetBaseDetachingChance, PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<uint32>("ShapeLength", &CmpPlatformParams::GetShapeLength, &CmpPlatformParams::SetShapeLength, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<Vector2*>("Shape", &CmpPlatformParams::GetShape, &CmpPlatformParams::SetShape, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<float32>("ExplodeEffectScale", &CmpPlatformParams::GetExplodeEffectScale, &CmpPlatformParams::SetExplodeEffectScale, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<StringKey>("ExplodeEffect", &CmpPlatformParams::GetExplodeEffect, &CmpPlatformParams::SetExplodeEffect, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<StringKey>("ResourceGroup", &CmpPlatformParams::GetResourceGroup, &CmpPlatformParams::SetResourceGroup, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<GfxSystem::Color>("FillColor", &CmpPlatformParams::GetFillColor, &CmpPlatformParams::SetFillColor, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ);
}
