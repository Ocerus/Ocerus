#include "Common.h"
#include "PolygonCollider.h"
#include <Box2D.h>

void EntityComponents::PolygonCollider::Create( void )
{
	mShape = 0;
	mDensity = 1;
	mSensorBody = 0;
	mFriction = 0.5f;
	mRestitution = 0.5f;
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
		if (!gEntityMgr.IsEntityPrototype(GetOwner())) RecreateShape();
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
				if (!gEntityMgr.IsEntityPrototype(GetOwner())) RecreateShape();
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
	RegisterProperty<float32>("Friction", &PolygonCollider::GetFriction, &PolygonCollider::SetFriction, PA_FULL_ACCESS, "");
	RegisterProperty<float32>("Restitution", &PolygonCollider::GetRestitution, &PolygonCollider::SetRestitution, PA_FULL_ACCESS, "");
	RegisterProperty<Array<Vector2>*>("Polygon", &PolygonCollider::GetPolygon, &PolygonCollider::SetPolygon, PA_FULL_ACCESS, "");
	RegisterProperty<PhysicalShape*>("PhysicalShape", &PolygonCollider::GetShape, 0, PA_NONE | PA_TRANSIENT, "");

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
	fixtureDef.friction = mFriction;
	fixtureDef.restitution = mRestitution;
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
	else if (GetOwner().HasProperty("PhysicalBody"))
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
	int32 count = val->GetSize();

	if (count < 3) return;

	//----------- Test for preventing Box2D assert ----------//
	float32 area = 0;
	Vector2* vs = val->GetRawArrayPtr();

	for (int32 i = 0; i < count; ++i)
	{
		// Triangle vertices.
		Vector2 p1(0,0);
		Vector2 p2 = vs[i];
		Vector2 p3 = i + 1 < count ? vs[i+1] : vs[0];

		Vector2 e1 = p2 - p1;
		Vector2 e2 = p3 - p1;

		float32 D = MathUtils::Cross(e1, e2);

		area += 0.5f * D;;
	}

	if (area <= FLT_EPSILON)
		return;
	//-------------------------------------------------------//

	mPolygon.CopyFrom(*val);
	if (GetOwner().IsInited()) RecreateShape();
}

float32 EntityComponents::PolygonCollider::GetDensity( void ) const
{
	if (mShape) mDensity = mShape->GetDensity();
	return mDensity;
}

void EntityComponents::PolygonCollider::SetDensity( float32 val )
{
	mDensity = val;
	if (mShape)
	{
		mShape->SetDensity(mDensity);
		PhysicalBody* body = mShape->GetBody();
		if (body) body->ResetMassData();
	}
}

float32 EntityComponents::PolygonCollider::GetFriction( void ) const
{
	if (mShape) mFriction = mShape->GetFriction();
	return mFriction;
}

void EntityComponents::PolygonCollider::SetFriction( float32 val )
{
	mFriction = val;
	if (mShape) mShape->SetFriction(mFriction);
}

float32 EntityComponents::PolygonCollider::GetRestitution( void ) const
{
	if (mShape) mRestitution = mShape->GetRestitution();
	return mRestitution;
}

void EntityComponents::PolygonCollider::SetRestitution( float32 val )
{
	mRestitution = val;
	if (mShape) mShape->SetRestitution(mRestitution);
}
