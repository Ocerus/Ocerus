#include "EntityMgr.h"
#include "../ComponentMgr/ComponentIterators.h"

using namespace EntitySystem;

EntityMgr::EntityMgr()
{
	mComponentMgr = new ComponentMgr();
}



void EntityMgr::PostMessage(EntityHandle h, const EntityMessage& msg)
{
	for (EntityComponentsIterator iter = mComponentMgr->GetEntityComponents(h); iter.HasMore(); ++iter)
		iter.GetComponent()->HandleMessage(msg);
}
