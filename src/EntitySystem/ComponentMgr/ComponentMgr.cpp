#include "Common.h"
#include "ComponentMgr.h"
#include "Component.h"

#include "../Components/_ComponentHeaders.h"

using namespace EntitySystem;

ComponentMgr::ComponentMgr()
{
	mComponentCreationMethod[NUM_COMPONENT_TYPES-1] = 0;

	// register components
	#define COMPONENT_TYPE(id, cls) mComponentCreationMethod[id] = (ComponentCreationMethod)cls::GetClassRTTI()->GetClassFactory();
	#include "../Components/_ComponentTypes.h"
	#undef COMPONENT_TYPE

	OC_ASSERT(mComponentCreationMethod[NUM_COMPONENT_TYPES-1]);
}

ComponentMgr::~ComponentMgr()
{
	OC_ASSERT_MSG(mEntityComponentsMap.size()==0, "ComponentsMap not empty. (EntityMgr should erase it before deleting ComponentMgr)");
}

EntityComponentsIterator ComponentMgr::GetEntityComponents(EntityID id) const
{
	EntityComponentsMap::const_iterator eci = mEntityComponentsMap.find(id);
	OC_ASSERT(eci != mEntityComponentsMap.end());
	return EntityComponentsIterator(eci->second);
}

Component* ComponentMgr::CreateComponent(EntityHandle h, const eComponentType type)
{
	OC_ASSERT(type < NUM_COMPONENT_TYPES && type >= 0);
	Component* cmp = mComponentCreationMethod[type]();
	EntityComponentsMap::const_iterator entIt = mEntityComponentsMap.find(h.GetID());
	if (entIt == mEntityComponentsMap.end())
	{
		mEntityComponentsMap[h.GetID()] = new ComponentsList();
		entIt = mEntityComponentsMap.find(h.GetID());
	}
	entIt->second->push_back(cmp);
	cmp->SetOwner(h);
	cmp->SetType(type);
	cmp->Create();
	return cmp;
}

void ComponentMgr::DestroyEntityComponents(EntityID id)
{
	EntityComponentsMap::iterator iter = mEntityComponentsMap.find(id);
	if (iter == mEntityComponentsMap.end())
		return;
	ComponentsList& cmpList = *iter->second;
	for (ComponentsList::iterator i=cmpList.begin(); i!=cmpList.end(); ++i)
	{
		(*i)->Destroy();
	}
	for (ComponentsList::iterator i=cmpList.begin(); i!=cmpList.end(); ++i)
	{
		delete (*i);
	}
	delete iter->second;
	mEntityComponentsMap.erase(iter);
}
