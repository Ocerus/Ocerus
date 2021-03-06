#include "Common.h"
#include "Camera.h"

void EntityComponents::Camera::Create( void )
{
	mPosition = Vector2_Zero;
	mRotation = 0.0f;
	mZoom = 1.0f;
}

void EntityComponents::Camera::Destroy( void )
{
}

EntityMessage::eResult EntityComponents::Camera::HandleMessage( const EntityMessage& msg )
{
	OC_UNUSED(msg);
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::Camera::RegisterReflection()
{
	RegisterProperty<float32>	("Zoom",		&Camera::GetZoom,		&Camera::SetZoom,		PA_FULL_ACCESS, "How big things appear in the camera. The bigger the number the bigger the objects.");
	RegisterProperty<float32>	("Rotation",	&Camera::GetRotation,	&Camera::SetRotation,	PA_FULL_ACCESS, "Rotation along the Z axis in radians.");
	RegisterProperty<Vector2>	("Position",	&Camera::GetPosition,	&Camera::SetPosition,	PA_FULL_ACCESS, "Position in the world.");
}
