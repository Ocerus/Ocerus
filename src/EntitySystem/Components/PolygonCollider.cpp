#include "Common.h"
#include "PolygonCollider.h"
#include <Box2D.h>

void EntityComponents::PolygonCollider::Create( void )
{
	mShape = 0;
	mDensity = 1;
	mSensorBody = 0;
}

void EntityComponents::PolygonCollider::Destroy( void )
{
	DestroyShape();
}

EntityMessage::eResult EntityComponents::PolygonCollider::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::POST_INIT: // we have to wait until the physical bodies are inited, that's why we're using POST_INIT
		RecreateShape();
		return EntityMessage::RESULT_OK;
	case EntityMessage::SYNC_PRE_PHYSICS:
		if (mPolygon.GetSize() != 0 && !(mPolygonScale == GetOwner().GetProperty("Scale").GetValue<Vector2>()))
		{
			RecreateShape();
		}
		else if (mSensorBody)
		{
			mSensorBody->SetTransform(GetOwner().GetProperty("Position").GetValue<Vector2>(), 
				GetOwner().GetProperty("Angle").GetValue<float32>());
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::COMPONENT_DESTROYED:
		{
			eComponentType cmpType = (eComponentType)*msg.parameters.GetParameter(0).GetData<uint32>();
			if (cmpType == CT_DynamicBody || cmpType == CT_StaticBody)
			{
				// the body with the shape was already deleted
				mShape = 0;
				RecreateShape();
			}
			break;
		}
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

void EntityComponents::PolygonCollider::RecreateShape( void )
{
	// delete the old shape
	if (mShape)
	{
		DestroyShape();
	}

	if (mPolygon.GetSize() == 0) return;

	// define the shape
	b2PolygonShape shapeDef;
	mPolygonScale = GetOwner().GetProperty("Scale").GetValue<Vector2>();
	b2Vec2* vertices = new b2Vec2[mPolygon.GetSize()];
	for (int i=0; i<mPolygon.GetSize(); ++i)
	{
		vertices[i].x = mPolygonScale.x * mPolygon.GetRawArrayPtr()[i].x;
		vertices[i].y = mPolygonScale.y * mPolygon.GetRawArrayPtr()[i].y;
	}
	shapeDef.Set(vertices, mPolygon.GetSize());
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

void EntityComponents::PolygonCollider::DestroyShape( void )
{
	if (mSensorBody)
	{
		GlobalProperties::Get<Physics>("Physics").DestroyBody(mSensorBody);
		mSensorBody = 0;
	}
	else
	{
		// remove the shape from the body
		PhysicalBody* body = GetOwner().GetProperty("PhysicalBody").GetValue<PhysicalBody*>();
		if (body)
		{
			body->DestroyFixture(mShape);
		}
	}
	mShape = 0;
}

void EntityComponents::PolygonCollider::SetPolygon( Array<Vector2>* val )
{
	if (val->GetSize() < 3) return;
	mPolygon.CopyFrom(*val);
	if (GetOwner().IsInited()) RecreateShape();
}
