#include "Common.h"
#include "CmpEngine.h"

using namespace EntitySystem;

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
	case EntityMessage::TYPE_DRAW_INNER:
		Draw();
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
	EntityHandle platform;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_PARENT, &platform);
	float32 platformAngle;
	platform.PostMessage(EntityMessage::TYPE_GET_ANGLE, &platformAngle);
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
