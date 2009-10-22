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
	mRelativePosition.SetZero();

	mInitBodyAngle = 0.0f;
	mInitBodyPosition.SetZero();
	mInitShapeAngle = 0.0f;
	mInitShapeFlip = false;
}


void EntityComponents::CmpPlatformPhysics::Init( void )
{
	PropertyHolder prop = GetProperty("ParentShip");
	EntityHandle ship = prop.GetValue<EntityHandle>();
	CreateBody(ship.IsValid());
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
		bodyDef.position = mInitBodyPosition;
		bodyDef.angle = mInitBodyAngle;
		bodyDef.userData = GetOwnerPtr();
		bodyDef.angularDamping = ANGULAR_DAMPING;
		bodyDef.linearDamping = LINEAR_DAMPING;
		mBody = GlobalProperties::Get<b2World>("CurrentPhysics").CreateBody(&bodyDef);
	}

	// create shape
	b2PolygonDef shapeDef;
	EntityHandle blueprints = GetProperty("Blueprints").GetValue<EntityHandle>();
	EntityHandle material = blueprints.GetProperty("Material").GetValue<EntityHandle>();;
	// set density
	float32 density = material.GetProperty("Density").GetValue<float32>();;
	shapeDef.density = density;
	// retrieve original shape
	Vector2* poly = blueprints.GetProperty("Shape").GetValue<Vector2*>();
	uint32 polyLen = blueprints.GetProperty("ShapeLength").GetValue<uint32>();
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
	if (!hasParentShip)
		mBody->SetMassFromShapes();
}

void EntityComponents::CmpPlatformPhysics::Destroy( void )
{
	PropertyHolder prop = GetProperty("ParentShip");
	if (mBody && !prop.GetValue<EntityHandle>().IsValid())
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
	RegisterProperty<Vector2>("RelativePosition", &CmpPlatformPhysics::GetRelativePosition, &CmpPlatformPhysics::SetRelativePosition, PA_INIT | PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<Vector2>("InitBodyPosition", &CmpPlatformPhysics::GetInitBodyPosition, &CmpPlatformPhysics::SetInitBodyPosition, PA_INIT);
	RegisterProperty<float32>("InitBodyAngle", &CmpPlatformPhysics::GetInitBodyAngle, &CmpPlatformPhysics::SetInitBodyAngle, PA_INIT);
	RegisterProperty<float32>("InitShapeAngle", &CmpPlatformPhysics::GetInitShapeAngle, &CmpPlatformPhysics::SetInitShapeAngle, PA_INIT);
	RegisterProperty<bool>("InitShapeFlip", &CmpPlatformPhysics::GetInitShapeFlip, &CmpPlatformPhysics::SetInitShapeFlip, PA_INIT);
	RegisterProperty<Vector2>("AbsolutePosition", &CmpPlatformPhysics::GetAbsolutePosition, &CmpPlatformPhysics::SetAbsolutePosition,  PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<float32>("Angle", &CmpPlatformPhysics::GetAngle, &CmpPlatformPhysics::SetAngle, PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<Vector2>("LinearVelocity", &CmpPlatformPhysics::GetLinearVelocity, &CmpPlatformPhysics::SetLinearVelocity, PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<Vector2*>("Shape", &CmpPlatformPhysics::GetShape, 0, PA_EDIT_READ | PA_SCRIPT_READ);
	RegisterProperty<uint32>("ShapeLength", &CmpPlatformPhysics::GetShapeLength, 0, PA_EDIT_READ | PA_SCRIPT_READ);

	AddComponentDependency(CT_PLATFORM_LOGIC);
}

Vector2 EntityComponents::CmpPlatformPhysics::GetAbsolutePosition( void ) const
{
	OC_DASSERT(mBody);
	return mBody->GetPosition() + MathUtils::Multiply(Matrix22(mBody->GetAngle()), mRelativePosition);
}

void EntityComponents::CmpPlatformPhysics::SetAbsolutePosition( Vector2 pos )
{
	OC_DASSERT(mBody);
	EntityHandle ship = GetProperty("ParentShip").GetValue<EntityHandle>();
	OC_DASSERT_MSG(!ship.IsValid(), "SetAbsolutePosition can be used for free platforms only");
	mBody->SetXForm(pos, mBody->GetAngle());
}

float32 EntityComponents::CmpPlatformPhysics::GetAngle( void ) const
{
	OC_DASSERT(mBody);
	return mBody->GetAngle();
}

void EntityComponents::CmpPlatformPhysics::SetAngle( const float32 angle )
{
	OC_DASSERT(mBody);
	mBody->SetXForm(mBody->GetPosition(), angle);
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

