#include "Common.h"
#include "StaticBody.h"
#include <Box2D.h>


void EntityComponents::StaticBody::Create( void )
{
	mBody = 0;
}

void EntityComponents::StaticBody::Destroy( void )
{
	if (mBody)
	{
		GlobalProperties::Get<Physics>("Physics").DestroyBody(mBody);
		mBody = 0;
	}
}

EntityMessage::eResult EntityComponents::StaticBody::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::INIT:
		if (!gEntityMgr.IsEntityPrototype(GetOwner())) CreateBody();
		return EntityMessage::RESULT_OK;
	case EntityMessage::SYNC_PRE_PHYSICS:	
		mBody->SetTransform(GetOwner().GetProperty("Position").GetValue<Vector2>(), 
			GetOwner().GetProperty("Angle").GetValue<float32>());
		return EntityMessage::RESULT_OK;
	default:
		break;
	}
	return EntityMessage::RESULT_IGNORED;}

void EntityComponents::StaticBody::RegisterReflection( void )
{
	RegisterProperty<PhysicalBody*>("PhysicalBody", &StaticBody::GetBody, &StaticBody::SetBody, PA_NONE | PA_TRANSIENT, "");

	// we need the transform to be able to have the position and angle ready while creating the body
	AddComponentDependency(CT_Transform);
}

void EntityComponents::StaticBody::CreateBody( void )
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position = GetOwner().GetProperty("Position").GetValue<Vector2>();
	bodyDef.angle = GetOwner().GetProperty("Angle").GetValue<float32>();
	bodyDef.userData = GetOwnerPtr();

	mBody = GlobalProperties::Get<Physics>("Physics").CreateBody(&bodyDef);
}
