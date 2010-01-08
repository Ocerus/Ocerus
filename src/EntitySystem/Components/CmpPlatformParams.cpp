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
	mShape.Clear();
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
	mArea = MathUtils::ComputePolygonArea(mShape.GetRawArrayPtr(), mShape.GetSize());
	float32 density = GetOwner().GetProperty("Density").GetValue<float32>();
	mMass = mArea * density;
	float32 durabilityRatio = GetOwner().GetProperty("DurabilityRatio").GetValue<float32>();;
	mMaxHitpoints = MathUtils::Round(HITPOINTS_RATIO * durabilityRatio * MathUtils::Sqrt(mArea));
	mNumSlots = MathUtils::Round(SLOTS_RATIO * mMass);

	// compute perimeter of the BS of the platform
	Vector2 center(0, 0);
	for (int32 i=0; i<mShape.GetSize(); ++i)
		center += mShape[i];
	center *= 1.0f/mShape.GetSize();
	float32 perimeter = 0.0f;
	for (int32 i=0; i<mShape.GetSize(); ++i)
	{
		float32 dist = MathUtils::Distance(mShape[i], center);
		if (dist > perimeter)
			perimeter = dist;
	}
	mNumLinkSlots = MathUtils::Round(LINKSLOTS_RATIO * perimeter);
	mBaseDetachingChance = 0.5f;
}

Array<Vector2>* EntityComponents::CmpPlatformParams::GetShape( void ) const
{
	return const_cast<Array<Vector2>*>(&mShape);
}

void EntityComponents::CmpPlatformParams::SetShape(Array<Vector2>* shape)
{
	mShape.CopyFrom(*shape);
}

void EntityComponents::CmpPlatformParams::RegisterReflection()
{
	RegisterProperty<EntityHandle>("Material", &CmpPlatformParams::GetMaterial, &CmpPlatformParams::SetMaterial, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ, "");
	RegisterProperty<uint32>("MaxHitpoints", &CmpPlatformParams::GetMaxHitpoints, &CmpPlatformParams::SetMaxHitpoints, PA_EDIT_READ | PA_SCRIPT_READ, "");
	RegisterProperty<uint32>("NumSlots", &CmpPlatformParams::GetNumSlots, &CmpPlatformParams::SetNumSlots, PA_EDIT_READ | PA_SCRIPT_READ, "");
	RegisterProperty<uint32>("NumLinkSlots", &CmpPlatformParams::GetNumLinkSlots, &CmpPlatformParams::SetNumLinkSlots, PA_EDIT_READ | PA_SCRIPT_READ, "");
	RegisterProperty<float32>("Area", &CmpPlatformParams::GetArea, &CmpPlatformParams::SetArea, PA_EDIT_READ | PA_SCRIPT_READ, "");
	RegisterProperty<float32>("Mass", &CmpPlatformParams::GetMass, &CmpPlatformParams::SetMass, PA_EDIT_READ | PA_SCRIPT_READ, "");
	RegisterProperty<float32>("BaseDetachingChance", &CmpPlatformParams::GetBaseDetachingChance, &CmpPlatformParams::SetBaseDetachingChance, PA_EDIT_READ | PA_SCRIPT_READ, "");
	RegisterProperty<Array<Vector2>*>("Shape", &CmpPlatformParams::GetShape, &CmpPlatformParams::SetShape, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ, "");
	RegisterProperty<float32>("ExplodeEffectScale", &CmpPlatformParams::GetExplodeEffectScale, &CmpPlatformParams::SetExplodeEffectScale, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ, "");
	RegisterProperty<StringKey>("ExplodeEffect", &CmpPlatformParams::GetExplodeEffect, &CmpPlatformParams::SetExplodeEffect, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ, "");
	RegisterProperty<StringKey>("ResourceGroup", &CmpPlatformParams::GetResourceGroup, &CmpPlatformParams::SetResourceGroup, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ, "");
	RegisterProperty<GfxSystem::Color>("FillColor", &CmpPlatformParams::GetFillColor, &CmpPlatformParams::SetFillColor, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ, "");
}
