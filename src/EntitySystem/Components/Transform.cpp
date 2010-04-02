#include "Common.h"
#include "Transform.h"
#include <Box2D.h>
#include "EntitySystem/EntityMgr/LayerMgr.h"

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
	RegisterProperty<Vector2>("Position", &Transform::GetPosition, &Transform::SetPosition, PA_FULL_ACCESS, "");
	RegisterProperty<Vector2>("Scale", &Transform::GetScale, &Transform::SetScale, PA_FULL_ACCESS, "");
	RegisterProperty<float32>("Angle", &Transform::GetAngle, &Transform::SetAngle, PA_FULL_ACCESS, "");
	RegisterProperty<int32>("Layer", &Transform::GetLayer, &Transform::SetLayer, PA_FULL_ACCESS, "");
}

void EntityComponents::Transform::SetLayer(int32 value)
{
	if (gLayerMgr.ExistsLayer(value))
	{
		mDepth = value;
	}
}
