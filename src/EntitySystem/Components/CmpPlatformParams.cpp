#include "Common.h"
#include "CmpPlatformParams.h"

using namespace EntitySystem;


void EntitySystem::CmpPlatformParams::Init( ComponentDescription& desc )
{
	SetMaterial(desc.GetNextItem()->GetData<EntityHandle>());
	SetShapeLength(desc.GetNextItem()->GetData<uint32>());
	mShape = 0;
	SetShape(desc.GetNextItem()->GetData<Vector2*>());
	ComputeParams();
}

void EntitySystem::CmpPlatformParams::Deinit( void )
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
	case EntityMessage::TYPE_GET_POLYSHAPE:
		assert(msg.data);
		((DataContainer*)msg.data)->SetData((uint8*)mShape, mShapeLength);
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_MATERIAL:
		assert(msg.data);
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
	mBaseDetachingChance = 1.0f;
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
	RegisterProperty<EntityHandle>("Material", &GetMaterial, &SetMaterial, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("MaxHitpoints", &GetMaxHitpoints, &SetMaxHitpoints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("NumSlots", &GetNumSlots, &SetNumSlots, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("NumLinkSlots", &GetNumLinkSlots, &SetNumLinkSlots, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Area", &GetArea, &SetArea, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Mass", &GetMass, &SetMass, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("BaseDetachingChance", &GetBaseDetachingChance, &SetBaseDetachingChance, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("ShapeLength", &GetShapeLength, &SetShapeLength, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2*>("Shape", &GetShape, &SetShape, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);

}