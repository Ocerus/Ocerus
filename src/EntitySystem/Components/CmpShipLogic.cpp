#include "Common.h"
#include "CmpShipLogic.h"

using namespace EntitySystem;


void EntitySystem::CmpShipLogic::Init( ComponentDescription& desc )
{

}

void EntitySystem::CmpShipLogic::Deinit( void )
{

}

EntityMessage::eResult EntitySystem::CmpShipLogic::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::TYPE_DRAW:
		for (EntityList::iterator i=mPlatforms.begin(); i!=mPlatforms.end(); ++i)
			i->PostMessage(EntityMessage::TYPE_DRAW_INNER);
		for (EntityList::iterator i=mLinks.begin(); i!=mLinks.end(); ++i)
			i->PostMessage(EntityMessage::TYPE_DRAW_INNER);
		return EntityMessage::RESULT_OK;
	}
	return EntityMessage::RESULT_IGNORED;
}

void EntitySystem::CmpShipLogic::RegisterReflection()
{

}