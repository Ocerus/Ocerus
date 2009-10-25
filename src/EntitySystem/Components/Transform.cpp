#include "Common.h"
#include "Transform.h"

void EntityComponents::Transform::Create( void )
{
	mPosition.SetZero();
	mScale.SetZero();
	mAngle = 0.0f;
	mDepth = 0;
}

void EntityComponents::Transform::Destroy( void )
{

}

EntityMessage::eResult EntityComponents::Transform::HandleMessage( const EntityMessage& msg )
{
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::Transform::RegisterReflection()
{
	RegisterProperty<Vector2>("Position", &Transform::GetPosition, &Transform::SetPosition, PA_FULL_ACCESS);
	RegisterProperty<Vector2>("Scale", &Transform::GetScale, &Transform::SetScale, PA_FULL_ACCESS);
	RegisterProperty<float32>("Angle", &Transform::GetAngle, &Transform::SetAngle, PA_FULL_ACCESS);
	RegisterProperty<int32>("Depth", &Transform::GetDepth, &Transform::SetDepth, PA_FULL_ACCESS);
}
