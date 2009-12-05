#include "Common.h"
#include "EntityMgr.h"
#include "EntityDescription.h"
#include "../ComponentMgr/ComponentMgr.h"
#include "../ComponentMgr/Component.h"
#include "../../ResourceSystem/XMLResource.h"

namespace EntitySystem
{
	/// This struct holds info about an instance of an entity in the system.
	struct EntityInfo
	{
		EntityInfo(const string& name, const EntityHandle prototype):
			mFullyInited(false),
			mName(name),
			mPrototype(prototype)
		{

		}

		/// True if this entity was fully inited in the system by the user.
		bool mFullyInited;
		/// Human readable user ID.
		string mName;
		/// Handle to the prototype entity (can be invalid).
		EntityHandle mPrototype;

	private:
		EntityInfo(const EntityInfo& rhs);
		EntityInfo& operator=(const EntityInfo& rhs);
	};

	/// This struct holds info specific for prototype entities.
	struct PrototypeInfo
	{
		PrototypeInfo(void): mCopy(INVALID_ENTITY_ID), mInstancesCount(0) {}

		/// Copy of the prototype to hold old values of properties.
		EntityID mCopy;
		/// Number of instances of this prototype.
		int32 mInstancesCount;
		/// Set of properties marked as shared with instances of the prototype.
		set<StringKey> mSharedProperties;

	private:
		PrototypeInfo(const PrototypeInfo& rhs);
		PrototypeInfo& operator=(const PrototypeInfo& rhs);
	};
}


using namespace EntitySystem;

EntityMgr::EntityMgr()
{
	ocInfo << "*** EntityMgr init ***";

	mComponentMgr = new ComponentMgr();

}

EntityMgr::~EntityMgr()
{
	DestroyAllEntities();
	delete mComponentMgr;
}

EntityMessage::eResult EntityMgr::PostMessage(EntityID targetEntity, const EntityMessage& msg)
{
	EntityMap::iterator ei = mEntities.find(targetEntity);
	if (ei == mEntities.end())
	{
		ocError << "Can't post message: Can't find entity with ID '" << targetEntity << "'";
		return EntityMessage::RESULT_ERROR;
	}
	if (EntityHandle::IsPrototypeID(targetEntity))
	{
		// the prototype nor its copy can't receive any messages
		return EntityMessage::RESULT_IGNORED;
	}
	if (msg.type != EntityMessage::INIT && msg.type != EntityMessage::POST_INIT && !ei->second->mFullyInited)
	{
		ocError << "Can't post message: Entity with ID '" << targetEntity << "' is not initialized";
		return EntityMessage::RESULT_ERROR;
	}
	if (msg.type != EntityMessage::INIT && msg.type == EntityMessage::POST_INIT && ei->second->mFullyInited)
	{
		ocError << "Can't post message: Entity with ID '" << targetEntity << "' is already initialized";
		return EntityMessage::RESULT_ERROR;
	}
	if (!msg.AreParametersValid())
	{
		ocError << "Can't post message: Parameters passed are not valid";
		return EntityMessage::RESULT_ERROR;
	}


	if (msg.type == EntityMessage::POST_INIT) ei->second->mFullyInited = true;

	EntityMessage::eResult result = EntityMessage::RESULT_IGNORED;
	for (EntityComponentsIterator iter = mComponentMgr->GetEntityComponents(targetEntity); iter.HasMore(); ++iter)
	{
		EntityMessage::eResult r = (*iter)->HandleMessage(msg);
		if ((r == EntityMessage::RESULT_ERROR)
			|| (r == EntityMessage::RESULT_OK && result == EntityMessage::RESULT_IGNORED))
		{
			result = r;
		}
	}
	return result;
}

void EntityMgr::BroadcastMessage(const EntityMessage& msg)
{
	for (EntityMap::const_iterator i = mEntities.begin(); i!=mEntities.end(); ++i)
		PostMessage(i->first, msg);
}

EntityHandle EntityMgr::CreateEntity(EntityDescription& desc)
{
	OC_ASSERT(mComponentMgr);

	bool isPrototype = desc.mKind == EntityDescription::EK_PROTOTYPE || desc.mKind == EntityDescription::EK_PROTOTYPE_COPY;

	if (desc.mComponents.size() == 0)
	{
		ocWarning << "Attempting to create an entity without components";
		return EntityHandle::Null; // no components, so we can't create the entity
	}
	if (isPrototype && desc.mPrototype != INVALID_ENTITY_ID)
	{
		ocError << "Can't create an entity prototype which is an instance of another prototype; invalidating the prototype link...";
		desc.mPrototype = INVALID_ENTITY_ID;
	}
	if (desc.mPrototype != INVALID_ENTITY_ID && mEntities.find(desc.mPrototype)==mEntities.end())
	{
		ocError << "Can't create an entity: it's prototype can't be found";
		return EntityHandle::Null;
	}

	// create the handle for the new entity
	EntityHandle entityHandle;
	if (desc.mID != INVALID_ENTITY_ID)
	{
		entityHandle = EntityHandle::CreateHandleFromID(desc.mID);
	}
	else
	{
		if (isPrototype) entityHandle = EntityHandle::CreateUniquePrototypeHandle();
		else entityHandle = EntityHandle::CreateUniqueHandle();
	}

	bool dependencyFailure = false;
	set<eComponentType> createdComponentTypes;
	for (ComponentDependencyList::const_iterator cmpDescIt=desc.mComponents.begin(); cmpDescIt!=desc.mComponents.end(); ++cmpDescIt)
	{
		eComponentType cmpType = *cmpDescIt;

		// create the component
		ComponentID cmpID = mComponentMgr->CreateComponent(entityHandle.GetID(), cmpType);
		Component* cmp = mComponentMgr->GetEntityComponent(entityHandle.GetID(), cmpID);
		OC_ASSERT(cmp);

		// check dependencies of the component
		ComponentDependencyList depList;
		cmp->GetRTTI()->EnumComponentDependencies(depList);
		for (ComponentDependencyList::iterator depIt=depList.begin(); depIt!=depList.end(); ++depIt)
		{
			if (createdComponentTypes.find(*depIt) == createdComponentTypes.end())
			{
				dependencyFailure = true;
				ocError << "Component dependency failure on entity " << entityHandle;
				ocError << "Component " << GetComponentTypeName(cmpType) << " depends on " << GetComponentTypeName(*depIt) << " which was not created yet";
			}
		}

		// take a note that this component type was already created
		createdComponentTypes.insert(cmpType);
	}

	// inits etity attributes
	mEntities[entityHandle.GetID()] = new EntityInfo(desc.mName, EntityHandle(desc.mPrototype));
	if (desc.mKind == EntityDescription::EK_PROTOTYPE) mPrototypes[entityHandle.GetID()] = new PrototypeInfo();

	// link the entity to its prototype
	if (mPrototypes.find(desc.mPrototype) != mPrototypes.end())
	{
		LinkEntityToPrototype(entityHandle.GetID(), desc.mPrototype);
	}

	// security checks
	if (dependencyFailure)
	{
		DestroyEntity(entityHandle);
		return entityHandle; // do like nothing's happened, but don't enum properties or they will access invalid memory
	}

	return entityHandle;
}


void EntityMgr::DestroyEntity(const EntityHandle entityToDestroy)
{
	mEntityDestroyQueue.push_back(entityToDestroy.GetID());
}

void EntitySystem::EntityMgr::ProcessDestroyQueue( void )
{
	for (EntityQueue::const_iterator it=mEntityDestroyQueue.begin(); it!=mEntityDestroyQueue.end(); ++it)
	{
		EntityMap::iterator mapIt = mEntities.find(*it);
		if (mapIt != mEntities.end()) // it can happen that we added one entity multiple times to the queue
		{
			DestroyEntityImmediately(*it, false);
			mEntities.erase(mapIt);
			mPrototypes.erase(*it);
		}
	}
	mEntityDestroyQueue.clear();
}

void EntityMgr::DestroyAllEntities()
{
	OC_ASSERT(mComponentMgr);
	for (EntityMap::const_iterator i = mEntities.begin(); i!=mEntities.end(); ++i)
	{
		DestroyEntityImmediately(i->first, false);
	}
	mEntityDestroyQueue.clear(); // new entities could be marked for removal during deleting another entities
	mEntities.clear();
	mPrototypes.clear();
}

void EntityMgr::DestroyEntityImmediately( const EntityID entityToDestroy, const bool erase )
{
	PrototypeMap::iterator protIt = mPrototypes.find(entityToDestroy);
	if (protIt != mPrototypes.end())
	{
		if (erase) DestroyEntityImmediately(protIt->second->mCopy, erase);
		else DestroyEntity(protIt->second->mCopy); // if we can't erase the entity from the map, it has to be deleted in the next round

		delete protIt->second;
		protIt->second = 0;

		if (erase) mPrototypes.erase(protIt);
	}

	EntityMap::iterator entityIt = mEntities.find(entityToDestroy);
	if (entityIt != mEntities.end())
	{
		mComponentMgr->DestroyEntityComponents(entityToDestroy);

		if (entityIt->second->mPrototype.IsValid())
		{
			UnlinkEntityFromPrototype(entityToDestroy);
		}

		delete entityIt->second;
		entityIt->second = 0;

		if (erase) mEntities.erase(entityIt);
	}
}

bool EntitySystem::EntityMgr::IsEntityInited( const EntityHandle h ) const
{
	EntityMap::const_iterator ei = mEntities.find(h.GetID());
	if (ei == mEntities.end())
	{
		ocError << "Can't find entity " << h;
		return false;
	}
	return ei->second->mFullyInited;
}

bool EntitySystem::EntityMgr::GetEntityProperties( const EntityHandle entity, PropertyList& out, const PropertyAccessFlags flagMask ) const
{
	out.clear();

	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(entity.GetID()); it.HasMore(); ++it)
	{
		(*it)->GetRTTI()->EnumProperties(*it, out, flagMask);
	}

	return true;
}

PropertyHolder EntitySystem::EntityMgr::GetEntityProperty( const EntityHandle entity, const StringKey key, const PropertyAccessFlags flagMask /*= 0xff*/ ) const
{
	OC_DASSERT(mComponentMgr);
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(entity.GetID()); it.HasMore(); ++it)
	{
		AbstractProperty* prop = (*it)->GetRTTI()->GetProperty(key, flagMask);
		if (prop) return PropertyHolder(*it, prop);
	}


	// property not found, print some info about why
	ocError << "EntityMgr: unknown property '" << key << "'";
	PropertyList propertyList;
	string propertiesString;
	GetEntityProperties(entity, propertyList, flagMask);
	for (PropertyList::iterator it=propertyList.begin(); it!=propertyList.end(); )
	{
		propertiesString += it->GetName();
		++it;
		if (it==propertyList.end()) propertiesString += ".";
		else propertiesString += ", ";
	}
	ocError << "Available properties (flags=" << (uint32)flagMask << "): " << propertiesString;

	// return an invalid holder
	return PropertyHolder();
}

Reflection::PropertyHolder EntitySystem::EntityMgr::GetEntityComponentProperty( const EntityHandle entity, const ComponentID component, const StringKey propertyKey, const PropertyAccessFlags flagMask /*= PA_FULL_ACCESS*/ ) const
{
	if (component >= mComponentMgr->GetNumberOfEntityComponents(entity.GetID()))
	{
		ocError << "Invalid component ID: " << component;
		return PropertyHolder();
	}

	Component* cmp = mComponentMgr->GetEntityComponent(entity.GetID(), component);
	AbstractProperty* prop = cmp->GetRTTI()->GetProperty(propertyKey, flagMask);
	if (prop) return PropertyHolder(cmp, prop);


	// property not found, print some info about why
	ocError << "EntityMgr: unknown property '" << propertyKey << "'";
	PropertyList propertyList;
	string propertiesString;
	GetEntityComponentProperties(entity.GetID(), component, propertyList, flagMask);
	for (PropertyList::iterator it=propertyList.begin(); it!=propertyList.end(); )
	{
		propertiesString += it->GetName();
		++it;
		if (it==propertyList.end()) propertiesString += ".";
		else propertiesString += ", ";
	}
	ocError << "Available properties (flags=" << (uint32)flagMask << "): " << propertiesString;

	// return an invalid holder
	return PropertyHolder();
}

bool EntitySystem::EntityMgr::HasEntityProperty( const EntityHandle entity, const StringKey key, const PropertyAccessFlags flagMask /*= PA_FULL_ACCESS*/ ) const
{
	OC_DASSERT(mComponentMgr);
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(entity.GetID()); it.HasMore(); ++it)
	{
		AbstractProperty* prop = (*it)->GetRTTI()->GetProperty(key, flagMask);
		if (prop) return true;
	}
	return false;
}

EntitySystem::EntityHandle EntitySystem::EntityMgr::FindFirstEntity( const string& name )
{
	for(EntityMap::const_iterator i=mEntities.begin(); i!=mEntities.end(); ++i)
		if (i->second->mName.compare(name) == 0)
			return i->first;
	return EntityHandle::Null;
}

void EntitySystem::EntityMgr::LoadEntityPropertyFromXML( const EntityID entityID, const ComponentID componentID, PrototypeInfo* prototypeInfo, ResourceSystem::XMLResourcePtr xml, ResourceSystem::XMLNodeIterator& xmlPropertyIterator )
{
	StringKey propertyKey(*xmlPropertyIterator);

	// the property was stored in the XML file for the prototype, so it's a shared property
	if (prototypeInfo) prototypeInfo->mSharedProperties.insert(propertyKey);

	PropertyHolder prop = GetEntityComponentProperty(entityID, componentID, propertyKey);

	if (prop.GetType() == PT_VECTOR2_ARRAY)
	{
		vector<Vector2> vertices;
		for (ResourceSystem::XMLNodeIterator vertIt=xml->IterateChildren(xmlPropertyIterator); vertIt!=xml->EndChildren(xmlPropertyIterator); ++vertIt)
		{
			if ((*vertIt).compare("Vertex") == 0)
				vertices.push_back(vertIt.GetChildValue<Vector2>());
			else
				ocError << "XML:Entity: Expected 'Vertex', found '" << *vertIt << "'";
		}

		Array<Vector2> vertArray(vertices.size());
		for (uint32 i=0; i<vertices.size(); ++i)
		{
			vertArray[i] = vertices[i];
		}
		prop.SetValue<Array<Vector2>*>(&vertArray);
	}
	else
	{
		prop.SetValueFromString(xmlPropertyIterator.GetChildValue<string>());
	}
}

void EntitySystem::EntityMgr::LoadEntityFromXML( ResourceSystem::XMLNodeIterator &entIt, const bool isPrototype, ResourceSystem::XMLResourcePtr xml )
{
	// init the entity description
	EntityDescription desc;
	desc.Reset();
	if (entIt.HasAttribute("Name"))	desc.SetName(entIt.GetAttribute<string>("Name"));
	if (entIt.HasAttribute("ID")) desc.SetDesiredID(entIt.GetAttribute<EntityID>("ID"));
	if (entIt.HasAttribute("Prototype")) desc.SetPrototype(entIt.GetAttribute<EntityID>("Prototype"));
	if (isPrototype) desc.SetKind(EntityDescription::EK_PROTOTYPE);

	// add component types
	for (ResourceSystem::XMLNodeIterator cmpIt=xml->IterateChildren(entIt); cmpIt!=xml->EndChildren(entIt); ++cmpIt)
	{
		if ((*cmpIt).compare("Component") == 0)	desc.AddComponent(DetectComponentType(cmpIt.GetAttribute<string>("Type")));
	}

	// create the entity
	EntityHandle entity = gEntityMgr.CreateEntity(desc);
	PrototypeInfo* prototypeInfo = 0;
	if (isPrototype)
	{
		OC_ASSERT(mPrototypes.find(entity.GetID()) != mPrototypes.end());
		prototypeInfo = mPrototypes[entity.GetID()];
	}

	// set properties loaded from the file
	ComponentID currentComponent = -1;
	for (ResourceSystem::XMLNodeIterator cmpIt=xml->IterateChildren(entIt); cmpIt!=xml->EndChildren(entIt); ++cmpIt)
	{
		// skip unwanted data
		if ((*cmpIt).compare("Component") != 0)
			continue;

		++currentComponent;

		for (ResourceSystem::XMLNodeIterator xmlPropertyIter=xml->IterateChildren(cmpIt); xmlPropertyIter!=xml->EndChildren(cmpIt); ++xmlPropertyIter)
		{
			// skip unwanted data
			if ((*xmlPropertyIter).compare("Type") == 0)
				continue;
			if (!HasEntityComponentProperty(entity.GetID(), currentComponent, *xmlPropertyIter, PA_INIT))
			{
				ocError << "XML:Entity: Unknown entity property '" << *xmlPropertyIter << "' (it might not be marked as initable (PA_INIT))";
			}
			else
			{
				LoadEntityPropertyFromXML(entity.GetID(), currentComponent, prototypeInfo, xml, xmlPropertyIter);
			}
		}
	}

	// finish init
	// note: the prototype copy can't be created here since all prototypes must be loaded first before any copy is created.
	//       otherwise it would screw up their IDs.
	entity.FinishInit();
}

bool EntitySystem::EntityMgr::LoadEntitiesFromResource( ResourceSystem::ResourcePtr res, const bool isPrototype )
{
    ResourceSystem::XMLResourcePtr xml = res;
	if (xml == 0)
	{
		ocError << "XML: Can't load data";
		return false;
	}

	for (ResourceSystem::XMLNodeIterator entIt=xml->IterateTopLevel(); entIt!=xml->EndTopLevel(); ++entIt)
	{
		if ((*entIt).compare("Entity") == 0)
		{
			LoadEntityFromXML(entIt, isPrototype, xml);
		}
		else
		{
			ocError << "XML: Expected 'Entity', found '" << *entIt << "'";
		}
	}

	return false;
}

bool EntitySystem::EntityMgr::EntityExists( const EntityHandle h ) const
{
	if (!h.IsValid())
		return false;
	return mEntities.find(h.GetID()) != mEntities.end();
}

bool EntitySystem::EntityMgr::HasEntityComponentOfType(const EntityHandle entity, const eComponentType componentType)
{
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(entity.GetID()); it.HasMore(); ++it)
	{
		if ((*it)->GetType() == componentType) return true;
	}
	return false;
}

bool EntitySystem::EntityMgr::GetEntityComponentTypes(const EntityHandle entity, ComponentTypeList& out)
{
	out.clear();
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(entity.GetID()); it.HasMore(); ++it)
	{
		out.push_back((*it)->GetType());
	}
	return true;
}

void EntitySystem::EntityMgr::UpdatePrototypeCopy( const EntityHandle prototype )
{
	EntityMap::iterator entityIter = mEntities.find(prototype.GetID());
	if (entityIter == mEntities.end())
	{
		ocError << "Can't update prototype copy; can't find entity " << prototype;
		return;
	}
	PrototypeMap::iterator prototypeIter = mPrototypes.find(prototype.GetID());
	if (prototypeIter == mPrototypes.end())
	{
		ocError << "Can't update prototype copy; entity " << prototype << " is not a prototype";
		return;
	}

	PrototypeInfo* prototypeInfo = prototypeIter->second;

	// recreate the copy from scratch
	if (mEntities.find(prototypeInfo->mCopy) != mEntities.end())
	{
		DestroyEntityImmediately(prototypeInfo->mCopy, true);
	}
	EntityDescription desc;
	desc.Reset();
	desc.SetKind(EntityDescription::EK_PROTOTYPE_COPY);
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(prototype.GetID()); it.HasMore(); ++it)
	{
		desc.AddComponent((*it)->GetType());
	}
	EntityHandle copyHandle = CreateEntity(desc);
	prototypeInfo->mCopy = copyHandle.GetID();

	// copy the properties
	// only initable properties can be copied
	PropertyList propList;
	GetEntityProperties(prototype, propList, PA_INIT);
	for (PropertyList::iterator it=propList.begin(); it!=propList.end(); ++it)
	{
		PropertyHolder copyProperty = GetEntityProperty(copyHandle, it->GetKey(), PA_INIT);
		copyProperty.CopyFrom(*it);
	}
}

void EntitySystem::EntityMgr::UpdatePrototypeInstances( const EntityHandle prototype )
{
	for (EntityMap::iterator it=mEntities.begin(); it!=mEntities.end(); ++it)
	{
		if (it->second->mPrototype == prototype)
		{
			UpdatePrototypeInstance(prototype.GetID(), it->first, false);
		}
	}
	UpdatePrototypeCopy(prototype);
}

void EntitySystem::EntityMgr::UpdatePrototypeInstance( const EntityID prototype, const EntityID instance, const bool forceOverwrite )
{
	OC_ASSERT(mPrototypes.find(prototype) != mPrototypes.end());
	PrototypeInfo* prototypeInfo = mPrototypes[prototype];

	ComponentTypeList prototypeComponentTypes;
	ComponentTypeList instanceComponentTypes;
	GetEntityComponentTypes(prototype, prototypeComponentTypes);
	GetEntityComponentTypes(instance, instanceComponentTypes);
	if (prototypeComponentTypes.size() > instanceComponentTypes.size())
	{
		ocError << "Cannot propagate prototype " << prototype << " to instance " << instance << "; not enough components in instance";
		return;
	}

	// iterate through components of the prototype
	ComponentID currentComponent = 0;
	ComponentTypeList::iterator protIt=prototypeComponentTypes.begin();
	ComponentTypeList::iterator instIt=instanceComponentTypes.begin();
	for (; protIt!=prototypeComponentTypes.end(); ++protIt, ++instIt, ++currentComponent)
	{
		if (*protIt != *instIt)
		{
			ocError << "Cannot propagate prototype " << prototype << " to instance " << instance << "; components mismatch";
			return;
		}

		// update shared properties
		PropertyList prototypeProperties;
		GetEntityComponentProperties(prototype, currentComponent, prototypeProperties);
		for (PropertyList::iterator protPropIter=prototypeProperties.begin(); protPropIter!=prototypeProperties.end(); ++protPropIter)
		{
			if (prototypeInfo->mSharedProperties.find(protPropIter->GetKey()) == prototypeInfo->mSharedProperties.end())
				continue;

			StringKey propertyKey = protPropIter->GetKey();
			PropertyHolder instanceProperty = GetEntityComponentProperty(instance, currentComponent, propertyKey);
			PropertyHolder copyProperty = GetEntityComponentProperty(prototypeInfo->mCopy, currentComponent, propertyKey);
			PropertyHolder prototypeProperty = *protPropIter;

			// if the property has different value from the prototype copy, then it means the property was specialized
			// by the user and we should leave it alone
			if (forceOverwrite || copyProperty.IsEqual(instanceProperty))
			{
				instanceProperty.CopyFrom(prototypeProperty);
			}
		}
	}
}

bool EntitySystem::EntityMgr::IsEntityPrototype( const EntityHandle entity ) const
{
	if (!entity.IsValid() || !EntityHandle::IsPrototypeID(entity.GetID()))
		return false;

	return mPrototypes.find(entity.GetID()) != mPrototypes.end();
}

EntitySystem::ComponentID EntitySystem::EntityMgr::AddComponentToEntity( const EntityHandle entity, const eComponentType componentType )
{
	OC_ASSERT(mComponentMgr);
	return mComponentMgr->CreateComponent(entity.GetID(), componentType);
}

bool EntitySystem::EntityMgr::IsPrototypePropertyShared( const EntityHandle prototype, const StringKey testedProperty ) const
{
	if (!IsEntityPrototype(prototype))
	{
		ocError << "Entity " << prototype << " is not a prototype";
		return false;
	}

	PrototypeMap::const_iterator protIter = mPrototypes.find(prototype.GetID());
	PrototypeInfo* protInfo = protIter->second;
	return protInfo->mSharedProperties.find(testedProperty) != protInfo->mSharedProperties.end();
}

void EntitySystem::EntityMgr::SetPrototypePropertyShared( const EntityHandle prototype, const StringKey propertyToMark )
{
	if (!IsEntityPrototype(prototype))
	{
		ocError << "Entity " << prototype << " is not a prototype";
		return;
	}

	PropertyHolder prop = GetEntityProperty(prototype.GetID(), propertyToMark);
	if (!(prop.GetAccessFlags() & Reflection::PA_INIT))
	{
		ocError << "Only properties marked as initable (PA_INIT) can be marked as shared";
		return;
	}

	PrototypeMap::const_iterator protIter = mPrototypes.find(prototype.GetID());
	PrototypeInfo* protInfo = protIter->second;
	protInfo->mSharedProperties.insert(propertyToMark);
}

void EntitySystem::EntityMgr::SetPrototypePropertyNonShared( const EntityHandle prototype, const StringKey propertyToMark )
{
	if (!IsEntityPrototype(prototype))
	{
		ocError << "Entity " << prototype << " is not a prototype";
		return;
	}

	PrototypeMap::const_iterator protIter = mPrototypes.find(prototype.GetID());
	PrototypeInfo* protInfo = protIter->second;
	protInfo->mSharedProperties.erase(propertyToMark);
}

void EntitySystem::EntityMgr::DestroyEntityComponent( const EntityHandle entity, const ComponentID componentToDestroy )
{
	OC_ASSERT(mComponentMgr);
	EntityMap::iterator entityIter = mEntities.find(entity.GetID());
	if (entityIter == mEntities.end())
	{
		ocError << "Entity " << entity << " not found";
		return;
	}

	// make a security check if we are not removing a component linked to the prototype
	if (entityIter->second->mPrototype.IsValid())
	{
		EntityID prototypeID = entityIter->second->mPrototype.GetID();
		if (IsEntityPrototype(prototypeID))
		{
			// here we suppose the entity has the same types of components (from the beginning) as the prototype.
			// so if the user wants to remove the component linked to the prototype, we have to break the link
			if (componentToDestroy < GetNumberOfEntityComponents(prototypeID))
			{
				ocInfo << "Unlinking " << entity << " from prototype " << prototypeID << " because of component destruction";
				UnlinkEntityFromPrototype(entity.GetID());
			}
		}
	}

	mComponentMgr->DestroyComponent(entity.GetID(), componentToDestroy);
}

int32 EntitySystem::EntityMgr::GetNumberOfEntityComponents( const EntityHandle entity ) const
{
	OC_DASSERT(mComponentMgr);
	return mComponentMgr->GetNumberOfEntityComponents(entity.GetID());
}

void EntitySystem::EntityMgr::LinkEntityToPrototype( const EntityHandle entity, const EntityHandle prototype )
{
	PrototypeMap::iterator protIt = mPrototypes.find(prototype.GetID());
	if (protIt == mPrototypes.end())
	{
		ocError << "Cannot find prototype " << prototype << " to link";
		return;
	}

	if (protIt->second->mInstancesCount == 0) UpdatePrototypeCopy(prototype.GetID());
	protIt->second->mInstancesCount++;
	UpdatePrototypeInstance(prototype.GetID(), entity.GetID(), true);
}

void EntitySystem::EntityMgr::UnlinkEntityFromPrototype( const EntityHandle entity )
{
	EntityMap::iterator entityIt = mEntities.find(entity.GetID());
	if (entityIt == mEntities.end())
	{
		ocError << "Cannot unlink entity " << entity << " ; not found";
		return;
	}

	if (!entityIt->second->mPrototype.IsValid())
	{
		ocError << "Entity " << entity << " is not linked to any prototype";
		return;
	}

	EntityID parentPrototypeID = entityIt->second->mPrototype.GetID();
	PrototypeMap::iterator parentPrototypeIter = mPrototypes.find(parentPrototypeID);
	if (parentPrototypeIter == mPrototypes.end())
	{
		ocError << "Entity prototype not found: " << parentPrototypeID;
		return;
	}

	if (parentPrototypeIter->second != 0)
		parentPrototypeIter->second->mInstancesCount--;
}

bool EntitySystem::EntityMgr::GetEntityComponentProperties( const EntityHandle entity, const ComponentID component, PropertyList& out, const PropertyAccessFlags flagMask /*= PA_FULL_ACCESS*/ ) const
{
	out.clear();

	if (component >= mComponentMgr->GetNumberOfEntityComponents(entity.GetID()))
	{
		ocError << "Invalid component ID: " << component;
		return false;
	}

	Component* cmp = mComponentMgr->GetEntityComponent(entity.GetID(), component);
	cmp->GetRTTI()->EnumProperties(cmp, out, flagMask);

	return true;
}

bool EntitySystem::EntityMgr::HasEntityComponentProperty( const EntityHandle entity, const ComponentID componentID, const StringKey propertyKey, const PropertyAccessFlags flagMask /*= PA_FULL_ACCESS*/ ) const
{
	OC_DASSERT(mComponentMgr);

	if (componentID >= mComponentMgr->GetNumberOfEntityComponents(entity.GetID()))
	{
		ocError << "Invalid component ID: " << componentID;
		return false;
	}

	Component* cmp = mComponentMgr->GetEntityComponent(entity.GetID(), componentID);
	AbstractProperty* prop = cmp->GetRTTI()->GetProperty(propertyKey, flagMask);
	if (prop) return true;

	return false;
}