#include "Common.h"
#include "ComponentMgr.h"
#include "Component.h"

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
	mComponentCreationMethod[CT_MATERIAL] = (ComponentCreationMethod)CmpMaterial::GetClassRTTI()->GetClassFactory();
	mComponentCreationMethod[CT_PLATFORM_LINKS] = (ComponentCreationMethod)CmpPlatformLinks::GetClassRTTI()->GetClassFactory();
	mComponentCreationMethod[CT_PLATFORM_PARAMS] = (ComponentCreationMethod)CmpPlatformParams::GetClassRTTI()->GetClassFactory();
	mComponentCreationMethod[CT_PLATFORM_PHYSICS] = (ComponentCreationMethod)CmpPlatformPhysics::GetClassRTTI()->GetClassFactory();
	mComponentCreationMethod[CT_PLATFORM_LOGIC] = (ComponentCreationMethod)CmpPlatformLogic::GetClassRTTI()->GetClassFactory();
	mComponentCreationMethod[CT_PLATFORM_VISUAL] = (ComponentCreationMethod)CmpPlatformVisual::GetClassRTTI()->GetClassFactory();
	mComponentCreationMethod[CT_SHIP_LOGIC] = (ComponentCreationMethod)CmpShipLogic::GetClassRTTI()->GetClassFactory();
	mComponentCreationMethod[CT_SHIP_PHYSICS] = (ComponentCreationMethod)CmpShipPhysics::GetClassRTTI()->GetClassFactory();
	mComponentCreationMethod[CT_SHIP_VISUAL] = (ComponentCreationMethod)CmpShipVisual::GetClassRTTI()->GetClassFactory();
	mComponentCreationMethod[CT_PLATFORM_ITEM] = (ComponentCreationMethod)CmpPlatformItem::GetClassRTTI()->GetClassFactory();
	mComponentCreationMethod[CT_ENGINE_PARAMS] = (ComponentCreationMethod)CmpEngineParams::GetClassRTTI()->GetClassFactory();
	mComponentCreationMethod[CT_ENGINE] = (ComponentCreationMethod)CmpEngine::GetClassRTTI()->GetClassFactory();

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

Component* ComponentMgr::CreateComponent(EntityHandle h, const eComponentType type)
{
	assert(type < NUM_COMPONENT_TYPES && type >= 0);
	Component* cmp = mComponentCreationMethod[type]();
	EntityComponentsMap::const_iterator entIt = mEntityComponentsMap.find(h.GetID());
	if (entIt == mEntityComponentsMap.end())
	{
		mEntityComponentsMap[h.GetID()] = DYN_NEW ComponentsList();
		entIt = mEntityComponentsMap.find(h.GetID());
	}
	entIt->second->push_back(cmp);
	cmp->SetOwner(h);
	cmp->Init();
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
		(*i)->Clean();
		DYN_DELETE (*i);
	}
	DYN_DELETE iter->second;
	mEntityComponentsMap.erase(iter);
} 

bool EntitySystem::ComponentMgr::GetEntityProperties( const EntityID id, PropertyList& out, const PropertyAccessFlags flagMask )
{
	out.clear();
	EntityComponentsMap::iterator iter = mEntityComponentsMap.find(id);
	if (iter == mEntityComponentsMap.end())
		return false;
	ComponentsList& cmpList = *iter->second;
	for (ComponentsList::iterator i=cmpList.begin(); i!=cmpList.end(); ++i)
		(*i)->GetRTTI()->EnumProperties(*i, out, flagMask);
	return true;
}

PropertyHolder EntitySystem::ComponentMgr::GetEntityProperty( const EntityHandle h, const StringKey key, const PropertyAccessFlags mask )
{
	EntityComponentsMap::iterator iter = mEntityComponentsMap.find(h.GetID());
	if (iter == mEntityComponentsMap.end())
		return PropertyHolder();
	ComponentsList& cmpList = *iter->second;
	for (ComponentsList::iterator i=cmpList.begin(); i!=cmpList.end(); ++i){
		AbstractProperty* prop = (*i)->GetRTTI()->GetProperty(key, mask);
		if (prop)
			return PropertyHolder(*i, prop);
	}
	return PropertyHolder();	
}