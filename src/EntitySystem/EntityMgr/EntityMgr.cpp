#include "Common.h"
#include "EntityMgr.h"
#include "EntityDescription.h"
#include "../ComponentMgr/ComponentMgr.h"
#include "../ComponentMgr/Component.h"
#include "ResourceSystem/XMLResource.h"
#include "GfxSystem/GfxSceneMgr.h"
#include "Editor/EditorMgr.h"
#include "Editor/EditorGUI.h"
#include "Editor/HierarchyWindow.h"

namespace EntitySystem
{
	/// This struct holds info about an instance of an entity in the system.
	struct EntityInfo
	{
		EntityInfo(const string& name, const EntityHandle prototype, const bool transient):
			mFullyInited(false),
			mName(name),
			mPrototype(prototype),
			mTransient(transient)
		{

		}

		/// True if this entity was fully inited in the system by the user.
		bool mFullyInited;
		/// Human readable user ID.
		string mName;
		/// Handle to the prototype entity (can be invalid).
		EntityHandle mPrototype;
		/// Whether the entity should not be saved.
		bool mTransient;

	private:
		EntityInfo(const EntityInfo& rhs);
		EntityInfo& operator=(const EntityInfo& rhs);
	};

	/// This struct holds info specific for prototype entities.
	struct PrototypeInfo
	{
		PrototypeInfo(void): mInstancesCount(0) {}

		/// Number of instances of this prototype.
		int32 mInstancesCount;
		/// Set of properties marked as shared with instances of the prototype.
		set<StringKey> mSharedProperties;

	private:
		PrototypeInfo(const PrototypeInfo& rhs);
		PrototypeInfo& operator=(const PrototypeInfo& rhs);
	};
}


/// File used for storing prototypes.
const char* PROTOTYPES_DEFAULT_FILE = "Prototypes.xml";


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
		// the entity was not initialized by calling FinishInit() or something similar
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

	bool isPrototype = desc.mKind == EntityDescription::EK_PROTOTYPE;

	if (desc.mComponents.size() == 0 && desc.mKind != EntityDescription::EK_PROTOTYPE)
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

	mComponentMgr->PrepareForEntity(entityHandle.GetID());

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
	mEntities[entityHandle.GetID()] = new EntityInfo(desc.mName, EntityHandle(desc.mPrototype), desc.mTransient);
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

	if (GlobalProperties::Get<bool>("DevelopMode"))
	{
		gEditorMgr.GetEditorGui()->GetHierarchyWindow()->AddEntityToHierarchy(entityHandle);
	}

	return entityHandle;
}

EntityHandle EntityMgr::DuplicateEntity(const EntityHandle oldEntity, const string& newName)
{
	OC_ASSERT(mComponentMgr);

	if (!oldEntity.IsValid()) { return EntityHandle::Null; }

	// create the handle for the new entity
	EntityHandle newEntity;
	if (IsEntityPrototype(oldEntity)) newEntity = EntityHandle::CreateUniquePrototypeHandle();
	else newEntity = EntityHandle::CreateUniqueHandle();

	// let the component manager know about the entity
	mComponentMgr->PrepareForEntity(newEntity.GetID());

	// copy all components
	ComponentTypeList cmpList;
	if (!GetEntityComponentTypes(oldEntity, cmpList)) { return EntityHandle::Null; }
	for (ComponentTypeList::const_iterator cmpIt = cmpList.begin(); cmpIt != cmpList.end(); ++cmpIt)
	{
		mComponentMgr->CreateComponent(newEntity.GetID(), *cmpIt);
	}

	// inits entity attributes
	mEntities[newEntity.GetID()] = new EntityInfo(newName.empty() ? mEntities[oldEntity.GetID()]->mName
		: newName, mEntities[oldEntity.GetID()]->mPrototype, mEntities[oldEntity.GetID()]->mTransient);
	if (IsEntityPrototype(oldEntity)) { mPrototypes[newEntity.GetID()] = new PrototypeInfo(); }

	// copy all property values
	PropertyList oldPropertyList, newPropertyList;
	if (!GetEntityProperties(oldEntity, oldPropertyList) || !GetEntityProperties(newEntity, newPropertyList))
	{ 
		DestroyEntity(newEntity);
		return EntityHandle::Null;
	}
	for (PropertyList::iterator oldIt = oldPropertyList.begin(), newIt = newPropertyList.begin();
		oldIt != oldPropertyList.end(); ++oldIt, ++newIt)
	{
		newIt->CopyFrom(*oldIt);
	}

	// link the entity to its prototype
	if (mEntities[newEntity.GetID()]->mPrototype.IsValid() && 
		mPrototypes.find(mEntities[newEntity.GetID()]->mPrototype.GetID()) != mPrototypes.end())
	{
		LinkEntityToPrototype(newEntity.GetID(), mEntities[newEntity.GetID()]->mPrototype);
	}

	ocTrace << "Entity duplicated: " << newEntity;

	if (GlobalProperties::Get<bool>("DevelopMode"))
	{
		gEditorMgr.GetEditorGui()->GetHierarchyWindow()->AddEntityToHierarchy(EntityHandle::Null, newEntity);
	}

	return newEntity;
}


void EntityMgr::DestroyEntity(const EntityHandle entityToDestroy)
{
	if (GlobalProperties::Get<bool>("DevelopMode"))
	{
		gEditorMgr.GetEditorGui()->GetHierarchyWindow()->RemoveEntityFromHierarchy(entityToDestroy);
	}
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
		delete protIt->second;
		protIt->second = 0;

		if (erase) mPrototypes.erase(protIt);
	}

	EntityMap::iterator entityIt = mEntities.find(entityToDestroy);
	if (entityIt == mEntities.end())
	{
		ocError << "Cannot destroy entity " << entityToDestroy << "; not found";
		return;
	}

	// destroy components of the entity
	mComponentMgr->DestroyEntityComponents(entityToDestroy);

	// destroy the link between the entity and its prototype
	if (entityIt->second->mPrototype.IsValid())
	{
		UnlinkEntityFromPrototype(entityToDestroy);
	}

	delete entityIt->second;

	if (erase) mEntities.erase(entityIt);

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

bool EntitySystem::EntityMgr::IsEntityTransient(const EntityHandle& h) const
{
  EntityMap::const_iterator ei = mEntities.find(h.GetID());
  if (ei == mEntities.end())
	{
		ocError << "Can't find entity " << h;
		return false;
	}
	return ei->second->mTransient;
}
		
void EntitySystem::EntityMgr::SetEntityTransient(const EntityHandle& h, const bool transient)
{
  EntityMap::const_iterator ei = mEntities.find(h.GetID());
  if (ei == mEntities.end())
	{
		ocError << "Can't find entity " << h;
		return;
	}
	ei->second->mTransient = transient;
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
	if (entIt.HasAttribute("Transient")) desc.SetTransient(entIt.GetAttribute<bool>("Transient"));
	if (isPrototype) desc.SetKind(EntityDescription::EK_PROTOTYPE);

	// add component types
	for (ResourceSystem::XMLNodeIterator cmpIt = entIt.IterateChildren(); cmpIt != entIt.EndChildren(); ++cmpIt)
	{
		if ((*cmpIt).compare("Component") == 0)	desc.AddComponent(DetectComponentType(cmpIt.GetAttribute<string>("Type")));
	}

	// create the entity
	EntityHandle entity = CreateEntity(desc);
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
	if (!res)
	{
		ocError << "XML: Can't load data; null resource pointer";
		return false;
	}
	ResourceSystem::XMLResourcePtr xml = res;

	bool result = true;

	ResourceSystem::XMLNodeIterator toplevelIter = xml->IterateTopLevel();
	
	if (toplevelIter == xml->EndTopLevel() || (*toplevelIter).compare("Entities") != 0)
	{
		ocError << "XML: Expected 'Entities'";
		return false;
	}

	for (ResourceSystem::XMLNodeIterator entIt = toplevelIter.IterateChildren(); entIt != toplevelIter.EndChildren(); ++entIt)
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

	if (!isPrototype && GlobalProperties::Get<bool>("DevelopMode"))
	{
		++toplevelIter;
		if (toplevelIter == xml->EndTopLevel() || (*toplevelIter).compare("Hierarchy") != 0)
		{
			ocError << "XML: Expected 'Hierarchy' after 'Entities'";
			return false;
		}
		gEditorMgr.LoadHierarchyWindow(toplevelIter);
	}

	return result;
}

bool EntitySystem::EntityMgr::SaveEntityToStorage(const EntitySystem::EntityID entityID, 
	ResourceSystem::XMLOutput &storage, const bool isPrototype, const bool evenTransient) const
{
	const EntityInfo* info = mEntities.at(entityID);
	if (!info) { return false; }
	const PrototypeInfo* protInfo = 0;
	if (isPrototype)
	{
		protInfo = mPrototypes.at(entityID);
		OC_ASSERT(protInfo);
	}
	
	if (!evenTransient && info->mTransient) { return true; }
	
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
	for (EntityComponentsIterator iter = mComponentMgr->GetEntityComponents(entityID); iter.HasMore(); ++iter)
	{
		Component* comp = (*iter);
		storage.BeginElementStart("Component");
		storage.AddAttribute("Type", GetComponentTypeName(comp->GetType()));
		storage.BeginElementFinish();

		PropertyList propertyList;
		comp->EnumProperties(comp, propertyList);
		if (!comp->IsTransient()) for (PropertyList::iterator it = propertyList.begin(); it != propertyList.end(); ++it)
		{
			bool protPropShared = protInfo && protInfo->mSharedProperties.find(it->GetKey()) != protInfo->mSharedProperties.end();
			if ((it->GetAccessFlags() & Reflection::PA_TRANSIENT) != 0 || (isPrototype && !protPropShared)) { continue; }
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
	}
	
	storage.EndElement();

	return true;
}

bool EntitySystem::EntityMgr::SaveEntitiesToStorage(ResourceSystem::XMLOutput& storage, const bool isPrototype,
	const bool evenTransient) const
{
	storage.BeginElement("Entities");
	
	bool result = true;
	for (EntityMap::const_iterator i = mEntities.begin(); i != mEntities.end(); ++i)
	{
		if ((isPrototype && mPrototypes.find(i->first) != mPrototypes.end())
			|| (!isPrototype && mPrototypes.find(i->first) == mPrototypes.end()))
		{
			if (!SaveEntityToStorage(i->first, storage, isPrototype, evenTransient)) { result = false; };
		}
	}

	storage.EndElement();


	storage.BeginElement("Hierarchy");

	gEditorMgr.SaveHierarchyWindow(storage);

	storage.EndElement();


	return result;
}

bool EntitySystem::EntityMgr::EntityExists( const EntityHandle h ) const
{
	if (!h.IsValid())
		return false;
	return mEntities.find(h.GetID()) != mEntities.end();
}

EntityHandle EntitySystem::EntityMgr::GetEntity(EntityID id) const
{
	if (mEntities.find(id) != mEntities.end())
		return EntityHandle(id);
	else
		return EntityHandle::Null;
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

bool EntitySystem::EntityMgr::GetEntityComponentsOfType( const EntityHandle entity, const eComponentType desiredType, ComponentIdList& out )
{
	ComponentID id = 0;
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(entity.GetID()); it.HasMore(); ++it, ++id)
	{
		if ((*it)->GetType() == desiredType) out.push_back(id);
	}

	return true;
}

void EntitySystem::EntityMgr::UpdatePrototypeInstances( const EntityHandle prototype )
{
	for (EntityMap::iterator it=mEntities.begin(); it!=mEntities.end(); ++it)
	{
		if (it->second->mPrototype == prototype)
		{
			UpdatePrototypeInstance(prototype.GetID(), it->first);
		}
	}
}

void EntitySystem::EntityMgr::UpdatePrototypeInstance( const EntityID prototype, const EntityID instance )
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
			PropertyHolder prototypeProperty = *protPropIter;
			instanceProperty.CopyFrom(prototypeProperty);
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
	ComponentID cid = mComponentMgr->CreateComponent(entity.GetID(), componentType);

	// if this is a prototype mark its properties as shared in the beginning
	if (IsEntityPrototype(entity))
	{
		PropertyList propList;
		GetEntityComponentProperties(entity, cid, propList);
		for (PropertyList::iterator it=propList.begin(); it!=propList.end(); ++it)
		{
			if ((it->GetAccessFlags() & Reflection::PA_INIT) != 0)
			{
				SetPrototypePropertyShared(entity, it->GetName());
			}
		}
	}

	return cid;
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
		ocError << "Property " << propertyToMark << " must be initable (PA_INIT) to be marked as shared";
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
	OC_DASSERT(mComponentMgr);

	if (!CanDestroyEntityComponent(entity, componentToDestroy))
	{
		ocError << "Cannot destroy component " << componentToDestroy << " of " << entity;
		return;
	}

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

	// if this was a prototype mark the properties as not shared
	if (IsEntityPrototype(entity))
	{
		PropertyList propList;
		GetEntityComponentProperties(entity, componentToDestroy, propList);
		for (PropertyList::iterator it=propList.begin(); it!=propList.end(); ++it)
		{
			SetPrototypePropertyNonShared(entity, it->GetName());
		}
	}

	eComponentType cmpType = mComponentMgr->GetEntityComponent(entity.GetID(), componentToDestroy)->GetType();

	mComponentMgr->DestroyComponent(entity.GetID(), componentToDestroy);

	PostMessage(entity, EntityMessage(EntityMessage::COMPONENT_DESTROYED, Reflection::PropertyFunctionParameters() << (uint32)cmpType));

	ocInfo << "Destroyed component " << componentToDestroy << " of " << entity;
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

	protIt->second->mInstancesCount++;
	UpdatePrototypeInstance(prototype.GetID(), entity.GetID());
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

void EntitySystem::EntityMgr::GetEntitiesWithComponent(EntityList& out, const eComponentType componentType, bool prototypes)
{
	out.clear();

	for (EntityMap::const_iterator i = mEntities.begin(); i != mEntities.end(); ++i)
	{
		if (HasEntityComponentOfType(i->first, componentType)
			&& (prototypes == IsEntityPrototype(EntityHandle(i->first))))
		{
			out.push_back(EntityHandle(i->first));
		}
	}
}

		
void EntitySystem::EntityMgr::GetEntities(EntityList& out, bool prototypes)
{
	out.clear();

	for (EntityMap::const_iterator i = mEntities.begin(); i != mEntities.end(); ++i)
	{
		if (prototypes == IsEntityPrototype(EntityHandle(i->first)))
		{
			out.push_back(EntityHandle(i->first));
		}
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

bool EntitySystem::EntityMgr::CanDestroyEntityComponent( const EntityHandle entity, const ComponentID componentToDestroy )
{
	OC_DASSERT(mComponentMgr);
	EntityMap::iterator entityIter = mEntities.find(entity.GetID());
	if (entityIter == mEntities.end())
	{
		ocError << "Entity " << entity << " not found";
		return false;
	}

	Component* cmp = mComponentMgr->GetEntityComponent(entity.GetID(), componentToDestroy);
	if (!cmp) return false;

	eComponentType typeToDestroy = cmp->GetType();
	EntityComponentsIterator cmpIter = mComponentMgr->GetEntityComponents(entity.GetID());
	while (*cmpIter != cmp) ++cmpIter;

	for (++cmpIter; cmpIter.HasMore(); ++cmpIter)
	{
		ComponentDependencyList depList;
		(*cmpIter)->GetRTTI()->EnumComponentDependencies(depList);
		if (find(depList.begin(), depList.end(), typeToDestroy) != depList.end())
		{
			// dependency failure
			return false;
		}
	}

	return true;
}

bool EntitySystem::EntityMgr::IsEntityLinkedToPrototype( const EntityHandle entity ) const
{
	OC_DASSERT(mComponentMgr);
	EntityMap::const_iterator entityIter = mEntities.find(entity.GetID());
	if (entityIter == mEntities.end())
	{
		ocError << "Entity " << entity << " not found";
		return false;
	}

	if (entityIter->second->mPrototype.IsValid())
	{
		EntityID prototypeID = entityIter->second->mPrototype.GetID();
		return IsEntityPrototype(prototypeID);
	}

	return false;
}

void EntitySystem::EntityMgr::GetPrototypes( EntityList& out )
{
	out.clear();

	for (PrototypeMap::const_iterator i = mPrototypes.begin(); i != mPrototypes.end(); ++i)
	{
		out.push_back(EntityHandle(i->first));
	}
}

bool EntitySystem::EntityMgr::LoadPrototypes()
{
	if (gResourceMgr.AddResourceFileToGroup(PROTOTYPES_DEFAULT_FILE, "Temp", ResourceSystem::RESTYPE_XMLRESOURCE, ResourceSystem::BPT_PROJECT, PROTOTYPES_DEFAULT_FILE))
	{
		ResourceSystem::ResourcePtr res = gResourceMgr.GetResource("Temp", PROTOTYPES_DEFAULT_FILE);
		res->Load();
		LoadEntitiesFromResource(res, true);
		gResourceMgr.DeleteGroup("Temp");
		ocInfo << "Prototypes loaded from " << PROTOTYPES_DEFAULT_FILE;
		return true;
	}
	else 
	{
		ocError << "Prototypes can't be loaded!";
		return false;
	}
}

bool EntitySystem::EntityMgr::SavePrototypes()
{
	ResourceSystem::XMLOutput storage(gResourceMgr.GetBasePath(ResourceSystem::BPT_PROJECT) + PROTOTYPES_DEFAULT_FILE);
	storage.BeginElement("Prototypes");
	bool result = SaveEntitiesToStorage(storage, true, false);
	storage.EndElement();
	result = result && storage.CloseAndReport();
	if (result) { ocInfo << "Prototypes saved into " << PROTOTYPES_DEFAULT_FILE; }
	else { ocError << "Prototypes can't be saved!"; }
	return result;
}

EntitySystem::EntityHandle EntitySystem::EntityMgr::GetEntityPrototype( const EntityHandle entity )
{
	EntityMap::iterator entityInfoIter = mEntities.find(entity.GetID());
	if (entityInfoIter == mEntities.end())
	{
		ocError << "Entity not found";
		return EntityHandle::Null;
	}
	EntityInfo* entityInfo = entityInfoIter->second;
	return entityInfo->mPrototype;
}

size_t EntitySystem::EntityMgr::GetNumberOfNonTransientEntities() const
{
	size_t result = 0;
	for (EntityMap::const_iterator i = mEntities.begin(); i != mEntities.end(); ++i)
	{
		if (!i->second->mTransient && !IsEntityPrototype(EntityHandle(i->first)))
		{
			++result;
		}
	}

	return result;
}