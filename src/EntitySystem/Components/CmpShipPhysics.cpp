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
	case EntityMessage::TYPE_GET_PHYSICS_BODY:
		assert(msg.data);
		assert(mBody);
		*(b2Body**)msg.data = mBody;
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpShipPhysics::RegisterReflection()
{

}