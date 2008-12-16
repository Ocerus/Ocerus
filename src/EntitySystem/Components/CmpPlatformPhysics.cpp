#include "Common.h"
#include "CmpPlatformPhysics.h"
#include "Box2D.h"
#include "../../Utility/DataContainer.h"
#include "../../Core/Game.h"

using namespace EntitySystem;

#define LINEAR_DAMPING 0.1f
#define ANGULAR_DAMPING 0.5f

void EntitySystem::CmpPlatformPhysics::Init( void )
{
	mBody = 0;
	mShape = 0;
	mRelativePosition.SetZero();

	mInitBodyAngle = 0.0f;
	mInitBodyPosition.SetZero();
	mInitShapeAngle = 0.0f;
	mInitShapeFlip = false;
}


void EntitySystem::CmpPlatformPhysics::PostInit( void )
{
	EntityHandle ship;
	PostMessage(EntityMessage::TYPE_GET_PARENT, &ship);

	// create body
	if (ship.IsValid())
	{
		ship.PostMessage(EntityMessage::TYPE_GET_PHYSICS_BODY, &mBody);
		ship.PostMessage(EntityMessage::TYPE_ADD_PLATFORM, GetOwnerPtr());
	}
	else
	{
		b2BodyDef bodyDef;
		bodyDef.position = mInitBodyPosition;
		bodyDef.angle = mInitBodyAngle;
		bodyDef.userData = GetOwnerPtr();
		bodyDef.angularDamping = ANGULAR_DAMPING;
		bodyDef.linearDamping = LINEAR_DAMPING;
		mBody = gApp.GetCurrentGame()->GetPhysics()->CreateBody(&bodyDef);
	}

	// create shape
	b2PolygonDef shapeDef;
	EntityHandle blueprints;
	PostMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints);
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
	bool flip = mInitShapeFlip;
	float32 angle = mInitShapeAngle;
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

void EntitySystem::CmpPlatformPhysics::Clean( void )
{

}

EntityMessage::eResult EntitySystem::CmpPlatformPhysics::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_POST_INIT:
		PostInit();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_BODY_POSITION: // I need this for drawing
		assert(msg.data);
		assert(mBody);
		((Vector2*)msg.data)->Set(mBody->GetPosition());
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_POSITION:
		assert(msg.data);
		((Vector2*)msg.data)->Set(GetAbsolutePosition());
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_RELATIVE_POSITION:
		assert(msg.data);
		((Vector2*)msg.data)->Set(GetRelativePosition());
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
	RegisterProperty<Vector2&>("RelativePosition", &GetRelativePosition, &SetRelativePosition, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2>("InitBodyPosition", 0, &SetInitBodyPosition, PROPACC_INIT);
	RegisterProperty<float32>("InitBodyAngle", 0, &SetInitBodyAngle, PROPACC_INIT);
	RegisterProperty<float32>("InitShapeAngle", 0, &SetInitShapeAngle, PROPACC_INIT);
	RegisterProperty<bool>("InitShapeFlip", 0, &SetInitShapeFlip, PROPACC_INIT);
	RegisterProperty<Vector2>("AbsolutePosition", &GetAbsolutePosition, &SetAbsolutePosition, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
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
	PostMessage(EntityMessage::TYPE_GET_PARENT, &ship);
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
