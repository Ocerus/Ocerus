#include "Common.h"
#include "Transform.h"
#include <Box2D.h>
#include "EntitySystem/EntityMgr/LayerMgr.h"

const float32 MIN_SCALAR_SCALE = 0.01f;

void EntityComponents::Transform::Create( void )
{
	mPosition.SetZero();
	mScale.x = 1;
	mScale.y = 1;
	mAngle = 0.0f;
	mDepth = 0;
	mShape = 0;
	mBody = 0;
}

void EntityComponents::Transform::Destroy( void )
{
	DestroyShape();
}

EntityMessage::eResult EntityComponents::Transform::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::SYNC_PRE_PHYSICS:
		if (GlobalProperties::Get<bool>("DevelopMode") && !gEntityMgr.IsEntityPrototype(GetOwner()))
		{
			bool hasCollider = gEntityMgr.HasEntityComponentOfType(GetOwner(), EntitySystem::CT_PolygonCollider);
			if (hasCollider && mShape) DestroyShape();
			else if (!hasCollider && !mShape) CreateShape();

			if (mBody)
			{
				if (mPosition != mBody->GetPosition() || mAngle != mBody->GetAngle())
				{
					mBody->SetTransform(mPosition, mAngle);
				}
			}
		}
		return EntityMessage::RESULT_OK;
	default:
		break;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::Transform::RegisterReflection()
{
	RegisterProperty<Vector2>("Position", &Transform::GetPosition, &Transform::SetPosition, PA_FULL_ACCESS, "Center position in the world.");
	RegisterProperty<Vector2>("Scale", &Transform::GetScale, &Transform::SetScale, PA_FULL_ACCESS, "How big the object is relatively to its default size. The components correspond to the X and Y axis respectively.");
	RegisterProperty<float32>("Angle", &Transform::GetAngle, &Transform::SetAngle, PA_FULL_ACCESS, "Rotation along the Z axis in radians.");
	RegisterProperty<int32>("Layer", &Transform::GetLayer, &Transform::SetLayer, PA_FULL_ACCESS, "What layer the object reside in.");
	RegisterProperty<PhysicalShape*>("PhysicalShapeDummy", &Transform::GetPhysicalShape, 0, PA_NONE | PA_TRANSIENT, "Pointer to the dummy physical shape when there's no collider in the entity.");
}

void EntityComponents::Transform::SetLayer(int32 value)
{
	if (gLayerMgr.ExistsLayer(value) || gEntityMgr.IsEntityPrototype(GetOwner()))
	{
		mDepth = value;
	}
}

void EntityComponents::Transform::SetScale( Vector2 value )
{
	if (value.x >= MIN_SCALAR_SCALE)
		mScale.x = value.x;
	if (value.y >= MIN_SCALAR_SCALE)
		mScale.y = value.y;
}

void EntityComponents::Transform::DestroyShape()
{
	if (!mShape) return;

	GlobalProperties::Get<Physics>("Physics").DestroyBody(mBody);
	mBody = 0;
	mShape = 0;
}

void EntityComponents::Transform::CreateShape()
{
	OC_ASSERT(!mShape);

	// create a dummy body
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position = GetOwner().GetProperty("Position").GetValue<Vector2>();
	bodyDef.angle = GetOwner().GetProperty("Angle").GetValue<float32>();
	bodyDef.fixedRotation = true;
	bodyDef.userData = GetOwnerPtr();
	mBody = GlobalProperties::Get<Physics>("Physics").CreateBody(&bodyDef);

	// define the shape
	b2PolygonShape shapeDef;
	b2Vec2 vertices[4];
	vertices[0].Set(-0.5f, -0.5f);
	vertices[1].Set(0.5f, -0.5f);
	vertices[2].Set(0.5f, 0.5f);
	vertices[3].Set(-0.5f, 0.5f);
	shapeDef.Set(vertices, 4);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shapeDef;
	fixtureDef.userData = GetOwnerPtr();
	fixtureDef.isSensor = true;
	mShape = mBody->CreateFixture(&fixtureDef);
}