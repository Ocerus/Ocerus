#include "Common.h"
#include "CmpWeapon.h"
#include "../../Core/Game.h"

using namespace EntitySystem;

#define PICK_CIRCLE_RADIUS 0.3f

void EntitySystem::CmpWeapon::Init( void )
{
	mTimeToReload = 0;
	mDefaultAngle = 0;
	mRelativeAngle = 0;
	mAmmoBlueprints.Invalidate();

	mWasSelected = false;
	mIsFiring = false;
	mTarget.Invalidate();
}

void EntitySystem::CmpWeapon::Clean( void )
{
}

EntityMessage::eResult EntitySystem::CmpWeapon::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::TYPE_START_SHOOTING:
		mIsFiring = true;
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_STOP_SHOOTING:
		mIsFiring = false;
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_SET_ANGLE:
		// note: the angle is absolute, but I need to convert it to an angle relative to the default angle
		BS_DASSERT(msg.data);
		{
			EntityMessage msg2(EntityMessage::TYPE_SET_RELATIVE_ANGLE);
			float32 relAngle = *(float32*)msg.data - GetAbsoluteDefaultAngle();
			msg2.data = &relAngle;
			return HandleMessage(msg2);
		}
	case EntityMessage::TYPE_GET_ANGLE:
		BS_DASSERT(msg.data);
		*(float32*)msg.data = GetAbsoluteAngle();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_RELATIVE_ANGLE:
		BS_DASSERT(msg.data);
		*(float32*)msg.data = GetRelativeAngle();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_SET_RELATIVE_ANGLE:
		BS_DASSERT(msg.data);
		{
			PropertyHolder prop;
			prop = GetProperty("Blueprints");
			EntityHandle blueprints = prop.GetValue<EntityHandle>();
			prop = blueprints.GetProperty("ArcAngle");
			float32 arcAngle = prop.GetValue<float32>();
			float32 relAngle = *(float32*)msg.data;
			if (MathUtils::IsAngleInRange(relAngle, -arcAngle, arcAngle))
				mRelativeAngle = MathUtils::WrapAngle(relAngle);
			else if (MathUtils::AngleDistance(relAngle, -arcAngle) < MathUtils::AngleDistance(relAngle, arcAngle))
				mRelativeAngle = MathUtils::WrapAngle(-arcAngle);
			else
				mRelativeAngle = MathUtils::WrapAngle(arcAngle);
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_MOUSE_PICK:
		BS_DASSERT(msg.data);
		{
			Vector2 pos;
			PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
			((EntityPicker*)msg.data)->Update(GetOwner(), pos, PICK_CIRCLE_RADIUS);
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW_UNDERLAY:
		BS_DASSERT(msg.data);
		{
			bool hover = *(bool*)msg.data;
			DrawSelectionUnderlay(hover);
			if (!hover)
			{
				mWasSelected = true;
				Draw(true);
			}
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW_PLATFORM_ITEM:
		if (!mWasSelected)
			Draw(false);
		mWasSelected = false;
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW_OVERLAY:
		BS_DASSERT(msg.data);
		DrawSelectionOverlay(*(bool*)msg.data);
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_UPDATE_PHYSICS_SERVER:
		BS_DASSERT(msg.data);
		{
			// auto aim
			if (mTarget.Exists())
			{
				Vector2 myPos = ((PropertyHolder)GetProperty("AbsolutePosition")).GetValue<Vector2>();
				Vector2 tarPos = ((PropertyHolder)mTarget.GetProperty("AbsolutePosition")).GetValue<Vector2>();
				float32 angle = MathUtils::Angle(tarPos - myPos);
				HandleMessage(EntityMessage(EntityMessage::TYPE_SET_ANGLE, &angle));
			}
			else if (mTarget.IsValid())
			{
				mTarget.Invalidate();
				HandleMessage(EntityMessage(EntityMessage::TYPE_STOP_SHOOTING));
			}

			if (mTimeToReload > 0)
				mTimeToReload -= *(float32*)msg.data;
			if (mIsFiring && mTimeToReload <= 0)
				Fire();
		}
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpWeapon::RegisterReflection( void )
{
	RegisterProperty<float32>("TimeToReload", &GetTimeToReload, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("Ammo", &GetAmmo, &SetAmmo, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("Target", &GetTarget, &SetTarget, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("RelativeAngle", &GetRelativeAngle, &SetRelativeAngle, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("DefaultAngle", &GetDefaultAngle, &SetDefaultAngle, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("AbsoluteAngle", &GetAbsoluteAngle, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("AbsoluteDefaultAngle", &GetAbsoluteDefaultAngle, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<bool>("IsFiring", &IsFiring, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);

	AddComponentDependency(CT_PLATFORM_ITEM);
}

float32 EntitySystem::CmpWeapon::GetAbsoluteAngle( void ) const
{
	EntityHandle platform;
	PostMessage(EntityMessage::TYPE_GET_PARENT, &platform);
	float32 platformAngle;
	platform.PostMessage(EntityMessage::TYPE_GET_ANGLE, &platformAngle);
	return platformAngle + mDefaultAngle + mRelativeAngle;	
}

float32 EntitySystem::CmpWeapon::GetAbsoluteDefaultAngle( void ) const
{
	EntityHandle platform;
	PostMessage(EntityMessage::TYPE_GET_PARENT, &platform);
	float32 platformAngle;
	platform.PostMessage(EntityMessage::TYPE_GET_ANGLE, &platformAngle);
	return platformAngle + mDefaultAngle;	
}

void EntitySystem::CmpWeapon::Draw( const bool selected ) const
{
	Vector2 pos;
	PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
	EntityHandle blueprints;
	PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
	float32 angle = GetAbsoluteAngle();

	// draw the image
	GfxSystem::Color color(255,255,255);
	if (selected)
		color = GfxSystem::Color(255,0,255);
	StringKey tex;
	blueprints.PostMessage(EntityMessage::TYPE_GET_TEXTURE, &tex);
	float32 texAngle;
	blueprints.PostMessage(EntityMessage::TYPE_GET_TEXTURE_ANGLE, &texAngle);
	float32 texScale;
	blueprints.PostMessage(EntityMessage::TYPE_GET_TEXTURE_SCALE, &texScale);
	StringKey resGroup;
	blueprints.PostMessage(EntityMessage::TYPE_GET_RESOURCE_GROUP, &resGroup);
	GfxSystem::TexturePtr img = gResourceMgr.GetResource(resGroup, tex);
	gGfxRenderer.DrawImageWithConversion(img, pos, GfxSystem::ANCHOR_HCENTER|GfxSystem::ANCHOR_VCENTER, 
		GetAbsoluteAngle() + texAngle, color, texScale);
}

void EntitySystem::CmpWeapon::DrawSelectionOverlay( const bool hover ) const
{
	Vector2 pos;
	PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
	GfxSystem::Color color(255,0,0,60);
	if (hover)
	{
		color = GfxSystem::Color(255,255,255,90);
		gGfxRenderer.DrawCircle(gGfxRenderer.WorldToScreen(pos), gGfxRenderer.WorldToScreenScalar(PICK_CIRCLE_RADIUS), 
			GfxSystem::Color::NullColor, GfxSystem::Pen(color));
	}
}

void EntitySystem::CmpWeapon::DrawSelectionUnderlay( const bool hover ) const
{
	// draw an arch designating possible power and angle
	PropertyHolder prop;
	prop = GetProperty("Blueprints");
	EntityHandle blueprints = prop.GetValue<EntityHandle>();
	prop = GetProperty("AbsolutePosition");
	Vector2 pos = prop.GetValue<Vector2>();
	prop = blueprints.GetProperty("ArcAngle");
	float32 angleDiver = prop.GetValue<float32>();
	GfxSystem::Color color(10,10,80,15);
	float32 defAngle = GetAbsoluteDefaultAngle();
	int32 circleRadius = gApp.GetCurrentGame()->GetWeaponCircleRadius();
	GfxSystem::Point screenPos = gGfxRenderer.WorldToScreen(pos);
	gGfxRenderer.DrawCircle(screenPos, circleRadius, color, GfxSystem::Pen::NullPen/*GfxSystem::Pen(color)*/, defAngle - angleDiver, defAngle + angleDiver);

	// draw target angle and power vector
	Vector2 indicator = MathUtils::VectorFromAngle(GetAbsoluteAngle(), (float32)circleRadius);
	GfxSystem::Pen indicatorPen(GfxSystem::Color(200,0,20,70));
	gGfxRenderer.DrawLine(screenPos.x, screenPos.y, screenPos.x + MathUtils::Round(indicator.x), screenPos.y + MathUtils::Round(indicator.y), indicatorPen);

}

void EntitySystem::CmpWeapon::SetTarget( const EntityHandle tar )
{
	if (!tar.IsValid())
	{
		mTarget.Invalidate();
	}
	else
	{
		eEntityType type = tar.GetType();
		if ((type == ET_PLATFORM || type == ET_ENGINE || type == ET_WEAPON)
			&& GetOwner().GetTeam() != tar.GetTeam())
			mTarget = tar;
	}
}

void EntitySystem::CmpWeapon::Fire( void )
{
	if (!mAmmoBlueprints.IsValid() || !mIsFiring || mTimeToReload > 0)
		return;

	PropertyHolder prop = GetProperty("Blueprints");
	EntityHandle blueprints = prop.GetValue<EntityHandle>();
	
	EntityDescription desc;
	desc.Init(ET_PROJECTILE);
	desc.AddComponent(CT_PROJECTILE);
	PropertyList props;
	EntityHandle projectile = gEntityMgr.CreateEntity(desc, props);

	// set our team for the projectile
	// note: we need to do that before finishing init, otherwise we don't catch correctly the first collision
	projectile.SetTeam(GetOwner().GetTeam());

	props["Blueprints"].SetValue(mAmmoBlueprints);
	prop = blueprints.GetProperty("FireEffectDisplacement");
	float32 fireEffectDisp = prop.GetValue<float32>();
	prop = GetProperty("AbsoluteAngle");
	float32 angle = prop.GetValue<float32>();
	prop = blueprints.GetProperty("Spread");
	float32 spread = prop.GetValue<float32>();
	angle += MathUtils::Random(-spread, spread);
	prop = GetProperty("AbsolutePosition");
	Vector2 firePos = prop.GetValue<Vector2>() + MathUtils::VectorFromAngle(angle, fireEffectDisp);
	props["InitBodyPosition"].SetValue<Vector2>(firePos);
	prop = blueprints.GetProperty("FiringSpeed");
	float32 speed = prop.GetValue<float32>();
	prop = mAmmoBlueprints.GetProperty("SpeedRatio");
	speed *= prop.GetValue<float32>();
	prop = GetProperty("ParentPlatform");
	EntityHandle platform = prop.GetValue<EntityHandle>();
	prop = platform.GetProperty("LinearVelocity");
	props["InitBodySpeed"].SetValue<Vector2>(MathUtils::VectorFromAngle(angle, speed) + prop.GetValue<Vector2>());
	prop = blueprints.GetProperty("FiringDistance");
	float32 maxFireDist = prop.GetValue<float32>();
	prop = mAmmoBlueprints.GetProperty("DistanceRatio");
	maxFireDist *= prop.GetValue<float32>();
	props["MaxDistance"].SetValue(maxFireDist);
	projectile.FinishInit();

	// create the fire effect
	prop = blueprints.GetProperty("FireEffect");
	StringKey effect = prop.GetValue<StringKey>();
	prop = blueprints.GetProperty("ResourceGroup");
	StringKey resGroup = prop.GetValue<StringKey>();
	GfxSystem::ParticleSystemPtr ps = gPSMgr.SpawnPS(resGroup, effect);
	if (!ps.IsNull())
	{
		ps->SetAngle(angle);
		prop = blueprints.GetProperty("FireEffectScale");
		ps->SetScale(prop.GetValue<float32>());
		ps->FireAt(firePos.x, firePos.y);
	}

	// reset the cooldown
	prop = blueprints.GetProperty("ReloadTime");
	mTimeToReload = prop.GetValue<float32>();
}