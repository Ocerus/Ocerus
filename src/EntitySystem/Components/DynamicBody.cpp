#include "Common.h"
#include "DynamicBody.h"
#include <Box2D.h>

const float32 LINEAR_DAMPING = 0.1f;
const float32 ANGULAR_DAMPING = 0.5f;

void EntityComponents::DynamicBody::Create( void )
{
	mBody = 0;
}

void EntityComponents::DynamicBody::Destroy( void )
{
	if (mBody)
	{
		GlobalProperties::Get<Physics>("Physics").DestroyBody(mBody);
		mBody = 0;
	}
}

EntityMessage::eResult EntityComponents::DynamicBody::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::INIT:
		Init();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::DynamicBody::RegisterReflection( void )
{
	RegisterProperty<PhysicalBody*>("PhysicalBody", &DynamicBody::GetBody, &DynamicBody::SetBody, PA_ENGINE, "");
	RegisterFunction("ApplyForce", &DynamicBody::ApplyForce, PA_SCRIPT_WRITE | PA_ENGINE, "");

	// we need the transform to be able to have the position and angle ready while creating the body
	AddComponentDependency(CT_Transform);
}

void EntityComponents::DynamicBody::Init( void )
{
	b2BodyDef bodyDef;
	bodyDef.position = GetOwner().GetProperty("Position").GetValue<Vector2>();
	bodyDef.angle = GetOwner().GetProperty("Angle").GetValue<float32>();
	bodyDef.angularDamping = ANGULAR_DAMPING;
	bodyDef.linearDamping = LINEAR_DAMPING;
	bodyDef.userData = GetOwnerPtr();

	mBody = GlobalProperties::Get<Physics>("Physics").CreateBody(&bodyDef);
}

void EntityComponents::DynamicBody::ApplyForce( PropertyFunctionParameters params )
{
	OC_ASSERT(mBody);
	mBody->ApplyForce(*params.GetParameter(0).GetData<Vector2>(), *params.GetParameter(1).GetData<Vector2>());
}