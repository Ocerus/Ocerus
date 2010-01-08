#include "Common.h"
#include "PolygonCollider.h"
#include <Box2D.h>

void EntityComponents::PolygonCollider::Create( void )
{
	mShape = 0;
	mDensity = 0;
}

void EntityComponents::PolygonCollider::Destroy( void )
{
	// the shape is destroyed automatically after the body it is attached to is destroyed		
}

EntityMessage::eResult EntityComponents::PolygonCollider::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::POST_INIT: // we have to wait until the physical bodies are inited, that's why we're using POST_INIT
		Init();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntityComponents::PolygonCollider::RegisterReflection( void )
{
	RegisterProperty<float32>("Density", &PolygonCollider::GetDensity, &PolygonCollider::SetDensity, PA_FULL_ACCESS, "");
	RegisterProperty<Array<Vector2>*>("Polygon", &PolygonCollider::GetPolygon, &PolygonCollider::SetPolygon, PA_FULL_ACCESS, "");

	// we need the transform to be able to have the position and angle ready while creating the body
	AddComponentDependency(CT_Transform);
}

void EntityComponents::PolygonCollider::Init( void )
{
	// define the shape
	b2PolygonDef shapeDef;
	shapeDef.density = mDensity;
	for (int32 i=0; i<mPolygon.GetSize(); ++i)
	{
		shapeDef.vertices[shapeDef.vertexCount++] = mPolygon[i];
	}
	shapeDef.userData = GetOwnerPtr();
	
	// find a parent body
	PhysicalBody* body = 0;
	if (GetOwner().HasProperty("PhysicalBody"))
	{
		PropertyHolder bodyProp = GetOwner().GetProperty("PhysicalBody");
		body = bodyProp.GetValue<PhysicalBody*>();
	}
	else
	{
		/// @todo to make colliders work without bodies we must upgrade Box2d to 2.0.2+ and use b2Fixture.
		OC_FAIL("Not implemented");
	}
	
	// create the shape
	mShape = body->CreateShape(&shapeDef);

	// update the mass of the body
	body->SetMassFromShapes();
}
