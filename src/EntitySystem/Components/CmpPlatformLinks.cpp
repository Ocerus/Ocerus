#include "Common.h"
#include "CmpPlatformLinks.h"

using namespace EntitySystem;


void EntitySystem::CmpPlatformLinks::Init( ComponentDescription& desc )
{
	SetFirstPlatform(desc.GetNextItem()->GetEntityHandle());
	SetSecondPlatform(desc.GetNextItem()->GetEntityHandle());
	ComputeDetachingChance();
	//TODO vytvorit linky
}

void EntitySystem::CmpPlatformLinks::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpPlatformLinks::HandleMessage( const EntityMessage& msg )
{
	switch(msg.type)
	{
	case EntityMessage::TYPE_DRAW:
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
	RegisterProperty<uint32>("NumLinks", &GetNumLinks, 0, PROPACC_EDIT_READ | PROPACC_SCRIPT_READ);
}

void EntitySystem::CmpPlatformLinks::ComputeDetachingChance( void )
{
	mDetachingChance = 0.0f;
}

void EntitySystem::CmpPlatformLinks::Draw( void ) const
{
	for (AnchorsList::const_iterator i=mAnchors.begin(); i!=mAnchors.end(); ++i)
		gGfxRenderer.DrawLineWithConversion(i->first, i->second, GfxSystem::Pen(GfxSystem::Color(200,0,0,168)));

}