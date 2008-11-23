#include "Common.h"
#include "CmpPlatformPhysics.h"
#include "Box2D.h"
#include "../../Utility/DataContainer.h"
#include "../../Core/Game.h"

using namespace EntitySystem;

void EntitySystem::CmpPlatformPhysics::Init( ComponentDescription& desc )
{
	mBody = 0;
	mShape = 0;
	EntityHandle ship;
	gEntityMgr.PostMessage(GetOwner(), EntityMessage(EntityMessage::TYPE_GET_PARENT, &ship));

	// create body
	if (ship.IsValid())
	{
		gEntityMgr.PostMessage(ship, EntityMessage(EntityMessage::TYPE_GET_PHYSICS_BODY, &mBody));
	}
	else
	{
		b2BodyDef bodyDef;
		bodyDef.position = desc.GetNextItem()->GetData<Vector2>();
		bodyDef.angle = desc.GetNextItem()->GetData<float32>();
		bodyDef.userData = GetOwnerPtr();
		mBody = gApp.GetCurrentGame()->GetPhysics()->CreateBody(&bodyDef);
	}

	// create shape
	b2PolygonDef shapeDef;
	EntityHandle blueprints;
	gEntityMgr.PostMessage(GetOwner(), EntityMessage(EntityMessage::TYPE_GET_BLUEPRINTS, &blueprints));
	EntityHandle material;
	gEntityMgr.PostMessage(blueprints, EntityMessage(EntityMessage::TYPE_GET_MATERIAL, &material));
	// set density
	float32 density;
	gEntityMgr.PostMessage(blueprints, EntityMessage(EntityMessage::TYPE_GET_DENSITY, &density));
	shapeDef.density = density;
	// retrieve original shape
	DataContainer cont;
	gEntityMgr.PostMessage(blueprints, EntityMessage(EntityMessage::TYPE_GET_POLYSHAPE, &cont));
	Vector2* poly = (Vector2*)cont.GetData();
	uint32 polyLen = cont.GetSize();
	// retrieve shape transformation info
	Vector2 relPos = b2Vec2_zero;
	if (ship.IsValid())
		relPos = desc.GetNextItem()->GetData<Vector2>();
	bool flip = desc.GetNextItem()->GetData<bool>();
	float32 angle = desc.GetNextItem()->GetData<float32>();
	b2XForm xform(relPos, b2Mat22(angle));
	// create the shape
	for (uint32 i=0; i<polyLen; ++i)
	{
		Vector2 vec = poly[i];
		if (flip)
		{
			vec.x = -vec.x;
			vec.y = -vec.y;
		}
		shapeDef.vertices[shapeDef.vertexCount++] = b2Mul(xform, vec);
	}
	// set the entity handle so that we can identify the shape later
	shapeDef.userData = GetOwnerPtr();

	mShape = mBody->CreateShape(&shapeDef);
}

void EntitySystem::CmpPlatformPhysics::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpPlatformPhysics::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_GET_POSITION:
		assert(msg.data);
		((Vector2*)msg.data)->Set(GetPosition());
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_ANGLE:
		assert(msg.data);
		*(float32*)msg.data = GetAngle();
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_GET_POLYSHAPE:
		assert(msg.data);
		assert(mShape);
		{
			b2PolygonShape* polyshape = (b2PolygonShape*)mShape;
			((DataContainer*)msg.data)->SetData((uint8*)polyshape->GetVertices(), polyshape->GetVertexCount());
		}
		return EntityMessage::RESULT_OK;
	case EntityMessage::TYPE_PLATFORM_DETACH:
		assert(mBody);
		//TODO
		return EntityMessage::RESULT_ERROR;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpPlatformPhysics::RegisterReflection()
{
	RegisterProperty<Vector2&>("Position", &GetPosition, &SetPosition, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("Angle", &GetAngle, &SetAngle, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

Vector2& EntitySystem::CmpPlatformPhysics::GetPosition( void ) const
{
	assert(mBody);
	return const_cast<Vector2&>(mBody->GetPosition());
}

void EntitySystem::CmpPlatformPhysics::SetPosition( Vector2& pos )
{
	assert(mBody);
	mBody->SetXForm(pos, mBody->GetAngle());
}

float32 EntitySystem::CmpPlatformPhysics::GetAngle( void ) const
{
	assert(mBody);
	return mBody->GetAngle();
}

void EntitySystem::CmpPlatformPhysics::SetAngle( const float32 angle )
{
	assert(mBody);
	mBody->SetXForm(mBody->GetPosition(), angle);
}