#include "Common.h"
#include "EntityMgr.h"
#include "EntityDescription.h"
#include "../ComponentMgr/ComponentMgr.h"
#include "../ComponentMgr/Component.h"
#include "../../ResourceSystem/XMLResource.h"
#include "../../GfxSystem/GfxSceneMgr.h"

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
	// test ulozeni entit
	ResourceSystem::XMLOutput storage("TestSave.xml");
	if (gEntityMgr.SaveEntitiesToStorage(storage))
	{
		ocInfo << "Test save successful.";
	}
	else
	{
		ocError << "Test save failed!";
	}
	
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
	for (EntityMap::iterator i = mEntities.begin(); i!=mEntities.end(); ++i)
	{
		PostMessage(i->first, msg);
	}
}

EntityHandle EntityMgr::CreateEntity(EntityDescription& desc)
{
	OC_ASSERT(mComponentMgr);

	bool isPrototype = desc.mKind == EntityDescription::EK_PROTOTYPE || desc.mKind == EntityDescription::EK_PROTOTYPE_COPY;

	if (desc.mComponents.size() == 0)
	{
		ocError << "Attempting to create an entity without components";
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
				ocInfo << "Component " << GetComponentTypeName(cmpType) << " depends on " << GetComponentTypeName(*depIt) << " which was not created yet";
			}
		}

		// take a note that this component type was already created
		createdComponentTypes.insert(cmpType);
	}

	// inits entity attributes
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

	ocTrace << "Entity created: " << entityHandle;

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
		if (protIt->second->mCopy != INVALID_ENTITY_ID)
		{
			if (erase) DestroyEntityImmediately(protIt->second->mCopy, erase);
			else DestroyEntity(protIt->second->mCopy); // if we can't erase the entity from the map, it has to be deleted in the next round
		}

		delete protIt->second;
		protIt->second = 0;

		if (erase) mPrototypes.erase(protIt);
	}

	EntityMap::iterator entityIt = mEntities.find(entityToDestroy);
	if (entityIt != mEntities.end())
	{
		// destroy components of the entity
		mComponentMgr->DestroyEntityComponents(entityToDestroy);

		// destroy the link between the entity and its prototype
		if (entityIt->second->mPrototype.IsValid())
		{
			UnlinkEntityFromPrototype(entityToDestroy);
		}

		delete entityIt->second;

		if (erase) mEntities.erase(entityIt);
	}

	ocTrace << "Entity destroyed " << entityToDestroy;
}

string EntitySystem::EntityMgr::GetEntityName(const EntitySystem::EntityHandle& h) const
{
	EntityMap::const_iterator ei = mEntities.find(h.GetID());
	if (ei == mEntities.end())
	{
		ocError << "Can't find entity " << h;
		return false;
	}
	return ei->second->mName;
}


void EntitySystem::EntityMgr::SetEntityName(const EntityHandle& h, const string& entityName)
{
	EntityMap::const_iterator ei = mEntities.find(h.GetID());
	if (ei == mEntities.end())
	{
		ocError << "Can't find entity " << h;
		return;
	}
	ei->second->mName = entityName;
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
		(*it)->EnumProperties(*it, out, flagMask);
	}

	return true;
}

PropertyHolder EntitySystem::EntityMgr::GetEntityProperty( const EntityHandle entity, const StringKey key, const PropertyAccessFlags flagMask /*= 0xff*/ ) const
{
	OC_DASSERT(mComponentMgr);
	if (!entity.IsValid())
	{
		ocError << "Invalid entity";
		return PropertyHolder();
	}
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(entity.GetID()); it.HasMore(); ++it)
	{
		AbstractProperty* prop = (*it)->GetPropertyPointer(key, flagMask);
		if (prop)
		{
			return PropertyHolder(*it, prop);
		}
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
	AbstractProperty* prop = cmp->GetPropertyPointer(propertyKey, flagMask);
	if (prop)
	{
		return PropertyHolder(cmp, prop);
	}


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
		AbstractProperty* prop = (*it)->GetPropertyPointer(key, flagMask);
		if (prop) return true;
	}
	return false;
}

bool EntitySystem::EntityMgr::RegisterDynamicPropertyOfEntityComponent(Reflection::ePropertyType propertyType, 
	const EntityHandle entity, const ComponentID component, const StringKey propertyKey,
	const Reflection::PropertyAccessFlags accessFlags, const string& comment)
{
	switch (propertyType)
	{
	// We generate cases for all property types and arrays of property types here.
	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) case typeID: \
		return RegisterDynamicPropertyOfEntityComponent<typeClass>(entity, component, propertyKey, accessFlags, comment);
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	#define PROPERTY_TYPE(typeID, typeClass, defaultValue, typeName, scriptSetter, cloning) case typeID##_ARRAY: \
		return RegisterDynamicPropertyOfEntityComponent<Array<typeClass>*>(entity, component, propertyKey, accessFlags, comment);
	#include "Utils/Properties/PropertyTypes.h"
	#undef PROPERTY_TYPE

	default:
		OC_NOT_REACHED();
		return false;
	}
}

bool EntitySystem::EntityMgr::UnregisterDynamicPropertyOfEntityComponent(const EntityHandle entity, const ComponentID component,
	const StringKey propertyKey)
{
	OC_DASSERT(mComponentMgr);
	if (component >= mComponentMgr->GetNumberOfEntityComponents(entity.GetID()))
	{
		ocError << "Invalid component ID: " << component;
		return false;
	}

	Component* cmp = mComponentMgr->GetEntityComponent(entity.GetID(), component);
	return cmp->UnregisterDynamicProperty(propertyKey);
}

EntitySystem::EntityHandle EntitySystem::EntityMgr::FindFirstEntity( const string& name )
{
	for(EntityMap::const_iterator i=mEntities.begin(); i!=mEntities.end(); ++i)
		if (i->second->mName.compare(name) == 0)
			return i->first;
	return EntityHandle::Null;
}

template<typename T>
void LoadEntityPropertyArrayFromXML( PropertyHolder prop, ResourceSystem::XMLResourcePtr xml, ResourceSystem::XMLNodeIterator& xmlPropertyIterator )
{
	vector<T> vertices;
	for (ResourceSystem::XMLNodeIterator vertIt=xml->IterateChildren(xmlPropertyIterator); vertIt!=xml->EndChildren(xmlPropertyIterator); ++vertIt)
	{
		if ((*vertIt).compare("Item") == 0) vertices.push_back(vertIt.GetChildValue<T>());
		else ocError << "XML:Entity: Expected 'Item', found '" << *vertIt << "'";
	}

	Array<T> vertArray(vertices.size());
	for (uint32 i=0; i<vertices.size(); ++i)
	{
		vertArray[i] = vertices[i];
	}
	prop.SetValue<Array<T>*>(&vertArray);
}

void EntitySystem::EntityMgr::LoadEntityPropertyFromXML(const EntityID entityID, const ComponentID componentID, 
	PrototypeInfo* prototypeInfo, ResourceSystem::XMLNodeIterator& xmlPropertyIterator)
{
	StringKey propertyKey(*xmlPropertyIterator);
	
	// test whether it is a dynamic property
	if (!HasEntityComponentProperty(entityID, componentID, propertyKey, PA_INIT))
	{
		if (xmlPropertyIterator.HasAttribute("Type"))
		{
			string propertyTypeName = xmlPropertyIterator.GetAttribute<string>("Type");
			Reflection::ePropertyType propertyType = Reflection::PropertyTypes::GetTypeFromName(propertyTypeName);
			if (propertyType == PT_UNKNOWN)
			{
				ocError << "XML:Entity: Unknown property type '" << propertyTypeName << "'.";
				return;
			}
			Reflection::PropertyAccessFlags accessFlags = Reflection::PA_FULL_ACCESS;
			if (xmlPropertyIterator.HasAttribute("Access"))
			{
				accessFlags = (uint8)xmlPropertyIterator.GetAttribute<uint16>("Access");
			}

			string comment = "";
			if (xmlPropertyIterator.HasAttribute("Comment"))
			{
				comment = xmlPropertyIterator.GetAttribute<string>("Comment");
			}

			if (!RegisterDynamicPropertyOfEntityComponent(propertyType, entityID, componentID,
				propertyKey, accessFlags, comment))
			{
				ocError << "XML:Entity: Cannot registry dynamic entity property '" << *xmlPropertyIterator << "'.";
				return;
			}
		}
		else
		{
			ocError << "XML:Entity: Unknown entity property '" << *xmlPropertyIterator << "' (it might not be marked as initable (PA_INIT))";
			return;
		}
	}
	
	// the property was stored in the XML file for the prototype, so it's a shared property
	if (prototypeInfo) prototypeInfo->mSharedProperties.insert(propertyKey);

	PropertyHolder prop = GetEntityComponentProperty(entityID, componentID, propertyKey);

	prop.ReadValueXML(xmlPropertyIterator);
}

void EntitySystem::EntityMgr::LoadEntityFromXML(ResourceSystem::XMLNodeIterator &entIt, const bool isPrototype)
{
	// init the entity description
	EntityDescription desc;
	desc.Reset();
	if (entIt.HasAttribute("Name"))	desc.SetName(entIt.GetAttribute<string>("Name"));
	if (entIt.HasAttribute("ID")) desc.SetDesiredID(entIt.GetAttribute<EntityID>("ID"));
	if (entIt.HasAttribute("Prototype")) desc.SetPrototype(entIt.GetAttribute<EntityID>("Prototype"));
	if (isPrototype) desc.SetKind(EntityDescription::EK_PROTOTYPE);

	// add component types
	for (ResourceSystem::XMLNodeIterator cmpIt = entIt.IterateChildren(); cmpIt != entIt.EndChildren(); ++cmpIt)
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
	for (ResourceSystem::XMLNodeIterator cmpIt = entIt.IterateChildren(); cmpIt != entIt.EndChildren(); ++cmpIt)
	{
		// skip unwanted data
		if ((*cmpIt).compare("Component") != 0)
			continue;

		++currentComponent;

		bool first = true;

		for (ResourceSystem::XMLNodeIterator xmlPropertyIter = cmpIt.IterateChildren(); 
			xmlPropertyIter != cmpIt.EndChildren(); ++xmlPropertyIter)
		{
			// skip unwanted data
			if (first && (*xmlPropertyIter).compare("Type") == 0)
			{
				first = false;
				continue;
			}
			LoadEntityPropertyFromXML(entity.GetID(), currentComponent, prototypeInfo, xmlPropertyIter);
		}
	}

	// finish init
	// note: the prototype copy can't be created here since all prototypes must be loaded first before any copy is created.
	//       otherwise it would screw up their IDs.
	entity.FinishInit();

	ocInfo << "Entity loaded from XML: " << entity;
}

bool EntitySystem::EntityMgr::LoadEntitiesFromResource(ResourceSystem::ResourcePtr res, const bool isPrototype)
{
	if (res == 0)
	{
		ocError << "XML: Can't load data; null resource pointer";
		return false;
	}
	ResourceSystem::XMLResourcePtr xml = res;

	bool result = true;

	for (ResourceSystem::XMLNodeIterator entIt = xml->IterateTopLevel(); entIt != xml->EndTopLevel(); ++entIt)
	{
		if ((*entIt).compare("Entity") == 0)
		{
			LoadEntityFromXML(entIt, isPrototype);
		}
		else
		{
			ocError << "XML: Expected 'Entity', found '" << *entIt << "'";
			result = false;
		}
	}

	return result;
}

bool EntitySystem::EntityMgr::SaveEntityToStorage(const EntitySystem::EntityID entityID, ResourceSystem::XMLOutput &storage, const bool isPrototype)
{
	EntityInfo* info = mEntities[entityID];
	if (!info) { return false; }
	PrototypeInfo* protInfo = 0;
	if (isPrototype)
	{
		protInfo = mPrototypes[entityID];
		OC_ASSERT(protInfo);
	}
	
	// write header of entity tag
	storage.BeginElementStart("Entity");
	storage.AddAttribute("Name", info->mName);
	storage.AddAttribute("ID", Utils::StringConverter::ToString(entityID));
	if (!isPrototype && info->mPrototype.IsValid())
	{ 
		storage.AddAttribute("Prototype", Utils::StringConverter::ToString(info->mPrototype.GetID()));
	}
	storage.BeginElementFinish();

	// components saving
	int32 compID = 0;
	for (EntityComponentsIterator iter = mComponentMgr->GetEntityComponents(entityID); iter.HasMore(); ++iter)
	{
		Component* comp = (*iter);
		storage.BeginElementStart("Component");
		storage.AddAttribute("Type", GetComponentTypeName(comp->GetType()));
		storage.BeginElementFinish();

		PropertyList propertyList;
		comp->EnumProperties(comp, propertyList);
		for (PropertyList::iterator it = propertyList.begin(); it != propertyList.end(); ++it)
		{
			if (((it->GetAccessFlags() & Reflection::PA_TRANSIENT) == Reflection::PA_TRANSIENT)
				|| (isPrototype && (protInfo->mSharedProperties.find(it->GetKey())
				== protInfo->mSharedProperties.end()))) { continue; }
			string propName = it->GetKey().ToString();
			storage.BeginElementStart(propName);
			if (it->IsValued()) // property is dynamic, so we must save a type
			{ 
				storage.AddAttribute("Type", string(Reflection::PropertyTypes::GetStringName(it->GetType())));
				if (it->GetAccessFlags() != Reflection::PA_FULL_ACCESS)
					{ storage.AddAttribute("Access", Utils::StringConverter::ToString<uint16>(it->GetAccessFlags())); }
				if (it->GetComment() != "")
					{ storage.AddAttribute("Comment", it->GetComment()); }
			}
			storage.BeginElementFinish();

			// write property value
			it->WriteValueXML(storage);

			storage.EndElement();
		}

		storage.EndElement();
		++compID;
	}
	
	storage.EndElement();

	return true;
}

bool EntitySystem::EntityMgr::SaveEntitiesToStorage(ResourceSystem::XMLOutput& storage, const bool isPrototype)
{
	storage.BeginElement("Entities");
	
	bool res = true;
	for (EntityMap::const_iterator i = mEntities.begin(); i != mEntities.end(); ++i)
	{
		if ((isPrototype && mPrototypes.find(i->first) != mPrototypes.end())
			|| (!isPrototype && mPrototypes.find(i->first) == mPrototypes.end()))
		{
			if (!SaveEntityToStorage(i->first, storage, isPrototype)) { res = false; };
		}
		else { res = false; }
	}
	storage.EndElement();
	
	return res;
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

eComponentType EntitySystem::EntityMgr::GetEntityComponentType(const EntityHandle entity, const ComponentID componentID)
{
	Component* component = mComponentMgr->GetEntityComponent(entity.GetID(), componentID);
	return component ? component->GetType() : CT_INVALID;
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

ComponentID EntitySystem::EntityMgr::GetEntityComponent(const EntityHandle entity, const eComponentType type)
{
	int32 i = 0;
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(entity.GetID()); it.HasMore(); ++it, ++i)
	{
		if ((*it)->GetType() == type) return i;
	}
	return -1;
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
	cmp->EnumProperties(cmp, out, flagMask);

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
	AbstractProperty* prop = cmp->GetPropertyPointer(propertyKey, flagMask);
	if (prop) return true;

	return false;
}

Component* EntitySystem::EntityMgr::GetEntityComponentPtr( const EntityHandle entity, const ComponentID id )
{
	OC_DASSERT(mComponentMgr);
	
	return mComponentMgr->GetEntityComponent(entity.GetID(), id);
}

Component* EntitySystem::EntityMgr::GetEntityComponentPtr( const EntityHandle entity, const eComponentType type )
{
	OC_DASSERT(mComponentMgr);

	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(entity.GetID()); it.HasMore(); ++it)
	{
		if ((*it)->GetType() == type) return *it;
	}
	return 0;
}

void EntitySystem::EntityMgr::GetEntitiesWithComponent(EntityList& out, const eComponentType componentType)
{
	out.clear();

	for (EntityMap::const_iterator i = mEntities.begin(); i != mEntities.end(); ++i)
	{
		if (HasEntityComponentOfType(i->first, componentType))
		{
			out.push_back(EntityHandle(i->first));
		}
	}
}

		
void EntitySystem::EntityMgr::GetEntities(EntityList& out)
{
	out.clear();

	for (EntityMap::const_iterator i = mEntities.begin(); i != mEntities.end(); ++i)
	{
		out.push_back(EntityHandle(i->first));
	}
}

EntitySystem::EntityHandle EntitySystem::EntityMgr::ExportEntityToPrototype( const EntityHandle entity )
{
	OC_DASSERT(mComponentMgr);

	// create the prototype entity in the system
	EntityDescription desc;
	desc.SetKind(EntityDescription::EK_PROTOTYPE);
	desc.SetName(GetEntityName(entity));
	ComponentTypeList componentTypes;
	GetEntityComponentTypes(entity, componentTypes);
	for (ComponentTypeList::iterator it=componentTypes.begin(); it!=componentTypes.end(); ++it)
	{
		desc.AddComponent(*it);
	}
	EntityHandle prototype = CreateEntity(desc);

	// copy properties
	OC_ASSERT(mComponentMgr->GetNumberOfEntityComponents(entity.GetID()) == mComponentMgr->GetNumberOfEntityComponents(prototype.GetID()));
	EntityComponentsIterator cmpItOld = mComponentMgr->GetEntityComponents(entity.GetID());
	EntityComponentsIterator cmpItNew = mComponentMgr->GetEntityComponents(prototype.GetID());

	for (; cmpItOld.HasMore(); ++cmpItOld, ++cmpItNew)
	{
		PropertyList propsOld;
		PropertyList propsNew;
		(*cmpItOld)->EnumProperties(*cmpItOld, propsOld, PA_FULL_ACCESS);
		(*cmpItNew)->EnumProperties(*cmpItNew, propsNew, PA_FULL_ACCESS);

		// duplicate dynamic properties
		for (PropertyList::iterator itOld=propsOld.begin(); itOld!=propsOld.end(); ++itOld)
		{
			PropertyList::iterator it = find(propsNew.begin(), propsNew.end(), *itOld);
			if (it == propsNew.end())
			{
				propsNew.insert(propsNew.end(), *itOld);
			}
		}

		// copy data
		OC_ASSERT(propsOld.size()==propsNew.size());
		PropertyList::iterator propItOld = propsOld.begin();
		PropertyList::iterator propItNew = propsNew.begin();
		for (; propItOld != propsOld.end(); ++propItOld, ++propItNew)
		{
			propItNew->CopyFrom(*propItOld);
		}
	}


	ocInfo << "Created prototype " << prototype << " from " << entity;

	return prototype;
}
