#include "Common.h"
#include "CmpPlatformParams.h"
#include "DataContainer.h"

using namespace EntityComponents;

#define HITPOINTS_RATIO 1000.0f
#define SLOTS_RATIO 1.0f
#define LINKSLOTS_RATIO 1.0f

void EntityComponents::CmpPlatformParams::Init( void )
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

void EntityComponents::CmpPlatformParams::Clean( void )
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
	case EntityMessage::TYPE_POST_INIT:
		ComputeParams();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_POLYSHAPE:
		BS_DASSERT(msg.data);
		((DataContainer*)msg.data)->SetData((uint8*)mShape, mShapeLength);
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_MAX_HITPOINTS:
		BS_DASSERT(msg.data);
		*(uint32*)msg.data = GetMaxHitpoints();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_MATERIAL:
		BS_DASSERT(msg.data);
		*(EntityHandle*)msg.data = mMaterial;
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void CmpPlatformParams::ComputeParams()
{
	mArea = MathUtils::ComputePolygonArea(mShape, mShapeLength);
	float32 density;
	mMaterial.PostMessage(EntityMessage::TYPE_GET_DENSITY, &density);
	mMass = mArea * density;
	float32 durabilityRatio;
	mMaterial.PostMessage(EntityMessage::TYPE_GET_DURABILITY_RATIO, &durabilityRatio);
	mMaxHitpoints = MathUtils::Round(HITPOINTS_RATIO * durabilityRatio * MathUtils::Sqrt(mArea));
	mNumSlots = MathUtils::Round(SLOTS_RATIO * mMass);

	// compute perimeter of the BS of the platform
	Vector2 center;
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
	RegisterProperty<EntityHandle>("Material", &EntityComponents::CmpPlatformParams::GetMaterial, &EntityComponents::CmpPlatformParams::SetMaterial, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("MaxHitpoints", &EntityComponents::CmpPlatformParams::GetMaxHitpoints, &EntityComponents::CmpPlatformParams::SetMaxHitpoints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("NumSlots", &EntityComponents::CmpPlatformParams::GetNumSlots, &EntityComponents::CmpPlatformParams::SetNumSlots, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("NumLinkSlots", &EntityComponents::CmpPlatformParams::GetNumLinkSlots, &EntityComponents::CmpPlatformParams::SetNumLinkSlots, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Area", &EntityComponents::CmpPlatformParams::GetArea, &EntityComponents::CmpPlatformParams::SetArea, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Mass", &EntityComponents::CmpPlatformParams::GetMass, &EntityComponents::CmpPlatformParams::SetMass, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("BaseDetachingChance", &EntityComponents::CmpPlatformParams::GetBaseDetachingChance, &EntityComponents::CmpPlatformParams::SetBaseDetachingChance, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("ShapeLength", &EntityComponents::CmpPlatformParams::GetShapeLength, &EntityComponents::CmpPlatformParams::SetShapeLength, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2*>("Shape", &EntityComponents::CmpPlatformParams::GetShape, &EntityComponents::CmpPlatformParams::SetShape, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("ExplodeEffectScale", &EntityComponents::CmpPlatformParams::GetExplodeEffectScale, &EntityComponents::CmpPlatformParams::SetExplodeEffectScale, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<StringKey>("ExplodeEffect", &EntityComponents::CmpPlatformParams::GetExplodeEffect, &EntityComponents::CmpPlatformParams::SetExplodeEffect, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<StringKey>("ResourceGroup", &EntityComponents::CmpPlatformParams::GetResourceGroup, &EntityComponents::CmpPlatformParams::SetResourceGroup, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<GfxSystem::Color>("FillColor", &EntityComponents::CmpPlatformParams::GetFillColor, &EntityComponents::CmpPlatformParams::SetFillColor, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}
