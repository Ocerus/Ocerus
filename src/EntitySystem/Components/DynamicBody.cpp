#include "Common.h"
#include "DynamicBody.h"
#include <Box2D.h>

void EntityComponents::DynamicBody::Create( void )
{
	mBody = 0;
	mAngularDamping = 0.5f;
	mLinearDamping = 0.1f;
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
		CreateBody();
		return EntityMessage::RESULT_OK;
	case EntityMessage::SYNC_PRE_PHYSICS:	
		mBody->SetTransform(GetOwner().GetProperty("Position").GetValue<Vector2>(), 
			GetOwner().GetProperty("Angle").GetValue<float32>());
		return EntityMessage::RESULT_OK;
	case EntityMessage::SYNC_POST_PHYSICS:
		GetOwner().GetProperty("Position").SetValue<Vector2>(mBody->GetPosition());
		GetOwner().GetProperty("Angle").SetValue<float32>(mBody->GetAngle());
		return EntityMessage::RESULT_OK;
	default:
		break;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::DynamicBody::RegisterReflection( void )
{
	RegisterProperty<PhysicalBody*>("PhysicalBody", &DynamicBody::GetBody, &DynamicBody::SetBody, PA_NONE | PA_TRANSIENT, "");
	RegisterFunction("ApplyForce", &DynamicBody::ApplyForce, PA_SCRIPT_WRITE, "");
	RegisterFunction("ApplyTorque", &DynamicBody::ApplyTorque, PA_SCRIPT_WRITE, "");
	RegisterProperty<float32>("LinearDamping", &DynamicBody::GetLinearDamping, &DynamicBody::SetLinearDamping, PA_FULL_ACCESS, "");
	RegisterProperty<float32>("AngularDamping", &DynamicBody::GetAngularDamping, &DynamicBody::SetAngularDamping, PA_FULL_ACCESS, "");

	// we need the transform to be able to have the position and angle ready while creating the body
	AddComponentDependency(CT_Transform);
}

void EntityComponents::DynamicBody::CreateBody( void )
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = GetOwner().GetProperty("Position").GetValue<Vector2>();
	bodyDef.angle = GetOwner().GetProperty("Angle").GetValue<float32>();
	bodyDef.angularDamping = mAngularDamping;
	bodyDef.linearDamping = mLinearDamping;
	bodyDef.userData = GetOwnerPtr();

	mBody = GlobalProperties::Get<Physics>("Physics").CreateBody(&bodyDef);
}

void EntityComponents::DynamicBody::ApplyForce( PropertyFunctionParameters params )
{
	OC_ASSERT(mBody);
	OC_ASSERT(params.GetParametersCount() >= 1);
	Vector2 force = *params.GetParameter(0).GetData<Vector2>();
	Vector2 point = Vector2_Zero;
	if (params.GetParametersCount() >= 2) point = *params.GetParameter(1).GetData<Vector2>();
	mBody->ApplyForce(force, point);
}

void EntityComponents::DynamicBody::ApplyTorque( PropertyFunctionParameters params )
{
	OC_ASSERT(mBody);
	OC_ASSERT(params.GetParametersCount() >= 1);
	mBody->ApplyTorque(*params.GetParameter(0).GetData<float32>());
}

float32 EntityComponents::DynamicBody::GetAngularDamping() const
{
	if (mBody) mAngularDamping = mBody->GetAngularDamping();
	return mAngularDamping;
}

void EntityComponents::DynamicBody::SetAngularDamping( float32 val )
{
	mAngularDamping = val;
	if (mBody) mBody->SetAngularDamping(mAngularDamping);
}

float32 EntityComponents::DynamicBody::GetLinearDamping() const
{
	if (mBody) mLinearDamping = mBody->GetLinearDamping();
	return mLinearDamping;
}

void EntityComponents::DynamicBody::SetLinearDamping( float32 val )
{
	mLinearDamping = val;
	if (mBody) mBody->SetLinearDamping(mLinearDamping);
}