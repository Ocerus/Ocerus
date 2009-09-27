#include "Common.h"
#include "CmpShipPhysics.h"
#include "Box2D.h"
#include "../../Core/Game.h"
#include "../../Core/Application.h"

using namespace EntitySystem;

#define LINEAR_DAMPING 0.1f
#define ANGULAR_DAMPING 0.5f

void EntitySystem::CmpShipPhysics::Init( void )
{
	mBody = 0;
	mInitBodyAngle = 0.0f;
	mInitBodyPosition.SetZero();
}

void EntitySystem::CmpShipPhysics::PostInit( void )
{
	b2BodyDef bodyDef;
	bodyDef.position = mInitBodyPosition;
	bodyDef.angle = mInitBodyAngle;
	bodyDef.userData = GetOwnerPtr();
	bodyDef.angularDamping = ANGULAR_DAMPING;
	bodyDef.linearDamping = LINEAR_DAMPING;
	mBody = gApp.GetCurrentGame()->GetPhysics()->CreateBody(&bodyDef);
}

void EntitySystem::CmpShipPhysics::Clean( void )
{
	if (mBody)
	{
		gApp.GetCurrentGame()->GetPhysics()->DestroyBody(mBody);
		mBody = 0;
	}
}

EntityMessage::eResult EntitySystem::CmpShipPhysics::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_POST_INIT:
		PostInit();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_POSITION:
		BS_DASSERT(msg.data);
		((Vector2*)msg.data)->Set(GetAbsolutePosition());
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_ANGLE:
		BS_DASSERT(msg.data);
		*(float32*)msg.data = GetAngle();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_PHYSICS_BODY:
		BS_DASSERT(msg.data);
		BS_DASSERT(mBody);
		*(b2Body**)msg.data = mBody;
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_PHYSICS_UPDATE_MASS:
		BS_DASSERT(mBody);
		mBody->SetMassFromShapes();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpShipPhysics::RegisterReflection()
{
	RegisterProperty<Vector2&>("AbsolutePosition", &GetAbsolutePosition, &SetAbsolutePosition, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Angle", &GetAngle, &SetAngle, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2>("InitBodyPosition", 0, &SetInitBodyPosition, PROPACC_INIT);
	RegisterProperty<float32>("InitBodyAngle", 0, &SetInitBodyAngle, PROPACC_INIT);
}

Vector2& EntitySystem::CmpShipPhysics::GetAbsolutePosition( void ) const
{
	BS_DASSERT(mBody);
	return const_cast<Vector2&>(mBody->GetPosition());
}

void EntitySystem::CmpShipPhysics::SetAbsolutePosition( Vector2& pos )
{
	BS_DASSERT(mBody);
	mBody->SetXForm(pos, mBody->GetAngle());
}

float32 EntitySystem::CmpShipPhysics::GetAngle( void ) const
{
	BS_DASSERT(mBody);
	return mBody->GetAngle();
}

void EntitySystem::CmpShipPhysics::SetAngle( const float32 angle )
{
	BS_DASSERT(mBody);
	mBody->SetXForm(mBody->GetPosition(), angle);
}

