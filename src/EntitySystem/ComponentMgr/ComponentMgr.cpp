#include "Common.h"
#include "ComponentMgr.h"
#include "Component.h"

#include "../Components/_ComponentHeaders.h"

using namespace EntitySystem;

ComponentMgr::ComponentMgr()
{
	mComponentCreationMethod[NUM_COMPONENT_TYPES-1] = 0;

	// register components
	#define COMPONENT_TYPE(cls) mComponentCreationMethod[CT_##cls] = (ComponentCreationMethod)cls::GetClassRTTI()->GetClassFactory();
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

ComponentID ComponentMgr::CreateComponent(const EntityID id, const eComponentType type)
{
	OC_ASSERT(type < NUM_COMPONENT_TYPES && type >= 0);
	Component* cmp = mComponentCreationMethod[type]();
	EntityComponentsMap::const_iterator entIt = mEntityComponentsMap.find(id);
	OC_ASSERT(entIt != mEntityComponentsMap.end());
	entIt->second->push_back(cmp);
	cmp->SetOwner(EntityHandle(id));
	cmp->_SetType(type);
	cmp->Create();
	ComponentID cmpID = entIt->second->size()-1;

	ocTrace << "Created component " << cmpID << " in entity " << id << " of type " << type;
	return cmpID;
}

void ComponentMgr::DestroyEntityComponents(EntityID id)
{
	EntityComponentsMap::iterator iter = mEntityComponentsMap.find(id);
	if (iter == mEntityComponentsMap.end()) return;

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

void EntitySystem::ComponentMgr::DestroyComponent( const EntityID id, const ComponentID componentToDestroy )
{
	EntityComponentsMap::iterator iter = mEntityComponentsMap.find(id);
	if (iter == mEntityComponentsMap.end())
	{
		// no components
		return;
	}

	ComponentsList* components = iter->second;
	if ((size_t)componentToDestroy >= components->size())
	{
		ocError << "Invalid component ID to destroy: " << componentToDestroy;
		return;
	}

	Component* cmp = (*components)[componentToDestroy];
	cmp->Destroy();
	delete cmp;

	components->erase(components->begin() + componentToDestroy);
}

Component* EntitySystem::ComponentMgr::GetEntityComponent( const EntityID id, const ComponentID cmpID ) const
{
	EntityComponentsMap::const_iterator iter = mEntityComponentsMap.find(id);
	if (iter == mEntityComponentsMap.end())
	{
		ocError << "Entity " << id << " doesn't have components";
		return 0;
	}

	OC_DASSERT(iter->second);

	if ((size_t)cmpID >= iter->second->size())
	{
		ocError << "Invalid ComponentID of " << cmpID << " when trying to get a component of entity " << id;
		return 0;
	}

	return (*iter->second)[cmpID];	
}

int32 EntitySystem::ComponentMgr::GetNumberOfEntityComponents( const EntityID id ) const
{
	EntityComponentsMap::const_iterator iter = mEntityComponentsMap.find(id);
	if (iter == mEntityComponentsMap.end())
		return 0; // no components

	OC_DASSERT(iter->second);
	return iter->second->size();
}

void EntitySystem::ComponentMgr::PrepareForEntity( const EntityID id )
{
	EntityComponentsMap::const_iterator entIt = mEntityComponentsMap.find(id);
	if (entIt == mEntityComponentsMap.end())
	{
		mEntityComponentsMap[id] = new ComponentsList();
	}
}

void EntitySystem::ComponentMgr::EnumComponentDependencies(const eComponentType type, Reflection::ComponentDependencyList& out) const
{
	switch (type)
	{
	  #define COMPONENT_TYPE(cls) case CT_##cls : cls::GetClassRTTI()->EnumComponentDependencies(out); break;
	  #include "../Components/_ComponentTypes.h"
	  #undef COMPONENT_TYPE
	  default: break;
	}
}