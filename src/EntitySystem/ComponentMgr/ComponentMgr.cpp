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

ComponentMgr::~ComponentMgr() 
{
	assert(mEntityComponentsMap.size()==0 && "ComponentsMap not empty. (EntityMgr shold erase it before deleting ComponentMgr)");
}

EntityComponentsIterator ComponentMgr::GetEntityComponents(EntityID id)
{
	EntityComponentsMap::iterator eci = mEntityComponentsMap.find(id);
	assert(eci != mEntityComponentsMap.end());
	return EntityComponentsIterator(eci->second);
}

bool ComponentMgr::CreateComponent(EntityHandle h, const ComponentDescription& desc)
{
	Component* cmp = mComponentCreationMethod[desc.GetType()]();
	cmp->Init(desc);
	mEntityComponentsMap[h.mEntityID].push_back(cmp);
	return true;
}

void ComponentMgr::DestroyEntityComponents(EntityID id)
{
	EntityComponentsMap::const_iterator iter = mEntityComponentsMap.find(id);
	assert(iter!=mEntityComponentsMap.end());
	const ComponentsList& cmpList = iter->second;
	for (ComponentsList::const_iterator i=cmpList.begin(); i!=cmpList.end(); ++i)
	{
		(*i)->Deinit();
		DYN_DELETE (*i);
	}
	mEntityComponentsMap.erase(iter);
}