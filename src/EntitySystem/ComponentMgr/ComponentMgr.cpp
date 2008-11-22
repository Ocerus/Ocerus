#include "Common.h"
#include "ComponentMgr.h"
#include "Component.h"
#include "ComponentDescription.h"

#include "../Components/CmpMaterial.h"
#include "../Components/CmpPlatformLinks.h"
#include "../Components/CmpPlatformParams.h"
#include "../Components/CmpPlatformPhysics.h"
#include "../Components/CmpPlatformStats.h"
#include "../Components/CmpPlatformVisual.h"

using namespace EntitySystem;

ComponentMgr::ComponentMgr()
{
	mComponentCreationMethod[NUM_COMPONENT_TYPES-1] = 0;

	// register components
	mComponentCreationMethod[CT_MATERIAL] = CmpMaterial::CreateMe;
	mComponentCreationMethod[CT_PLATFORM_LINKS] = CmpPlatformLinks::CreateMe;
	mComponentCreationMethod[CT_PLATFORM_PARAMS] = CmpPlatformParams::CreateMe;
	mComponentCreationMethod[CT_PLATFORM_PHYSICS] = CmpPlatformPhysics::CreateMe;
	mComponentCreationMethod[CT_PLATFORM_STATS] = CmpPlatformStats::CreateMe;
	mComponentCreationMethod[CT_PLATFORM_VISUAL] = CmpPlatformVisual::CreateMe;

	assert(mComponentCreationMethod[NUM_COMPONENT_TYPES-1]);
}

ComponentMgr::~ComponentMgr() 
{
	assert(mEntityComponentsMap.size()==0 && "ComponentsMap not empty. (EntityMgr should erase it before deleting ComponentMgr)");
}

EntityComponentsIterator ComponentMgr::GetEntityComponents(EntityID id)
{
	EntityComponentsMap::iterator eci = mEntityComponentsMap.find(id);
	assert(eci != mEntityComponentsMap.end());
	return EntityComponentsIterator(eci->second);
}

bool ComponentMgr::CreateComponent(EntityHandle h, ComponentDescription& desc)
{
	Component* cmp = mComponentCreationMethod[desc.GetType()]();
	cmp->SetOwner(h);
	cmp->Init(desc);
	mEntityComponentsMap[h.GetID()].push_back(cmp);
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