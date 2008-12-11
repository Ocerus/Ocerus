#include "Common.h"
#include "CmpPlatformItem.h"

using namespace EntitySystem;


void EntitySystem::CmpPlatformItem::Init( ComponentDescription& desc )
{
	SetBlueprints(desc.GetNextItem()->GetData<EntityHandle>());
	SetParentPlatform(desc.GetNextItem()->GetData<EntityHandle>());
	SetRelativePosition(desc.GetNextItem()->GetData<Vector2>());
	mBlueprints.PostMessage(EntityMessage::TYPE_GET_MAX_HITPOINTS, &mHitpoints);
	mParentPlatform.PostMessage(EntityMessage::TYPE_ADD_PLATFORM_ITEM, GetOwnerPtr());
}

void EntitySystem::CmpPlatformItem::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpPlatformItem::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_GET_PARENT:
		assert(msg.data);
		*(EntityHandle*)msg.data = GetParentPlatform();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_BLUEPRINTS:
		assert(msg.data);
		*(EntityHandle*)msg.data = GetBlueprints();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_POSITION:
		assert(msg.data);
		((Vector2*)msg.data)->Set(GetAbsolutePosition());
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_HITPOINTS:
		assert(msg.data);
		*(uint32*)msg.data = GetHitpoints();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpPlatformItem::RegisterReflection( void )
{
	RegisterProperty<Vector2&>("RelativePosition", &GetRelativePosition, &SetRelativePosition, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2>("AbsolutePosition", &GetAbsolutePosition, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("Hitpoints", &GetHitpoints, &SetHitpoints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("ParentPlatform", &GetParentPlatform, &SetParentPlatform, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("Blueprints", &GetBlueprints, &SetBlueprints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

Vector2 EntitySystem::CmpPlatformItem::GetAbsolutePosition( void ) const
{
	EntityHandle platform;
	PostMessage(EntityMessage::TYPE_GET_PARENT, &platform);
	Vector2 platformPos;
	platform.PostMessage(EntityMessage::TYPE_GET_POSITION, &platformPos);
	float32 platformAngle;
	platform.PostMessage(EntityMessage::TYPE_GET_ANGLE, &platformAngle);
	return platformPos + MathUtils::Multiply(Matrix22(platformAngle), mRelativePosition);
}