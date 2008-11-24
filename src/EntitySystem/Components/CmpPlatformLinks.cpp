#include "Common.h"
#include "CmpPlatformLinks.h"

using namespace EntitySystem;


void EntitySystem::CmpPlatformLinks::Init( ComponentDescription& desc )
{
	SetFirstPlatform(desc.GetNextItem()->GetData<EntityHandle>());
	SetSecondPlatform(desc.GetNextItem()->GetData<EntityHandle>());
	ComputeDetachingChance();
	SetNumLinks(desc.GetNextItem()->GetData<uint32>());
	//TODO zautomatizovat pozicovani anchor - nejspis uedlat do SetNumLinks, at se prepocitaji jejich pozice
	mFirstAnchors = 0;
	SetFirstAnchors(desc.GetNextItem()->GetData<Vector2*>());
	mSecondAnchors = 0;
	SetSecondAnchors(desc.GetNextItem()->GetData<Vector2*>());

	EntityHandle ship;
	mFirstPlatform.PostMessage(EntityMessage::TYPE_GET_PARENT, &ship);
	EntityHandle ship2;
	mSecondPlatform.PostMessage(EntityMessage::TYPE_GET_PARENT, &ship2);
	assert(ship == ship2 && "Not compatible platforms");
	ship.PostMessage(EntityMessage::TYPE_LINK_PLATFORMS, GetOwnerPtr());
}

void EntitySystem::CmpPlatformLinks::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpPlatformLinks::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_DRAW_INNER:
		Draw();
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpPlatformLinks::RegisterReflection()
{
	RegisterProperty<EntityHandle>("FirstPlatform", &GetFirstPlatform, &SetFirstPlatform, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<EntityHandle>("SecondPlatform", &GetSecondPlatform, &SetSecondPlatform, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<float32>("DetachingChance", &GetDetachingChance, &SetDetachingChance, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<uint32>("NumLinks", &GetNumLinks, &SetNumLinks, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2*>("FirstAnchors", &GetFirstAnchors, &SetFirstAnchors, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
	RegisterProperty<Vector2*>("SecondAnchors", &GetSecondAnchors, &SetSecondAnchors, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

void EntitySystem::CmpPlatformLinks::ComputeDetachingChance( void )
{
	mDetachingChance = 0.0f;
}

void EntitySystem::CmpPlatformLinks::Draw( void )
{
	EntityHandle ship;
	mFirstPlatform.PostMessage(EntityMessage::TYPE_GET_PARENT, &ship);
	Vector2 shipPos;
	ship.PostMessage(EntityMessage::TYPE_GET_POSITION, &shipPos);
	float32 angle;
	ship.PostMessage(EntityMessage::TYPE_GET_ANGLE, &angle);
	XForm xf(shipPos, Matrix22(angle));
	for (uint32 i=0; i<mAnchorsLength; ++i)
		gGfxRenderer.DrawLineWithConversion(MathUtils::Multiply(xf, mFirstAnchors[i]), MathUtils::Multiply(xf, mSecondAnchors[i]), GfxSystem::Pen(GfxSystem::Color(200,0,0,168)));

}

void EntitySystem::CmpPlatformLinks::SetFirstAnchors( Vector2* anchors )
{
	if (mFirstAnchors)
		DYN_DELETE_ARRAY mFirstAnchors;
	mFirstAnchors = DYN_NEW Vector2[mAnchorsLength];
	for (uint32 i=0; i<mAnchorsLength; ++i)
		mFirstAnchors[i] = anchors[i];
}

void EntitySystem::CmpPlatformLinks::SetSecondAnchors( Vector2* anchors )
{
	if (mSecondAnchors)
		DYN_DELETE_ARRAY mSecondAnchors;
	mSecondAnchors = DYN_NEW Vector2[mAnchorsLength];
	for (uint32 i=0; i<mAnchorsLength; ++i)
		mSecondAnchors[i] = anchors[i];
}