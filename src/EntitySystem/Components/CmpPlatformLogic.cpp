#include "Common.h"
#include "CmpPlatformLogic.h"
#include <Box2D.h>
#include "DataContainer.h"

#define EXPLOSION_FORCE_RATIO 10.0f
#define KNOCKBACK_DETACH_RATIO 0.5f

void EntityComponents::CmpPlatformLogic::Create(void)
{
	mBlueprints.Invalidate();
	mHitpoints = 0;
	mParentShip.Invalidate();
	mItems.clear();

	mPickCircleCenter = Vector2_Zero;
	mPickCircleRadius = 0;
}

void EntityComponents::CmpPlatformLogic::Destroy(void)
{

}

EntityMessage::eResult EntityComponents::CmpPlatformLogic::HandleMessage(const EntityMessage& msg)
{
	switch(msg.type)
	{
	case EntityMessage::POST_INIT:
		{
			ComputePickStuff();
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::DESTROY:
		Die();
		return EntityMessage::RESULT_OK;
	case EntityMessage::MOUSE_PICK:
		OC_DASSERT(msg.data);
		{
			Vector2 pos = GetProperty("AbsolutePosition").GetValue<Vector2>();
			float32 angle = GetProperty("Angle").GetValue<float32>();
			((EntityPicker*)msg.data)->Update(GetOwner(), MathUtils::Multiply(Matrix22(angle), mPickCircleCenter) + pos, mPickCircleRadius);
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::DRAW_OVERLAY:
		OC_DASSERT(msg.data);
		DrawSelectionOverlay(*(bool*)msg.data);
		return EntityMessage::RESULT_OK;
	default:
		break;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::CmpPlatformLogic::RegisterReflection()
{
	RegisterProperty<uint32>("Hitpoints", &CmpPlatformLogic::GetHitpoints, &CmpPlatformLogic::SetHitpoints, PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<EntityHandle>("Blueprints", &CmpPlatformLogic::GetBlueprints, &CmpPlatformLogic::SetBlueprints, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<EntityHandle>("ParentShip", &CmpPlatformLogic::GetParentShip, &CmpPlatformLogic::SetParentShip, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ);
}

void EntityComponents::CmpPlatformLogic::DrawSelectionOverlay( const bool hover ) const
{
	Vector2 pos = GetProperty("AbsolutePosition").GetValue<Vector2>();
	float32 angle = GetProperty("Angle").GetValue<float32>();
	GfxSystem::Color color(255,0,0,180);
	if (hover)
		color = GfxSystem::Color(255,255,255,180);
	gGfxRenderer.DrawCircle(gGfxRenderer.WorldToScreen(MathUtils::Multiply(Matrix22(angle), mPickCircleCenter) + pos), gGfxRenderer.WorldToScreenScalar(mPickCircleRadius),
		GfxSystem::Color::NullColor, GfxSystem::Pen(color));
}

void EntityComponents::CmpPlatformLogic::Die( void )
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
		OC_ASSERT(shapeLen);
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
		it->PostMessage(EntityMessage::DESTROY);

	// delete the entity
	gEntityMgr.DestroyEntity(GetOwner());
}

void EntityComponents::CmpPlatformLogic::ComputePickStuff( void )
{
	// compute pick stuff
	Vector2* shape = GetProperty("Shape").GetValue<Vector2*>();
	int32 shapeLen = GetProperty("ShapeLength").GetValue<uint32>();
	// compute center
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
}
