#include "Common.h"
#include "CmpEngine.h"
#include "Box2D.h"

using namespace EntitySystem;

#define PICK_CIRCLE_RADIUS 0.5f

void EntitySystem::CmpEngine::Init( ComponentDescription& desc )
{
	SetDefaultAngle(desc.GetNextItem()->GetData<float32>());
	SetRelativeAngle(desc.GetNextItem()->GetData<float32>());
	EntityHandle blueprints;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
	blueprints.PostMessage(EntityMessage::TYPE_GET_MAX_POWER, &mPower);
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
			GetOwner().PostMessage(EntityMessage::TYPE_GET_PARENT, &platform);
			b2Body* platformBody;
			platform.PostMessage(EntityMessage::TYPE_GET_PHYSICS_BODY, &platformBody);
			Vector2 myPos;
			GetOwner().PostMessage(EntityMessage::TYPE_GET_POSITION, &myPos);
			Vector2 force = MathUtils::Multiply(Matrix22(GetAbsoluteAngle()), Vector2(-0.00001f, 0.0f));
			//platformBody->ApplyForce(force, myPos);
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
	case EntityMessage::TYPE_DRAW_INNER:
		Draw();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DRAW_HOVER_OVERLAY:
		DrawHoverOverlay();
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

void EntitySystem::CmpEngine::DrawHoverOverlay( void ) const
{
	Vector2 pos;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_POSITION, &pos);
	gGfxRenderer.DrawCircle(gGfxRenderer.WorldToScreen(pos), gGfxRenderer.WorldToScreenScalar(PICK_CIRCLE_RADIUS), 
		GfxSystem::Color::NullColor, GfxSystem::Pen(GfxSystem::Color(255,255,255,180)));
}