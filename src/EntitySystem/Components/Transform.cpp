#include "Common.h"
#include "Transform.h"
#include <Box2D.h>

void EntityComponents::Transform::Create( void )
{
	mBoundToPhysics = false;
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
	switch (msg.type)
	{
	case EntityMessage::POST_INIT:
		if (GetOwner().HasProperty("PhysicalBody")) mBoundToPhysics = true;
		return EntityMessage::RESULT_OK;
	case EntityMessage::UPDATE_POST_PHYSICS:
		if (mBoundToPhysics)
		{
			PhysicalBody* body = GetOwner().GetProperty("PhysicalBody").GetValue<PhysicalBody*>();
			OC_ASSERT(body);
			mPosition = body->GetPosition();
			mAngle = body->GetAngle();
		}
		return EntityMessage::RESULT_OK;
	default:
		break;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::Transform::RegisterReflection()
{
	RegisterProperty<Vector2>("Position", &Transform::GetPosition, &Transform::SetPosition, PA_FULL_ACCESS, "");
	RegisterProperty<Vector2>("Scale", &Transform::GetScale, &Transform::SetScale, PA_FULL_ACCESS, "");
	RegisterProperty<float32>("Angle", &Transform::GetAngle, &Transform::SetAngle, PA_FULL_ACCESS, "");
	RegisterProperty<int32>("Depth", &Transform::GetDepth, &Transform::SetDepth, PA_FULL_ACCESS, "");
}
