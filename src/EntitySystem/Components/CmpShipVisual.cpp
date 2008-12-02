#include "Common.h"
#include "CmpShipVisual.h"

using namespace EntitySystem;


void EntitySystem::CmpShipVisual::Init( ComponentDescription& desc )
{

}

void EntitySystem::CmpShipVisual::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpShipVisual::HandleMessage( const EntityMessage& msg )
{
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpShipVisual::RegisterReflection()
{

}