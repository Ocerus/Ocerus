#include "Common.h"
#include "CmpShipPhysics.h"

using namespace EntitySystem;


void EntitySystem::CmpShipPhysics::Init( ComponentDescription& desc )
{

}

void EntitySystem::CmpShipPhysics::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpShipPhysics::HandleMessage( const EntityMessage& msg )
{
	return EntityMessage::RESULT_OK;
}

void EntitySystem::CmpShipPhysics::RegisterReflection()
{

}