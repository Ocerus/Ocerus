#include "Common.h"
#include "CmpShipVisual.h"

using namespace EntitySystem;


void EntitySystem::CmpShipVisual::Init( void )
{

}

void EntitySystem::CmpShipVisual::Clean( void )
{

}

EntityMessage::eResult EntitySystem::CmpShipVisual::HandleMessage( const EntityMessage& msg )
{
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpShipVisual::RegisterReflection()
{

}