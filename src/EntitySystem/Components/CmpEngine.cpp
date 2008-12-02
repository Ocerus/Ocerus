#include "Common.h"
#include "CmpEngine.h"
#include "Box2D.h"
#include "../../Core/Game.h"

using namespace EntitySystem;

#define PICK_CIRCLE_RADIUS 0.5f
#define POWER_RATIO -0.00000002f

void EntitySystem::CmpEngine::Init( ComponentDescription& desc )
{
	SetDefaultAngle(desc.GetNextItem()->GetData<float32>());
	SetRelativeAngle(desc.GetNextItem()->GetData<float32>());
	EntityHandle blueprints;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
	mPower = 0;

	mTargetPower = mPower;
	mTargetAngle = mRelativeAngle;
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
			EntityHandle blueprints;
			GetOwner().PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
			float32 angularSpeed;
			blueprints.PostMessage(EntityMessage::TYPE_GET_ANGULAR_SPEED, &angularSpeed);
			//TODO brat v potaz rychlost otaceni
			mRelativeAngle = mTargetAngle;

			mPower = mTargetPower;

			EntityHandle platform;
			GetOwner().PostMessage(EntityMessage::TYPE_GET_PARENT, &platform);
			b2Body* platformBody;
			platform.PostMessage(EntityMessage::TYPE_GET_PHYSICS_BODY, &platformBody);
			Vector2 myPos;
			GetOwner().PostMessage(EntityMessage::TYPE_GET_POSITION, &myPos);
			Vector2 force = MathUtils::Multiply(Matrix22(GetAbsoluteAngle()), Vector2(POWER_RATIO * mPower, 0.0f));
			platformBody->ApplyForce(force, myPos);
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_SET_TARGET_ANGLE:
		// note: the angle is absolute, but I need to convert it to an angle relative to the default angle
		assert(msg.data);
		{
			float32 arcAngle;
			EntityHandle blueprints;
			GetOwner().PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
			blueprints.PostMessage(EntityMessage::TYPE_GET_ARC_ANGLE, &arcAngle);
			float32 relAngle = MathUtils::WrapAngle((*(float32*)msg.data) - GetAbsoluteDefaultAngle());
			if (MathUtils::IsAngleInRange(relAngle, -arcAngle, arcAngle))
				mTargetAngle = relAngle;
			else if (MathUtils::AngleDistance(relAngle, -arcAngle) < MathUtils::AngleDistance(relAngle, arcAngle))
				mTargetAngle = MathUtils::WrapAngle(-arcAngle);
			else
				mTargetAngle = MathUtils::WrapAngle(arcAngle);
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_SET_TARGET_POWER_RATIO:
		assert(msg.data);
		{
			uint32 maxPower;
			EntityHandle blueprints;
			GetOwner().PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
			blueprints.PostMessage(EntityMessage::TYPE_GET_MAX_POWER, &maxPower);
			mTargetPower = MathUtils::Round(maxPower * MathUtils::Clamp(*(float32*)msg.data, 0.0f, 1.0f));
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_MOUSE_PICK:
		assert(msg.data);
		{
			Vector2 pos;
			GetOwner().PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
			((EntityPicker*)msg.data)->Update(GetOwner(), pos, PICK_CIRCLE_RADIUS);
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW_SELECTION_UNDERLAY:
	case EntityMessage::TYPE_DRAW_HOVER_UNDERLAY:
		DrawSelectionUnderlay();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW_INNER:
		Draw();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW_HOVER_OVERLAY:
		DrawSelectionOverlay(true);
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW_SELECTION_OVERLAY:
		DrawSelectionOverlay(false);
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
}

void EntitySystem::CmpEngine::Draw( void ) const
{
	Vector2 pos;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
	GfxSystem::TexturePtr img = gResourceMgr.GetResource("ShipParts/engine0.png");
	gGfxRenderer.DrawImageWithConversion(img, pos, GfxSystem::ANCHOR_HCENTER|GfxSystem::ANCHOR_VCENTER, GetAbsoluteAngle());
}

float32 EntitySystem::CmpEngine::GetAbsoluteAngle( void ) const
{
	EntityHandle platform;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_PARENT, &platform);
	float32 platformAngle;
	platform.PostMessage(EntityMessage::TYPE_GET_ANGLE, &platformAngle);
	return platformAngle + mDefaultAngle + mRelativeAngle;	
}

void EntitySystem::CmpEngine::DrawSelectionOverlay( const bool hover ) const
{
	Vector2 pos;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
	GfxSystem::Color color(255,0,0,180);
	if (hover)
		color = GfxSystem::Color(255,255,255,180);
	gGfxRenderer.DrawCircle(gGfxRenderer.WorldToScreen(pos), gGfxRenderer.WorldToScreenScalar(PICK_CIRCLE_RADIUS), 
		GfxSystem::Color::NullColor, GfxSystem::Pen(color));
}

void EntitySystem::CmpEngine::DrawSelectionUnderlay( void ) const
{
	// draw an arch designating possible power and angle
	Vector2 pos;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
	EntityHandle blueprints;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
	float32 angleDiver;
	blueprints.PostMessage(EntityMessage::TYPE_GET_ARC_ANGLE, &angleDiver);
	GfxSystem::Color color(10,10,80,80);
	float32 defAngle = GetAbsoluteDefaultAngle();
	gGfxRenderer.DrawCircle(gGfxRenderer.WorldToScreen(pos), gApp.GetCurrentGame()->GetEnginePowerCircleRadius(), color, /*GfxSystem::Pen::NullPen*/GfxSystem::Pen(color), defAngle - angleDiver, defAngle + angleDiver);
}

float32 EntitySystem::CmpEngine::GetAbsoluteDefaultAngle( void ) const
{
	EntityHandle platform;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_PARENT, &platform);
	float32 platformAngle;
	platform.PostMessage(EntityMessage::TYPE_GET_ANGLE, &platformAngle);
	return platformAngle + mDefaultAngle;	
}