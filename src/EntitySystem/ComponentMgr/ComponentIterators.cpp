#include "ComponentIterators.h"
#include "ComponentMgr.h"

using namespace EntitySystem

EntityComponentsIterator::EntityComponentsIterator(EntityHandle h): mEntityID(h.mEntityID), mIndex(0)
{
	SetToFirstValidComponent();
}

void EntityComponentsIterator::SetToFirstValidComponent()
{
	assert (NUM_COMPONENT_TYPE_IDS > 0);
	ComponentMgr& cm = ComponentMgr::GetSingleton();
	if (cm.mEntityComponentMap[mIndex].find(mEntityID) == cm.mEntityComponentMap[mIndex].end())
	{ // Couldn't find a component belonging to entity in the first list, now a simple increment will do
		++(*this);
	}
}
