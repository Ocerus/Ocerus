#include "Common.h"
#include "CmpEngine.h"
#include "Box2D.h"
#include "../../Core/Game.h"

using namespace EntitySystem;

#define PICK_CIRCLE_RADIUS 0.5f
#define POWER_RATIO 0.005f
#define STABILIZATION_RATIO 0.0002f

void EntitySystem::CmpEngine::Init( void )
{
	mDefaultAngle = 0.0f;
	mRelativeAngle = 0.0f;
	mPowerRatio = 0.0f;
}

void EntitySystem::CmpEngine::Clean( void )
{

}

EntityMessage::eResult EntitySystem::CmpEngine::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::TYPE_POST_INIT:
		{
			EntityHandle blueprints;
			PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
			StringKey effect;
			blueprints.PostMessage(EntityMessage::TYPE_GET_EFFECT, &effect);
			StringKey resGroup;
			blueprints.PostMessage(EntityMessage::TYPE_GET_RESOURCE_GROUP, &resGroup);
			mThrustPS = gPSMgr.SpawnPS(resGroup, effect);
			//mThrustPS->FireAt(0,0);
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_UPDATE_PHYSICS_SERVER:
		{
			EntityHandle platform;
			PostMessage(EntityMessage::TYPE_GET_PARENT, &platform);
			b2Body* platformBody;
			platform.PostMessage(EntityMessage::TYPE_GET_PHYSICS_BODY, &platformBody);
			Vector2 myPos;
			PostMessage(EntityMessage::TYPE_GET_POSITION, &myPos);
			Vector2 forceDir = MathUtils::VectorFromAngle(GetAbsoluteAngle());
			platformBody->ApplyForce(-POWER_RATIO * GetPower() * forceDir, myPos);
			Vector2 perpDir(-forceDir.y, forceDir.x);
			float32 perpVel = MathUtils::Dot(perpDir, platformBody->GetLinearVelocity());
			EntityHandle blueprints;
			PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
			uint32 stabRatio;
			blueprints.PostMessage(EntityMessage::TYPE_GET_STABILIZATION_RATIO, &stabRatio);
			platformBody->ApplyForce(-STABILIZATION_RATIO * stabRatio * perpVel * perpDir, platformBody->GetWorldCenter());
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_SET_ANGLE:
		// note: the angle is absolute, but I need to convert it to an angle relative to the default angle
		assert(msg.data);
		{
			EntityMessage msg2(EntityMessage::TYPE_SET_RELATIVE_ANGLE);
			float32 relAngle = *(float32*)msg.data - GetAbsoluteDefaultAngle();
			msg2.data = &relAngle;
			return HandleMessage(msg2);
		}
	case EntityMessage::TYPE_GET_ANGLE:
		assert(msg.data);
		*(float32*)msg.data = GetAbsoluteAngle();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_SET_RELATIVE_ANGLE:
		assert(msg.data);
		{
			float32 arcAngle;
			EntityHandle blueprints;
			PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
			blueprints.PostMessage(EntityMessage::TYPE_GET_ARC_ANGLE, &arcAngle);
			float32 relAngle = *(float32*)msg.data;
			if (MathUtils::IsAngleInRange(relAngle, -arcAngle, arcAngle))
				mRelativeAngle = MathUtils::WrapAngle(relAngle);
			else if (MathUtils::AngleDistance(relAngle, -arcAngle) < MathUtils::AngleDistance(relAngle, arcAngle))
				mRelativeAngle = MathUtils::WrapAngle(-arcAngle);
			else
				mRelativeAngle = MathUtils::WrapAngle(arcAngle);
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_RELATIVE_ANGLE:
		assert(msg.data);
		*(float32*)msg.data = GetRelativeAngle();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_SET_POWER_RATIO:
		assert(msg.data);
		SetPowerRatio(*(float32*)msg.data);
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_POWER_RATIO:
		assert(msg.data);
		*(float32*)msg.data = GetPowerRatio();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_MOUSE_PICK:
		assert(msg.data);
		{
			Vector2 pos;
			PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
			((EntityPicker*)msg.data)->Update(GetOwner(), pos, PICK_CIRCLE_RADIUS);
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW_UNDERLAY:
		assert(msg.data);
		DrawSelectionUnderlay(*(bool*)msg.data);
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW_PLATFORM_ITEM:
		Draw();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW_OVERLAY:
		assert(msg.data);
		DrawSelectionOverlay(*(bool*)msg.data);
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpEngine::RegisterReflection( void )
{
	RegisterProperty<uint32>("Power", &GetPower, &SetPower, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("RelativeAngle", &GetRelativeAngle, &SetRelativeAngle, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("DefaultAngle", &GetDefaultAngle, &SetDefaultAngle, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("AbsoluteAngle", &GetAbsoluteAngle, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("AbsoluteDefaultAngle", &GetAbsoluteDefaultAngle, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

void EntitySystem::CmpEngine::Draw( void ) const
{
	Vector2 pos;
	PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
	EntityHandle blueprints;
	PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
	float32 angle = GetAbsoluteAngle();

	// draw the particle effect
	float32 thrustScale;
	blueprints.PostMessage(EntityMessage::TYPE_GET_EFFECT_SCALE, &thrustScale);
	if (!mThrustPS.IsNull())
	{
		PropertyHolder prop;
		blueprints.GetProperty(prop, "ThrustEffectDisplacement");
		Vector2 disp = MathUtils::VectorFromAngle(angle, prop.GetValue<float32>());
		//mThrustPS->MoveTo(gGfxRenderer.WorldToScreen(pos + disp), true);
		mThrustPS->MoveTo(pos + disp, true);
		mThrustPS->SetScale(gGfxRenderer.WorldToScreenScale(thrustScale));
		mThrustPS->SetAngle(angle + MathUtils::HALF_PI);
		float32 powRat = GetPowerRatio();
		blueprints.GetProperty(prop, "ThrustEffectPowerScale");
		float32 powScale = prop.GetValue<float32>();	
		if (powRat < 0.1) mThrustPS->Stop();
		else mThrustPS->Fire();	
		mThrustPS->SetSpeed(powScale*powRat, powScale*powRat);
		mThrustPS->Render();
	}

	// draw the image
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
		GetAbsoluteAngle() + texAngle, 255, texScale);
}

float32 EntitySystem::CmpEngine::GetAbsoluteAngle( void ) const
{
	EntityHandle platform;
	PostMessage(EntityMessage::TYPE_GET_PARENT, &platform);
	float32 platformAngle;
	platform.PostMessage(EntityMessage::TYPE_GET_ANGLE, &platformAngle);
	return platformAngle + mDefaultAngle + mRelativeAngle;	
}

void EntitySystem::CmpEngine::DrawSelectionOverlay( const bool hover ) const
{
	Vector2 pos;
	PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
	GfxSystem::Color color(255,0,0,180);
	if (hover)
		color = GfxSystem::Color(255,255,255,180);
	gGfxRenderer.DrawCircle(gGfxRenderer.WorldToScreen(pos), gGfxRenderer.WorldToScreenScalar(PICK_CIRCLE_RADIUS), 
		GfxSystem::Color::NullColor, GfxSystem::Pen(color));
}

void EntitySystem::CmpEngine::DrawSelectionUnderlay( const bool hover ) const
{
	// draw an arch designating possible power and angle
	Vector2 pos;
	PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
	EntityHandle blueprints;
	PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
	float32 angleDiver;
	blueprints.PostMessage(EntityMessage::TYPE_GET_ARC_ANGLE, &angleDiver);
	GfxSystem::Color color(10,10,80,90);
	float32 defAngle = GetAbsoluteDefaultAngle();
	int32 circleRadius = gApp.GetCurrentGame()->GetEnginePowerCircleRadius();
	GfxSystem::Point screenPos = gGfxRenderer.WorldToScreen(pos);
	gGfxRenderer.DrawCircle(screenPos, circleRadius, color, GfxSystem::Pen::NullPen/*GfxSystem::Pen(color)*/, defAngle - angleDiver, defAngle + angleDiver);

	// draw target angle and power vector
	uint32 maxPower;
	blueprints.PostMessage(EntityMessage::TYPE_GET_MAX_POWER, &maxPower);
	Vector2 indicator = MathUtils::VectorFromAngle(GetAbsoluteAngle(), (float32)circleRadius*GetPowerRatio());
	GfxSystem::Pen indicatorPen(GfxSystem::Color(200,0,20,200));
	gGfxRenderer.DrawLine(screenPos.x, screenPos.y, screenPos.x + MathUtils::Round(indicator.x), screenPos.y + MathUtils::Round(indicator.y), indicatorPen);
}

float32 EntitySystem::CmpEngine::GetAbsoluteDefaultAngle( void ) const
{
	EntityHandle platform;
	PostMessage(EntityMessage::TYPE_GET_PARENT, &platform);
	float32 platformAngle;
	platform.PostMessage(EntityMessage::TYPE_GET_ANGLE, &platformAngle);
	return platformAngle + mDefaultAngle;	
}

uint32 EntitySystem::CmpEngine::GetPower( void ) const
{
	uint32 maxPower;
	EntityHandle blueprints;
	PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
	blueprints.PostMessage(EntityMessage::TYPE_GET_MAX_POWER, &maxPower);
	return MathUtils::Round(mPowerRatio * (float32)maxPower);
}

void EntitySystem::CmpEngine::SetPower( const uint32 pow )
{
	uint32 maxPower;
	EntityHandle blueprints;
	PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
	blueprints.PostMessage(EntityMessage::TYPE_GET_MAX_POWER, &maxPower);
	SetPowerRatio((float32)pow / (float32)maxPower);
}

void EntitySystem::CmpEngine::SetPowerRatio( const float32 powrat )
{
	mPowerRatio = MathUtils::Clamp(powrat, 0.0f, 1.0f);
}