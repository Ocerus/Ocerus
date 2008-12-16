#include "Common.h"
#include "CmpShipPhysics.h"
#include "Box2D.h"
#include "../../Core/Game.h"

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

}

EntityMessage::eResult EntitySystem::CmpShipPhysics::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_POST_INIT:
		PostInit();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_POSITION:
		assert(msg.data);
		((Vector2*)msg.data)->Set(GetPosition());
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_ANGLE:
		assert(msg.data);
		*(float32*)msg.data = GetAngle();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_PHYSICS_BODY:
		assert(msg.data);
		assert(mBody);
		*(b2Body**)msg.data = mBody;
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_PHYSICS_UPDATE_MASS:
		assert(mBody);
		mBody->SetMassFromShapes();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpShipPhysics::RegisterReflection()
{
	RegisterProperty<Vector2&>("Position", &GetPosition, &SetPosition, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Angle", &GetAngle, &SetAngle, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2>("InitBodyPosition", 0, &SetInitBodyPosition, PROPACC_INIT);
	RegisterProperty<float32>("InitBodyAngle", 0, &SetInitBodyAngle, PROPACC_INIT);
}

Vector2& EntitySystem::CmpShipPhysics::GetPosition( void ) const
{
	assert(mBody);
	return const_cast<Vector2&>(mBody->GetPosition());
}

void EntitySystem::CmpShipPhysics::SetPosition( Vector2& pos )
{
	assert(mBody);
	mBody->SetXForm(pos, mBody->GetAngle());
}

float32 EntitySystem::CmpShipPhysics::GetAngle( void ) const
{
	assert(mBody);
	return mBody->GetAngle();
}

void EntitySystem::CmpShipPhysics::SetAngle( const float32 angle )
{
	assert(mBody);
	mBody->SetXForm(mBody->GetPosition(), angle);
}

