#include "Common.h"
#include "Camera.h"

void EntityComponents::Camera::Create( void )
{
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
	RegisterProperty<float32>	("Zoom",		&Camera::GetZoom,		&Camera::SetZoom,		PA_FULL_ACCESS, "");
	RegisterProperty<float32>	("Rotation",	&Camera::GetRotation,	&Camera::SetRotation,	PA_FULL_ACCESS, "");
	RegisterProperty<Vector2>	("Position",	&Camera::GetPosition,	&Camera::SetPosition,	PA_FULL_ACCESS, "");
}
