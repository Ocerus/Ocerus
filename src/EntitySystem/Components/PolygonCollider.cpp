#include "Common.h"
#include "PolygonCollider.h"
#include <Box2D.h>

void EntityComponents::PolygonCollider::Create( void )
{
	mShape = 0;
	mDensity = 0;
	mSensorBody = 0;
}

void EntityComponents::PolygonCollider::Destroy( void )
{
	// the shape is destroyed automatically after the body it is attached to is destroyed
	if (mSensorBody)
	{
		GlobalProperties::Get<Physics>("Physics").DestroyBody(mSensorBody);
		mSensorBody = 0;
	}
}

EntityMessage::eResult EntityComponents::PolygonCollider::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::POST_INIT: // we have to wait until the physical bodies are inited, that's why we're using POST_INIT
		Init();
		return EntityMessage::RESULT_OK;
	case EntityMessage::SYNC_PRE_PHYSICS:	
		if (mSensorBody)
		{
			mSensorBody->SetTransform(GetOwner().GetProperty("Position").GetValue<Vector2>(), 
				GetOwner().GetProperty("Angle").GetValue<float32>());
		}
		return EntityMessage::RESULT_OK;
	default:
		break;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::PolygonCollider::RegisterReflection( void )
{
	RegisterProperty<float32>("Density", &PolygonCollider::GetDensity, &PolygonCollider::SetDensity, PA_FULL_ACCESS, "");
	RegisterProperty<Array<Vector2>*>("Polygon", &PolygonCollider::GetPolygon, &PolygonCollider::SetPolygon, PA_FULL_ACCESS, "");

	// we need the transform to be able to have the position and angle ready while creating the body
	AddComponentDependency(CT_Transform);
}

void EntityComponents::PolygonCollider::Init( void )
{
	// define the shape
	b2PolygonShape shapeDef;
	shapeDef.Set(mPolygon.GetRawArrayPtr(), mPolygon.GetSize());
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shapeDef;
	fixtureDef.density = mDensity;
	fixtureDef.userData = GetOwnerPtr();

	// find a parent body
	PhysicalBody* body = 0;
	if (GetOwner().HasProperty("PhysicalBody"))
	{
		PropertyHolder bodyProp = GetOwner().GetProperty("PhysicalBody");
		body = bodyProp.GetValue<PhysicalBody*>();
	}
	else
	{
		// enable collision, but disable collision response
		fixtureDef.isSensor = true;

		// create a dummy body by ourself
		b2BodyDef bodyDef;
		bodyDef.type = b2_staticBody;
		bodyDef.position = GetOwner().GetProperty("Position").GetValue<Vector2>();
		bodyDef.angle = GetOwner().GetProperty("Angle").GetValue<float32>();
		bodyDef.fixedRotation = true;
		bodyDef.userData = GetOwnerPtr();
		mSensorBody = GlobalProperties::Get<Physics>("Physics").CreateBody(&bodyDef);

		body = mSensorBody;
	}

	// create the shape
	mShape = body->CreateFixture(&fixtureDef);
}
