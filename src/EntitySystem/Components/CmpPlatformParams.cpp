#include "Common.h"
#include "CmpPlatformParams.h"

using namespace EntitySystem;

#define HITPOINTS_RATIO 1000.0f
#define SLOTS_RATIO 1.0f
#define LINKSLOTS_RATIO 1.0f

void EntitySystem::CmpPlatformParams::Init( void )
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

void EntitySystem::CmpPlatformParams::Clean( void )
{
	if (mShape)
	{
		DYN_DELETE_ARRAY mShape;
		mShape = 0;
	}
}

EntityMessage::eResult EntitySystem::CmpPlatformParams::HandleMessage( const EntityMessage& msg )
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
	//TODO tady by mel hrat roli jeste parametr v materialu, kerej tam chybi
	mNumLinkSlots = MathUtils::Round(LINKSLOTS_RATIO * perimeter); 
	//TODO tohle chce domyslet
	mBaseDetachingChance = 0.5f;
}

void EntitySystem::CmpPlatformParams::SetShape(Vector2* shape)
{
	if (mShape)
		DYN_DELETE_ARRAY mShape;
	mShape = DYN_NEW Vector2[mShapeLength];
	for (uint32 i=0; i<mShapeLength; ++i)
		mShape[i] = shape[i];
}

void EntitySystem::CmpPlatformParams::RegisterReflection()
{
	RegisterProperty<EntityHandle>("Material", &GetMaterial, &SetMaterial, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("MaxHitpoints", &GetMaxHitpoints, &SetMaxHitpoints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("NumSlots", &GetNumSlots, &SetNumSlots, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("NumLinkSlots", &GetNumLinkSlots, &SetNumLinkSlots, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Area", &GetArea, &SetArea, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Mass", &GetMass, &SetMass, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("BaseDetachingChance", &GetBaseDetachingChance, &SetBaseDetachingChance, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("ShapeLength", &GetShapeLength, &SetShapeLength, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2*>("Shape", &GetShape, &SetShape, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("ExplodeEffectScale", &GetExplodeEffectScale, &SetExplodeEffectScale, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<StringKey>("ExplodeEffect", &GetExplodeEffect, &SetExplodeEffect, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<StringKey>("ResourceGroup", &GetResourceGroup, &SetResourceGroup, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<GfxSystem::Color>("FillColor", &GetFillColor, &SetFillColor, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}