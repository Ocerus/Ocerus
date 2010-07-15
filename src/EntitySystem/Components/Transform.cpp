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
}

void EntityComponents::Transform::Destroy( void )
{

}

EntityMessage::eResult EntityComponents::Transform::HandleMessage( const EntityMessage& msg )
{
	OC_UNUSED(msg);
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::Transform::RegisterReflection()
{
	RegisterProperty<Vector2>("Position", &Transform::GetPosition, &Transform::SetPosition, PA_FULL_ACCESS, "Center position in the world.");
	RegisterProperty<Vector2>("Scale", &Transform::GetScale, &Transform::SetScale, PA_FULL_ACCESS, "How big the object is relatively to its default size. The components correspond to the X and Y axis respectively.");
	RegisterProperty<float32>("Angle", &Transform::GetAngle, &Transform::SetAngle, PA_FULL_ACCESS, "Rotation along the Z axis in radians.");
	RegisterProperty<int32>("Layer", &Transform::GetLayer, &Transform::SetLayer, PA_FULL_ACCESS, "What layer the object reside in.");
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
	if (value.x < MIN_SCALAR_SCALE || value.y < MIN_SCALAR_SCALE)
	{
		return;
	}
	mScale = value;
}