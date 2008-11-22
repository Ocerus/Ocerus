#include "Common.h"
#include "CmpPlatformPhysics.h"
#include "Box2D.h"
#include "../../Utility/DataContainer.h"

using namespace EntitySystem;


void EntitySystem::CmpPlatformPhysics::Init( ComponentDescription& desc )
{
	mBody = 0;
	mShape = 0;
}

void EntitySystem::CmpPlatformPhysics::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpPlatformPhysics::HandleMessage( const EntityMessage& msg )
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
	case EntityMessage::TYPE_GET_POLYSHAPE:
		assert(msg.data);
		assert(mShape);
		{
			b2PolygonShape* polyshape = (b2PolygonShape*)mShape;
			((DataContainer*)msg.data)->SetData((uint8*)polyshape->GetVertices(), polyshape->GetVertexCount());
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_PLATFORM_DETACH:
		assert(mBody);
		//TODO
		return EntityMessage::RESULT_ERROR;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpPlatformPhysics::RegisterReflection()
{
	RegisterProperty<Vector2&>("Position", &GetPosition, &SetPosition, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Angle", &GetAngle, &SetAngle, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

Vector2& EntitySystem::CmpPlatformPhysics::GetPosition( void ) const
{
	assert(mBody);
	return const_cast<Vector2&>(mBody->GetPosition());
}

void EntitySystem::CmpPlatformPhysics::SetPosition( Vector2& pos )
{
	assert(mBody);
	mBody->SetXForm(pos, mBody->GetAngle());
}

float32 EntitySystem::CmpPlatformPhysics::GetAngle( void ) const
{
	assert(mBody);
	return mBody->GetAngle();
}

void EntitySystem::CmpPlatformPhysics::SetAngle( const float32 angle )
{
	assert(mBody);
	mBody->SetXForm(mBody->GetPosition(), angle);
}