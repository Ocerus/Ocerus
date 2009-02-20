#include "Common.h"
#include "CmpPlatformItem.h"

using namespace EntitySystem;


void EntitySystem::CmpPlatformItem::Init( void )
{
	mBlueprints.Invalidate();
	mParentPlatform.Invalidate();
	mRelativePosition.SetZero();
	mHitpoints = 0;
}

void EntitySystem::CmpPlatformItem::Clean( void )
{

}

EntityMessage::eResult EntitySystem::CmpPlatformItem::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_POST_INIT:
		mBlueprints.PostMessage(EntityMessage::TYPE_GET_MAX_HITPOINTS, &mHitpoints);
		mParentPlatform.PostMessage(EntityMessage::TYPE_ADD_PLATFORM_ITEM, GetOwnerPtr());
		GetOwner().SetTeam(mParentPlatform.GetTeam());
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DIE:
		gEntityMgr.DestroyEntity(GetOwner());
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_PARENT:
		DASSERT(msg.data);
		*(EntityHandle*)msg.data = GetParentPlatform();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_BLUEPRINTS:
		DASSERT(msg.data);
		*(EntityHandle*)msg.data = GetBlueprints();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_POSITION:
		DASSERT(msg.data);
		((Vector2*)msg.data)->Set(GetAbsolutePosition());
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_HITPOINTS:
		DASSERT(msg.data);
		*(uint32*)msg.data = GetHitpoints();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpPlatformItem::RegisterReflection( void )
{
	RegisterProperty<Vector2&>("RelativePosition", &GetRelativePosition, &SetRelativePosition, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2>("AbsolutePosition", &GetAbsolutePosition, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("Hitpoints", &GetHitpoints, &SetHitpoints, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("ParentPlatform", &GetParentPlatform, &SetParentPlatform, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("Blueprints", &GetBlueprints, &SetBlueprints, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
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

