#include "Common.h"
#include "CmpPlatformLogic.h"
#include <Box2D.h>

using namespace EntitySystem;

#define EXPLOSION_FORCE_RATIO 10.0f

void CmpPlatformLogic::Init(void)
{
	mBlueprints.Invalidate();
	mHitpoints = 0;
	mParentShip.Invalidate();
	mItems.clear();

	mPickCircleCenter = Vector2_Zero;
	mPickCircleRadius = 0;
}

void CmpPlatformLogic::Clean(void) 
{

}

EntityMessage::eResult CmpPlatformLogic::HandleMessage(const EntityMessage& msg)
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_POST_INIT:
		{
			mBlueprints.PostMessage(EntityMessage::TYPE_GET_MAX_HITPOINTS, &mHitpoints);

			// compute pick stuff
			DataContainer cont;
			PostMessage(EntityMessage::TYPE_GET_POLYSHAPE, &cont);
			Vector2* shape = (Vector2*)cont.GetData();
			int32 shapeLen = cont.GetSize();
			// compute center
			//TODO najit ten stred tak, aby to udelalo nejlepsi bounding circle
			mPickCircleCenter.SetZero();
			for (int i=0; i<shapeLen; ++i)
				mPickCircleCenter += shape[i];
			mPickCircleCenter *= 1.0f/shapeLen;
			// compute radius
			mPickCircleRadius = 0.0f;
			for (int i=0; i<shapeLen; ++i)
			{
				float32 dist = MathUtils::Distance(mPickCircleCenter, shape[i]);
				if (dist > mPickCircleRadius)
					mPickCircleRadius = dist;
			}

			// set the team
			GetOwner().SetTeam(mParentShip);
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DAMAGE:
		assert(msg.data);
		{
			PropertyHolder prop = mBlueprints.GetProperty("Material");
			EntityHandle material = prop.GetValue<EntityHandle>();
			prop = material.GetProperty("DurabilityRatio");
			int32 newHP = mHitpoints - MathUtils::Round(*(float32*)msg.data / prop.GetValue<float32>());
			if (newHP <= 0)
				Die();
			else
				mHitpoints = newHP;
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DIE:
		Die();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DETACH_PLATFORM:
		mParentShip.Invalidate();
		break;
	case EntityMessage::TYPE_GET_BLUEPRINTS:
		assert(msg.data);
		*(EntityHandle*)msg.data = GetBlueprints();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_PARENT:
		assert(msg.data);
		*(EntityHandle*)msg.data = GetParentShip();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_HITPOINTS:
		assert(msg.data);
		*(uint32*)msg.data = GetHitpoints();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_ADD_PLATFORM_ITEM:
		assert(msg.data);
		mItems.push_back(*(EntityHandle*)msg.data);
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_MOUSE_PICK:
		assert(msg.data);
		{
			Vector2 pos;
			PostMessage(EntityMessage::TYPE_GET_BODY_POSITION, &pos);
			float32 angle;
			PostMessage(EntityMessage::TYPE_GET_ANGLE, &angle);
			((EntityPicker*)msg.data)->Update(GetOwner(), MathUtils::Multiply(Matrix22(angle), mPickCircleCenter) + pos, mPickCircleRadius);
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW_OVERLAY:
		assert(msg.data);
		DrawSelectionOverlay(*(bool*)msg.data);
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void CmpPlatformLogic::RegisterReflection()
{
	RegisterProperty<uint32>("Hitpoints", &GetHitpoints, &SetHitpoints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("Blueprints", &GetBlueprints, &SetBlueprints, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("ParentShip", &GetParentShip, &SetParentShip, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);

	AddComponentDependency(CT_PLATFORM_PHYSICS);
}

void EntitySystem::CmpPlatformLogic::DrawSelectionOverlay( const bool hover ) const
{
	Vector2 pos;
	PostMessage(EntityMessage::TYPE_GET_BODY_POSITION, &pos);
	float32 angle;
	PostMessage(EntityMessage::TYPE_GET_ANGLE, &angle);
	GfxSystem::Color color(255,0,0,180);
	if (hover)
		color = GfxSystem::Color(255,255,255,180);
	gGfxRenderer.DrawCircle(gGfxRenderer.WorldToScreen(MathUtils::Multiply(Matrix22(angle), mPickCircleCenter) + pos), gGfxRenderer.WorldToScreenScalar(mPickCircleRadius), 
		GfxSystem::Color::NullColor, GfxSystem::Pen(color));
}

void EntitySystem::CmpPlatformLogic::Die( void )
{
	// create the PS explode effect
	PropertyHolder prop = mBlueprints.GetProperty("ExplodeEffect");
	StringKey effect = prop.GetValue<StringKey>();
	prop = mBlueprints.GetProperty("ResourceGroup");
	StringKey resGroup = prop.GetValue<StringKey>();
	GfxSystem::ParticleSystemPtr ps = gPSMgr.SpawnPS(resGroup, effect);
	if (!ps.IsNull())
	{
		prop = mBlueprints.GetProperty("ExplodeEffectScale");
		float32 scale = prop.GetValue<float32>();
		prop = GetProperty("Shape");
		Vector2* shape = prop.GetValue<Vector2*>();
		prop = GetProperty("ShapeLength");
		int32 shapeLen = prop.GetValue<uint32>();
		assert(shapeLen);
		float32 radius = 0;
		Vector2 center = Vector2_Zero;
		for (int32 i=0; i<shapeLen; ++i)
			center += shape[i];
		center *= 1.0f / shapeLen;
		for (int32 i=0; i<shapeLen; ++i)
		{
			float32 dist = MathUtils::Distance(center, shape[i]);
			if (dist > radius)
				radius = dist;
		}
		ps->SetScale(radius * scale);
		prop = GetProperty("AbsolutePosition");
		ps->MoveTo(prop.GetValue<Vector2>());
		ps->Fire();
	}

	mHitpoints = 0;

	// destroy everything on this platform
	for (EntityList::iterator it=mItems.begin(); it!=mItems.end(); ++it)
		it->PostMessage(EntityMessage::TYPE_DIE);

	// detach the platform from the body and delete it
	if (mParentShip.IsValid())
	{
		// apply some force from the explosion to the parent ship
		//TODO aplikovat silu na vsechno v dosahu
		b2Body* body;
		GetOwner().PostMessage(EntityMessage::TYPE_GET_PHYSICS_BODY, &body);
		prop = GetProperty("AbsolutePosition");
		Vector2 myPos = prop.GetValue<Vector2>();
		prop = mParentShip.GetProperty("AbsolutePosition");
		Vector2 forceDir = prop.GetValue<Vector2&>() - myPos;
		float32 distSq = forceDir.LengthSquared();
		body->ApplyForce(EXPLOSION_FORCE_RATIO / distSq * forceDir, myPos);

		mParentShip.PostMessage(EntityMessage::TYPE_REMOVE_PLATFORM, GetOwnerPtr());
		bool recreate = false;
		GetOwner().PostMessage(EntityMessage::TYPE_DETACH_PLATFORM, &recreate);
	}

	// delete the entity
	gEntityMgr.DestroyEntity(GetOwner());
}