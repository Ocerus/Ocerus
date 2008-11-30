#include "Common.h"
#include "CmpShipPhysics.h"
#include "Box2D.h"
#include "../../Core/Game.h"

using namespace EntitySystem;


void EntitySystem::CmpShipPhysics::Init( ComponentDescription& desc )
{
	mBody = 0;
	b2BodyDef bodyDef;
	bodyDef.position = desc.GetNextItem()->GetData<Vector2>();
	bodyDef.angle = desc.GetNextItem()->GetData<float32>();
	bodyDef.userData = GetOwnerPtr();
	mBody = gApp.GetCurrentGame()->GetPhysics()->CreateBody(&bodyDef);
}

void EntitySystem::CmpShipPhysics::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpShipPhysics::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
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