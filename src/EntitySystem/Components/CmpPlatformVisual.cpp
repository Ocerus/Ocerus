#include "Common.h"
#include "CmpPlatformVisual.h"

using namespace EntitySystem;


void EntitySystem::CmpPlatformVisual::Init( ComponentDescription& desc )
{

}

void EntitySystem::CmpPlatformVisual::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpPlatformVisual::HandleMessage( const EntityMessage& msg )
{
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpPlatformVisual::RegisterReflection()
{

}