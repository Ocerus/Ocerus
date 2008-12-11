#include "Common.h"
#include "CmpEngine.h"
#include "Box2D.h"
#include "../../Core/Game.h"

using namespace EntitySystem;

#define PICK_CIRCLE_RADIUS 0.5f
#define POWER_RATIO 0.005f
#define PICTURE_SCALE 0.5f
#define STABILIZATION_RATIO 0.0002f

void EntitySystem::CmpEngine::Init( ComponentDescription& desc )
{
	SetDefaultAngle(desc.GetNextItem()->GetData<float32>());
	SetRelativeAngle(desc.GetNextItem()->GetData<float32>());
	EntityHandle blueprints;
	PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
	mPower = 0;
}

void EntitySystem::CmpEngine::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpEngine::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::TYPE_UPDATE_PHYSICS_SERVER:
		{
			EntityHandle platform;
			PostMessage(EntityMessage::TYPE_GET_PARENT, &platform);
			b2Body* platformBody;
			platform.PostMessage(EntityMessage::TYPE_GET_PHYSICS_BODY, &platformBody);
			Vector2 myPos;
			PostMessage(EntityMessage::TYPE_GET_POSITION, &myPos);
			Vector2 forceDir = MathUtils::VectorFromAngle(GetAbsoluteAngle());
			platformBody->ApplyForce(-POWER_RATIO * mPower * forceDir, myPos);
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
			float32 arcAngle;
			EntityHandle blueprints;
			PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
			blueprints.PostMessage(EntityMessage::TYPE_GET_ARC_ANGLE, &arcAngle);
			float32 relAngle = *(float32*)msg.data;
			relAngle = relAngle - GetAbsoluteDefaultAngle();
			if (MathUtils::IsAngleInRange(relAngle, -arcAngle, arcAngle))
				mRelativeAngle = MathUtils::WrapAngle(relAngle);
			else if (MathUtils::AngleDistance(relAngle, -arcAngle) < MathUtils::AngleDistance(relAngle, arcAngle))
				mRelativeAngle = MathUtils::WrapAngle(-arcAngle);
			else
				mRelativeAngle = MathUtils::WrapAngle(arcAngle);
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_ANGLE:
		assert(msg.data);
		*(float32*)msg.data = GetAbsoluteAngle();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_SET_POWER_RATIO:
		assert(msg.data);
		{
			uint32 maxPower;
			EntityHandle blueprints;
			PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
			blueprints.PostMessage(EntityMessage::TYPE_GET_MAX_POWER, &maxPower);
			mPower = MathUtils::Round(maxPower * MathUtils::Clamp(*(float32*)msg.data, 0.0f, 1.0f));
		}
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
	RegisterProperty<float32>("RelativeAngle", &GetRelativeAngle, &SetRelativeAngle, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("DefaultAngle", &GetDefaultAngle, &SetDefaultAngle, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("AbsoluteAngle", &GetAbsoluteAngle, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("AbsoluteDefaultAngle", &GetAbsoluteDefaultAngle, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

void EntitySystem::CmpEngine::Draw( void ) const
{
	Vector2 pos;
	PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
	GfxSystem::TexturePtr img = gResourceMgr.GetResource("ShipParts/engine0.png");
	gGfxRenderer.DrawImageWithConversion(img, pos, GfxSystem::ANCHOR_HCENTER|GfxSystem::ANCHOR_VCENTER, 
		GetAbsoluteAngle(), 255, PICTURE_SCALE);
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
	Vector2 indicator = MathUtils::VectorFromAngle(GetAbsoluteAngle(), (float32)circleRadius*(float32)mPower/(float32)maxPower);
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

