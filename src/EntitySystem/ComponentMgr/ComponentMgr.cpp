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

ComponentMgr::ComponentsList& ComponentMgr::GetEntityComponents(EntityHandle h)
{
	EntityComponentsMap::const_iterator eci = mEntityComponentsMap.find(h.mEntityID);
	assert(eci != mEntityComponentsMap.end());
	return eci->second;
}