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
#include "../Components/CmpPlatformItemParams.h"
#include "../Components/CmpEngineParams.h"
#include "../Components/CmpEngine.h"
#include "../Components/CmpWeaponParams.h"
#include "../Components/CmpWeapon.h"
#include "../Components/CmpAmmoParams.h"
#include "../Components/CmpProjectile.h"

using namespace EntitySystem;

ComponentMgr::ComponentMgr()
{
	mComponentCreationMethod[NUM_COMPONENT_TYPES-1] = 0;

	// register components
	#define COMPONENT_TYPE(id, cls) mComponentCreationMethod[id] = (ComponentCreationMethod)cls::GetClassRTTI()->GetClassFactory();
	#include "ComponentTypes.h"
	#undef COMPONENT_TYPE

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

PropertyHolderMediator EntitySystem::ComponentMgr::GetEntityProperty( const EntityHandle h, const StringKey key, const PropertyAccessFlags mask ) const
{
	EntityComponentsMap::const_iterator iter = mEntityComponentsMap.find(h.GetID());
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