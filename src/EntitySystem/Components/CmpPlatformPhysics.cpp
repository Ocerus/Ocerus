#include "Common.h"
#include "CmpPlatformPhysics.h"
#include "Box2D.h"
#include "DataContainer.h"
#include "../../Core/Game.h"
#include "../../Core/Application.h"

#define LINEAR_DAMPING 0.1f
#define ANGULAR_DAMPING 0.5f

void EntityComponents::CmpPlatformPhysics::Init( void )
{
	mBody = 0;
	mShape = 0;
	mRelativePosition.SetZero();

	mInitBodyAngle = 0.0f;
	mInitBodyPosition.SetZero();
	mInitShapeAngle = 0.0f;
	mInitShapeFlip = false;
}


void EntityComponents::CmpPlatformPhysics::PostInit( void )
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
		mBody = GlobalProperties::Get<b2World>("CurrentPhysics").CreateBody(&bodyDef);
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
	if (!hasParentShip)
		mBody->SetMassFromShapes();
}

void EntityComponents::CmpPlatformPhysics::Clean( void )
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
	case EntityMessage::TYPE_POST_INIT:
		PostInit();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_BODY_POSITION: // I need this for drawing
		BS_DASSERT(msg.data);
		BS_DASSERT(mBody);
		((Vector2*)msg.data)->Set(mBody->GetPosition());
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_POSITION:
		BS_DASSERT(msg.data);
		((Vector2*)msg.data)->Set(GetAbsolutePosition());
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_RELATIVE_POSITION:
		BS_DASSERT(msg.data);
		((Vector2*)msg.data)->Set(GetRelativePosition());
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_ANGLE:
		BS_DASSERT(msg.data);
		*(float32*)msg.data = GetAngle();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_POLYSHAPE:
		BS_DASSERT(msg.data);
		BS_DASSERT(mShape);
		{
			b2PolygonShape* polyshape = (b2PolygonShape*)mShape;
			((DataContainer*)msg.data)->SetData((uint8*)polyshape->GetVertices(), polyshape->GetVertexCount());
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_PHYSICS_BODY:
		BS_DASSERT(msg.data);
		BS_DASSERT(mBody);
		*(b2Body**)msg.data = mBody;
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_DETACH_PLATFORM:
		BS_DASSERT(mBody);
		BS_DASSERT(msg.data);
		{
			// Note that I must invalidate the parent ship here to update the team properly.
			// CreateBody invokes collision checking and at that time it must be already updated.
			((PropertyHolder)GetProperty("ParentShip")).SetValue(EntityHandle::Null);
			PostMessage(EntityMessage::TYPE_UPDATE_TEAM);

			bool recreate = *(bool*)msg.data;
			if (recreate)
			{
				mInitBodyPosition = GetAbsolutePosition();
				mInitBodyAngle = GetAngle();
				Vector2 linVel = mBody->GetLinearVelocity();
				float32 angVel = mBody->GetAngularVelocity();
				mRelativePosition = Vector2_Zero;
				Vector2 shipPos = mBody->GetPosition();
				mBody->DestroyShape(mShape);
				mShape = 0;
				CreateBody(false);
				//mBody->SetAngularVelocity(angVel);
				//mBody->SetLinearVelocity(linVel);
				//TODO pridal sem detachnuty platforme trochu kick, at to ma grady
				mBody->SetAngularVelocity(angVel + MathUtils::Random(-1.0f, 1.0f));
				Vector2 dir = mBody->GetPosition() - shipPos;
				if (dir.x!=0 && dir.y!=0)
				{
					dir.Normalize();
					mBody->SetLinearVelocity(linVel + MathUtils::Random(0, 2.0f)*dir);
				}
			}
			else
			{
				mBody->DestroyShape(mShape);
				mShape = 0;
				mBody->SetMassFromShapes();
				mBody = 0; // prevent body to be deleted upon destruction of this entity
			}
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_EXPLOSION:
		BS_DASSERT(msg.data);
		{
			Vector2 myPos = GetAbsolutePosition();
			Vector2 exploPos = *(Vector2*)msg.data;
			Vector2 dir = myPos - exploPos;
			if (dir.x!=0 && dir.y!=0)
			{
				float32 lenSq = dir.LengthSquared();
				//TODO magic number...ale hlavne sila z exploze se ma delat pres query
				mBody->ApplyForce(100.0f/lenSq*dir, myPos);
			}
		}
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::CmpPlatformPhysics::RegisterReflection()
{
	RegisterProperty<Vector2&>("RelativePosition", &GetRelativePosition, &SetRelativePosition, PROPACC_INIT | PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2>("InitBodyPosition", &GetInitBodyPosition, &SetInitBodyPosition, PROPACC_INIT);
	RegisterProperty<float32>("InitBodyAngle", &GetInitBodyAngle, &SetInitBodyAngle, PROPACC_INIT);
	RegisterProperty<float32>("InitShapeAngle", &GetInitShapeAngle, &SetInitShapeAngle, PROPACC_INIT);
	RegisterProperty<bool>("InitShapeFlip", &GetInitShapeFlip, &SetInitShapeFlip, PROPACC_INIT);
	RegisterProperty<Vector2>("AbsolutePosition", &GetAbsolutePosition, &SetAbsolutePosition,  PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Angle", &GetAngle, &SetAngle, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2>("LinearVelocity", &GetLinearVelocity, &SetLinearVelocity, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2*>("Shape", &GetShape, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("ShapeLength", &GetShapeLength, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

Vector2 EntityComponents::CmpPlatformPhysics::GetAbsolutePosition( void ) const
{
	BS_DASSERT(mBody);
	return mBody->GetPosition() + MathUtils::Multiply(Matrix22(mBody->GetAngle()), mRelativePosition);
}

void EntityComponents::CmpPlatformPhysics::SetAbsolutePosition( Vector2 pos )
{
	BS_DASSERT(mBody);
	EntityHandle ship;
	PostMessage(EntityMessage::TYPE_GET_PARENT, &ship);
	BS_DASSERT_MSG(!ship.IsValid(), "SetAbsolutePosition can be used for free platforms only");
	mBody->SetXForm(pos, mBody->GetAngle());
}

float32 EntityComponents::CmpPlatformPhysics::GetAngle( void ) const
{
	BS_DASSERT(mBody);
	return mBody->GetAngle();
}

void EntityComponents::CmpPlatformPhysics::SetAngle( const float32 angle )
{
	BS_DASSERT(mBody);
	mBody->SetXForm(mBody->GetPosition(), angle);
}

Vector2 EntityComponents::CmpPlatformPhysics::GetLinearVelocity( void ) const
{
	BS_DASSERT(mBody);
	return mBody->GetLinearVelocity();
}

void EntityComponents::CmpPlatformPhysics::SetLinearVelocity( const Vector2 linVel )
{
	BS_DASSERT(mBody);
	mBody->SetLinearVelocity(linVel);
}

Vector2* EntityComponents::CmpPlatformPhysics::GetShape( void ) const
{
	BS_DASSERT(mShape);
	b2PolygonShape* polyshape = (b2PolygonShape*)mShape;
	return const_cast<Vector2*>(polyshape->GetVertices());
}

uint32 EntityComponents::CmpPlatformPhysics::GetShapeLength( void ) const
{
	BS_DASSERT(mShape);
	b2PolygonShape* polyshape = (b2PolygonShape*)mShape;
	return polyshape->GetVertexCount();
}

