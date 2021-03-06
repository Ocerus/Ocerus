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
			Vector2 position = GetOwner().GetProperty("Position").GetValue<Vector2>();
			float32 angle = GetOwner().GetProperty("Angle").GetValue<float32>();
			if (position != mSensorBody->GetPosition() || angle != mSensorBody->GetAngle())
			{
				mSensorBody->SetTransform(position, angle);
			}
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::COMPONENT_CREATED:
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
	RegisterProperty<float32>("Density", &PolygonCollider::GetDensity, &PolygonCollider::SetDensity, PA_FULL_ACCESS, "How heavy the material is relatively to the standard one.");
	RegisterProperty<float32>("Friction", &PolygonCollider::GetFriction, &PolygonCollider::SetFriction, PA_FULL_ACCESS, "How much will the object slow down while touching other objects. Range from 0 to 1.");
	RegisterProperty<float32>("Restitution", &PolygonCollider::GetRestitution, &PolygonCollider::SetRestitution, PA_FULL_ACCESS, "How much till the object bounce after colliding with other objects. Range from 0 to 1.");
	RegisterProperty<Array<Vector2>*>("Polygon", &PolygonCollider::GetPolygon, &PolygonCollider::SetPolygon, PA_FULL_ACCESS, "Polygon shape of the collider.");
	RegisterProperty<PhysicalShape*>("PhysicalShape", &PolygonCollider::GetShape, 0, PA_NONE | PA_TRANSIENT, "Pointer to the physics object.");

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
	GetBox2dVertices(&mPolygon, vertices);
	shapeDef.Set(vertices, mPolygon.GetSize());
	delete[] vertices;
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
		if (body && mShape)
		{
			body->DestroyFixture(mShape);
		}
	}
	mShape = 0;
}

void EntityComponents::PolygonCollider::SetPolygon( Array<Vector2>* val )
{
	int32 vertexCount = val->GetSize();

	if (vertexCount < 3) return;

	//----------- Test for preventing Box2D assert ----------//
	float32 area = 0;
	Vector2* vs = val->GetRawArrayPtr();

	for (int32 i = 0; i < vertexCount; ++i)
	{
		// Triangle vertices.
		Vector2 p1(0,0);
		Vector2 p2 = vs[i];
		Vector2 p3 = i + 1 < vertexCount ? vs[i+1] : vs[0];

		Vector2 e1 = p2 - p1;
		Vector2 e2 = p3 - p1;

		float32 D = MathUtils::Cross(e1, e2);

		area += 0.5f * D;;
	}

	if (area <= FLT_EPSILON) return;


	b2Vec2* vertices = new b2Vec2[vertexCount];
	GetBox2dVertices(val, vertices);
	bool verticesOk = true;
	for (int32 i = 0; i < vertexCount; ++i)
	{
		int32 i1 = i;
		int32 i2 = i + 1 < vertexCount ? i + 1 : 0;
		if ((vertices[i2] - vertices[i1]).LengthSquared() <= FLT_EPSILON * FLT_EPSILON)
		{
			verticesOk = false;
		}
	}
	delete[] vertices;
	if (!verticesOk) return;
	//-------------------------------------------------------//

	mPolygon.CopyFrom(*val);
	if (GetOwner().IsInited() && !gEntityMgr.IsEntityPrototype(GetOwner()))
	{
		RecreateShape();
	}
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

void EntityComponents::PolygonCollider::GetBox2dVertices( Array<Vector2>* polygon, b2Vec2* vertices )
{
	Vector2 scale = GetOwner().GetProperty("Scale").GetValue<Vector2>();
	for (int i=0; i<polygon->GetSize(); ++i)
	{
		vertices[i].x = scale.x * polygon->GetRawArrayPtr()[i].x;
		vertices[i].y = scale.y * polygon->GetRawArrayPtr()[i].y;
	}
}