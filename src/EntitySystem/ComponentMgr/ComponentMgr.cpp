#include "ComponentMgr.h"
#include "../Components/CmpPlatform.h"

using namespace EntitySystem;

ComponentMgr::ComponentMgr()
{
	mComponentCreationMethod[NUM_COMPONENT_TYPES-1] = 0;

	// register components
	mComponentCreationMethod[CT_PLATFORM] = CmpPlatform::CreateMe;

	assert(mComponentCreationMethod[NUM_COMPONENT_TYPES-1]);
}

ComponentMgr::~ComponentMgr() {}

EntityComponentsIterator ComponentMgr::GetEntityComponents(EntityID id)
{
	EntityComponentsMap::iterator eci = mEntityComponentsMap.find(id);
	assert(eci != mEntityComponentsMap.end());
	return EntityComponentsIterator(eci->second);
}

