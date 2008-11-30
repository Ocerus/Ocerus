#include "Common.h"
#include "CmpPlatformPhysics.h"
#include "Box2D.h"
#include "../../Utility/DataContainer.h"
#include "../../Core/Game.h"

using namespace EntitySystem;

void EntitySystem::CmpPlatformPhysics::Init( ComponentDescription& desc )
{
	mBody = 0;
	mShape = 0;
	EntityHandle ship;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_PARENT, &ship);

	// create body
	if (ship.IsValid())
	{
		ship.PostMessage(EntityMessage::TYPE_GET_PHYSICS_BODY, &mBody);
		ship.PostMessage(EntityMessage::TYPE_ADD_PLATFORM, GetOwnerPtr());
	}
	else
	{
		b2BodyDef bodyDef;
		bodyDef.position = desc.GetNextItem()->GetData<Vector2>();
		bodyDef.angle = desc.GetNextItem()->GetData<float32>();
		bodyDef.userData = GetOwnerPtr();
		mBody = gApp.GetCurrentGame()->GetPhysics()->CreateBody(&bodyDef);
	}

	// create shape
	b2PolygonDef shapeDef;
	EntityHandle blueprints;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
	EntityHandle material;
	blueprints.PostMessage(EntityMessage::TYPE_GET_MATERIAL, &material);
	// set density
	float32 density;
	material.PostMessage(EntityMessage::TYPE_GET_DENSITY, &density);
	shapeDef.density = density;
	// retrieve original shape
	DataContainer cont;
	blueprints.PostMessage(EntityMessage::TYPE_GET_POLYSHAPE, &cont);
	Vector2* poly = (Vector2*)cont.GetData();
	uint32 polyLen = cont.GetSize();
	// retrieve shape transformation info
	mRelativePosition = b2Vec2_zero;
	if (ship.IsValid())
		mRelativePosition = desc.GetNextItem()->GetData<Vector2>();
	bool flip = desc.GetNextItem()->GetData<bool>();
	float32 angle = desc.GetNextItem()->GetData<float32>();
	b2XForm xform(mRelativePosition, b2Mat22(angle));
	// create the shape
	for (int i=flip?(polyLen-1):(0); flip?(i>=0):(i<(int)polyLen); flip?(--i):(++i))
	{
		Vector2 vec = poly[i];
		if (flip)
			vec.y = -vec.y;
		shapeDef.vertices[shapeDef.vertexCount++] = b2Mul(xform, vec);
	}
	// set the entity handle so that we can identify the shape later
	shapeDef.userData = GetOwnerPtr();

	mShape = mBody->CreateShape(&shapeDef);

	// compute mass
	if (!ship.IsValid())
		mBody->SetMassFromShapes();
}

void EntitySystem::CmpPlatformPhysics::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpPlatformPhysics::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_GET_BODY_POSITION: // I need this for drawing
		assert(msg.data);
		assert(mBody);
		((Vector2*)msg.data)->Set(mBody->GetPosition());
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_POSITION:
		assert(msg.data);
		((Vector2*)msg.data)->Set(GetAbsolutePosition());
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
	case EntityMessage::TYPE_GET_PHYSICS_BODY:
		assert(msg.data);
		assert(mBody);
		*(b2Body**)msg.data = mBody;
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
	RegisterProperty<Vector2&>("RelativePosition", &GetRelativePosition, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2>("AbsolutePosition", &GetAbsolutePosition, &SetAbsolutePosition, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Angle", &GetAngle, &SetAngle, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

Vector2 EntitySystem::CmpPlatformPhysics::GetAbsolutePosition( void ) const
{
	assert(mBody);
	return mBody->GetPosition() + MathUtils::Multiply(Matrix22(mBody->GetAngle()), mRelativePosition);
}

void EntitySystem::CmpPlatformPhysics::SetAbsolutePosition( Vector2 pos )
{
	assert(mBody);
	EntityHandle ship;
	GetOwner().PostMessage(EntityMessage::TYPE_GET_PARENT, &ship);
	assert(!ship.IsValid() && "SetAbsolutePosition can be used for free platforms only");
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