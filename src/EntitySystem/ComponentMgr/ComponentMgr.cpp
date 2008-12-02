#include "Common.h"
#include "ComponentMgr.h"
#include "Component.h"
#include "ComponentDescription.h"

#include "../Components/CmpMaterial.h"
#include "../Components/CmpPlatformLinks.h"
#include "../Components/CmpPlatformParams.h"
#include "../Components/CmpPlatformPhysics.h"
#include "../Components/CmpPlatformLogic.h"
#include "../Components/CmpPlatformVisual.h"
#include "../Components/CmpShipLogic.h"
#include "../Components/CmpShipPhysics.h"
#include "../Components/CmpShipVisual.h"
#include "../Components/CmpPlatformItem.h"
#include "../Components/CmpEngineParams.h"
#include "../Components/CmpEngine.h"

using namespace EntitySystem;

ComponentMgr::ComponentMgr()
{
	mComponentCreationMethod[NUM_COMPONENT_TYPES-1] = 0;

	// register components
	mComponentCreationMethod[CT_MATERIAL] = CmpMaterial::CreateMe;
	mComponentCreationMethod[CT_PLATFORM_LINKS] = CmpPlatformLinks::CreateMe;
	mComponentCreationMethod[CT_PLATFORM_PARAMS] = CmpPlatformParams::CreateMe;
	mComponentCreationMethod[CT_PLATFORM_PHYSICS] = CmpPlatformPhysics::CreateMe;
	mComponentCreationMethod[CT_PLATFORM_LOGIC] = CmpPlatformLogic::CreateMe;
	mComponentCreationMethod[CT_PLATFORM_VISUAL] = CmpPlatformVisual::CreateMe;
	mComponentCreationMethod[CT_SHIP_LOGIC] = CmpShipLogic::CreateMe;
	mComponentCreationMethod[CT_SHIP_PHYSICS] = CmpShipPhysics::CreateMe;
	mComponentCreationMethod[CT_SHIP_VISUAL] = CmpShipVisual::CreateMe;
	mComponentCreationMethod[CT_PLATFORM_ITEM] = CmpPlatformItem::CreateMe;
	mComponentCreationMethod[CT_ENGINE_PARAMS] = CmpEngineParams::CreateMe;
	mComponentCreationMethod[CT_ENGINE] = CmpEngine::CreateMe;

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
	assert(desc.GetType() < NUM_COMPONENT_TYPES && desc.GetType() >= 0);
	Component* cmp = mComponentCreationMethod[desc.GetType()]();
	mEntityComponentsMap[h.GetID()].push_back(cmp);
	cmp->SetOwner(h);
	cmp->Init(desc);
	return true;
}

void ComponentMgr::DestroyEntityComponents(EntityID id)
{
	EntityComponentsMap::iterator iter = mEntityComponentsMap.find(id);
	assert(iter!=mEntityComponentsMap.end());
	ComponentsList& cmpList = iter->second;
	for (ComponentsList::iterator i=cmpList.begin(); i!=cmpList.end(); ++i)
	{
		(*i)->Deinit();
		DYN_DELETE (*i);
	}
	mEntityComponentsMap.erase(iter);
} 