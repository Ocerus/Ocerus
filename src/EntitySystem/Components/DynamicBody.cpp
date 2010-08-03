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
		if (!gEntityMgr.IsEntityPrototype(GetOwner())) CreateBody();
		return EntityMessage::RESULT_OK;
	case EntityMessage::SYNC_PRE_PHYSICS:
		{
			Vector2 position = GetOwner().GetProperty("Position").GetValue<Vector2>();
			float32 angle = GetOwner().GetProperty("Angle").GetValue<float32>();
			if (position != mBody->GetPosition() || angle != mBody->GetAngle())
			{
				mBody->SetTransform(position, angle);
				mBody->SetAwake(true);
			}
			return EntityMessage::RESULT_OK;
		}
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
	RegisterProperty<PhysicalBody*>("PhysicalBody", &DynamicBody::GetBody, &DynamicBody::SetBody, PA_NONE | PA_TRANSIENT, "Pointer to the physics object.");
	RegisterFunction("ApplyForce", &DynamicBody::ApplyForceWorldCoords, PA_SCRIPT_WRITE, "Applies force to the object in the world coords.");
	RegisterFunction("ApplyForceWorldCoords", &DynamicBody::ApplyForceWorldCoords, PA_SCRIPT_WRITE, "Applies force to the object in the world coords.");
	RegisterFunction("ApplyForceLocalCoords", &DynamicBody::ApplyForceLocalCoords, PA_SCRIPT_WRITE, "Applies force to the object in the local coords.");
	RegisterFunction("ApplyTorque", &DynamicBody::ApplyTorque, PA_SCRIPT_WRITE, "Applies torque to the object.");
	RegisterFunction("ApplyLinearImpulse", &DynamicBody::ApplyLinearImpulseWorldCoords, PA_SCRIPT_WRITE, "Applies force impulse to the object in the world coords.");
	RegisterFunction("ApplyLinearImpulseWorldCoords", &DynamicBody::ApplyLinearImpulseWorldCoords, PA_SCRIPT_WRITE, "Applies force impulse to the object in the world coords.");
	RegisterFunction("ApplyAngularImpulse", &DynamicBody::ApplyAngularImpulseWorldCoords, PA_SCRIPT_WRITE, "Applies angular impulse to the object in the world coords.");
	RegisterFunction("ApplyAngularImpulseWorldCoords", &DynamicBody::ApplyAngularImpulseWorldCoords, PA_SCRIPT_WRITE, "Applies angular impulse to the object in the world coords.");
	RegisterFunction("ZeroVelocity", &DynamicBody::ZeroVelocity, PA_SCRIPT_WRITE, "Zeroes out all velocities of the body.");
	RegisterProperty<float32>("LinearDamping", &DynamicBody::GetLinearDamping, &DynamicBody::SetLinearDamping, PA_FULL_ACCESS, "How much is the body going to slow down in the linear motion.");
	RegisterProperty<float32>("AngularDamping", &DynamicBody::GetAngularDamping, &DynamicBody::SetAngularDamping, PA_FULL_ACCESS, "How much is the body going to slow down in the angular motion.");
	RegisterProperty<Vector2>("LinearVelocity", &DynamicBody::GetLinearVelocity, 0, PA_EDIT_READ | PA_SCRIPT_READ, "Current velocity of the linear motion.");
	RegisterProperty<float32>("AngularVelocity", &DynamicBody::GetAngularVelocity, 0, PA_EDIT_READ | PA_SCRIPT_READ, "Current velocity of the angular motion.");
	RegisterProperty<Array<Vector2>*>("Contacts", &DynamicBody::GetContacts, 0, PA_SCRIPT_READ, "Read-only list of contact points attached to the body (world coords).");
	RegisterProperty<uint32>("ContactsCount", &DynamicBody::GetContactsCount, 0, PA_SCRIPT_READ, "Number of contact points attached to the body.");

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

void EntityComponents::DynamicBody::ApplyForceLocalCoords( PropertyFunctionParameters params )
{
	OC_ASSERT(mBody);
	OC_ASSERT(params.GetParametersCount() >= 1);
	Vector2 force = *params.GetParameter(0).GetData<Vector2>();
	Vector2 point = Vector2_Zero;
	if (params.GetParametersCount() >= 2) point = *params.GetParameter(1).GetData<Vector2>();
	force = mBody->GetLocalVector(force);
	point = mBody->GetLocalPoint(point);
	mBody->ApplyForce(force, point);
}

void EntityComponents::DynamicBody::ApplyForceWorldCoords( PropertyFunctionParameters params )
{
	OC_ASSERT(mBody);
	OC_ASSERT(params.GetParametersCount() >= 1);
	Vector2 force = *params.GetParameter(0).GetData<Vector2>();
	Vector2 point = mBody->GetWorldCenter();
	if (params.GetParametersCount() >= 2) point = *params.GetParameter(1).GetData<Vector2>();
	mBody->ApplyForce(force, point);
}

void EntityComponents::DynamicBody::ApplyTorque( PropertyFunctionParameters params )
{
	OC_ASSERT(mBody);
	OC_ASSERT(params.GetParametersCount() >= 1);
	mBody->ApplyTorque(*params.GetParameter(0).GetData<float32>());
}

void EntityComponents::DynamicBody::ApplyLinearImpulseWorldCoords( PropertyFunctionParameters params )
{
	OC_ASSERT(mBody);
	OC_ASSERT(params.GetParametersCount() >= 1);
	Vector2 impulse = *params.GetParameter(0).GetData<Vector2>();
	Vector2 point = mBody->GetWorldCenter();
	if (params.GetParametersCount() >= 2) point = *params.GetParameter(1).GetData<Vector2>();
	mBody->ApplyLinearImpulse(impulse, point);
}

void EntityComponents::DynamicBody::ApplyAngularImpulseWorldCoords( PropertyFunctionParameters params )
{
	OC_ASSERT(mBody);
	OC_ASSERT(params.GetParametersCount() == 1);
	float32 impulse = *params.GetParameter(0).GetData<float32>();
	mBody->ApplyAngularImpulse(impulse);
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

Vector2 EntityComponents::DynamicBody::GetLinearVelocity() const
{
	if (mBody) return mBody->GetLinearVelocity();
	else return Vector2_Zero;
}

float32 EntityComponents::DynamicBody::GetAngularVelocity() const
{
	if (mBody) return mBody->GetAngularVelocity();
	else return 0.0f;
}

Array<Vector2>* EntityComponents::DynamicBody::GetContacts() const
{
	if (mBody)
	{
		b2ContactEdge* contactList = mBody->GetContactList();
		int32 contactsCount = 0;
		while (contactList)
		{
			if (contactList->contact->IsTouching() && contactList->contact->GetManifold()->pointCount > 0) ++contactsCount;
			contactList = contactList->next;
		}
		mContactsCache.Resize(contactsCount);

		int i = 0;
		for (contactList=mBody->GetContactList(); contactList; contactList=contactList->next)
		{
			if (!contactList->contact->IsTouching() || contactList->contact->GetManifold()->pointCount == 0) continue;

			b2WorldManifold worldManifold;
			contactList->contact->GetWorldManifold(&worldManifold);
			Vector2 worldPoint = Vector2_Zero;
			int32 pointCount = contactList->contact->GetManifold()->pointCount;
			for (int j=0; j<pointCount; ++j) worldPoint += worldManifold.points[j];
			worldPoint.x /= pointCount;
			worldPoint.y /= pointCount;
			mContactsCache[i] = worldPoint;

			++i;
		}

		return &mContactsCache;
	}
	else
	{
		return 0;
	}
}

uint32 EntityComponents::DynamicBody::GetContactsCount() const
{
	if (mBody)
	{
		b2ContactEdge* contactList = mBody->GetContactList();
		int32 contactsCount = 0;
		while (contactList)
		{
			if (contactList->contact->IsTouching() && contactList->contact->GetManifold()->pointCount > 0) ++contactsCount;
			contactList = contactList->next;
		}
		return contactsCount;
	}
	else
	{
		return 0;
	}
}

void EntityComponents::DynamicBody::ZeroVelocity( PropertyFunctionParameters params )
{
	OC_UNUSED(params);
	if (mBody)
	{
		mBody->SetLinearVelocity(Vector2_Zero);
		mBody->SetAngularVelocity(0);
	}
}