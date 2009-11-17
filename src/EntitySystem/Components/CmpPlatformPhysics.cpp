#include "Common.h"
#include "CmpPlatformPhysics.h"
#include "Box2D.h"
#include "DataContainer.h"
#include "../../Core/Game.h"
#include "../../Core/Application.h"

#define LINEAR_DAMPING 0.1f
#define ANGULAR_DAMPING 0.5f

void EntityComponents::CmpPlatformPhysics::Create( void )
{
	mBody = 0;
	mShape = 0;

	mInitShapeAngle = 0.0f;
	mInitShapeFlip = false;
}


void EntityComponents::CmpPlatformPhysics::Init( void )
{
	CreateBody(false);
}

void EntityComponents::CmpPlatformPhysics::CreateBody( const bool hasParentShip )
{
	PropertyHolder prop;

	// create body
	if (hasParentShip)
	{
		prop = GetProperty("ParentShip");
		EntityHandle ship = prop.GetValue<EntityHandle>();
		mBody = ship.GetProperty("PhysicsBody").GetValue<b2Body*>();
	}
	else
	{
		b2BodyDef bodyDef;
		bodyDef.position = GetProperty("Position").GetValue<Vector2>();
		bodyDef.angle = GetProperty("Angle").GetValue<float32>();
		bodyDef.userData = GetOwnerPtr();
		bodyDef.angularDamping = ANGULAR_DAMPING;
		bodyDef.linearDamping = LINEAR_DAMPING;
		mBody = GlobalProperties::Get<b2World>("CurrentPhysics").CreateBody(&bodyDef);
	}

	// create shape
	b2PolygonDef shapeDef;
	// set density
	float32 density = GetProperty("Density").GetValue<float32>();;
	shapeDef.density = density;
	// retrieve original shape
	Array<Vector2>& poly = *GetProperty("Shape").GetValue<Array<Vector2>*>();
	// retrieve shape transformation info
	bool flip = mInitShapeFlip;
	float32 angle = mInitShapeAngle;
	b2XForm xform(Vector2_Zero, b2Mat22(angle));
	// create the shape
	for (int i=flip?(poly.GetSize()-1):(0); flip?(i>=0):(i<(int)poly.GetSize()); flip?(--i):(++i))
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
	if (!hasParentShip)
		mBody->SetMassFromShapes();
}

void EntityComponents::CmpPlatformPhysics::Destroy( void )
{
	if (mBody)
	{
		GlobalProperties::Get<b2World>("CurrentPhysics").DestroyBody(mBody);
		mBody = 0;
	}
}

EntityMessage::eResult EntityComponents::CmpPlatformPhysics::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::INIT:
		Init();
		return EntityMessage::RESULT_OK;
	default:
		break;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::CmpPlatformPhysics::RegisterReflection()
{
	RegisterProperty<float32>("InitShapeAngle", &CmpPlatformPhysics::GetInitShapeAngle, &CmpPlatformPhysics::SetInitShapeAngle, PA_INIT, "");
	RegisterProperty<bool>("InitShapeFlip", &CmpPlatformPhysics::GetInitShapeFlip, &CmpPlatformPhysics::SetInitShapeFlip, PA_INIT, "");
	RegisterProperty<Vector2>("LinearVelocity", &CmpPlatformPhysics::GetLinearVelocity, &CmpPlatformPhysics::SetLinearVelocity, PA_EDIT_READ | PA_SCRIPT_READ, "");
	//RegisterProperty<Vector2*>("Shape", &CmpPlatformPhysics::GetShape, 0, PA_EDIT_READ | PA_SCRIPT_READ);
	//RegisterProperty<uint32>("ShapeLength", &CmpPlatformPhysics::GetShapeLength, 0, PA_EDIT_READ | PA_SCRIPT_READ);
}

Vector2 EntityComponents::CmpPlatformPhysics::GetLinearVelocity( void ) const
{
	OC_DASSERT(mBody);
	return mBody->GetLinearVelocity();
}

void EntityComponents::CmpPlatformPhysics::SetLinearVelocity( const Vector2 linVel )
{
	OC_DASSERT(mBody);
	mBody->SetLinearVelocity(linVel);
}

Vector2* EntityComponents::CmpPlatformPhysics::GetShape( void ) const
{
	OC_DASSERT(mShape);
	b2PolygonShape* polyshape = (b2PolygonShape*)mShape;
	return const_cast<Vector2*>(polyshape->GetVertices());
}

uint32 EntityComponents::CmpPlatformPhysics::GetShapeLength( void ) const
{
	OC_DASSERT(mShape);
	b2PolygonShape* polyshape = (b2PolygonShape*)mShape;
	return polyshape->GetVertexCount();
}

