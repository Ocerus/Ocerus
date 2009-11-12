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
		EntityInfo(const eEntityType type, const string& name, const EntityHandle prototype):
			mType(type), 
			mFullyInited(false), 
			mName(name), 
			mPrototype(prototype)
		{

		}

		/// User type of the entity.
		eEntityType mType;
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

	// this assumes EntityMgr is a singleton
	EntityPicker::SetupPriorities();
}

EntityMgr::~EntityMgr()
{
	DestroyAllEntities();
	delete mComponentMgr;

	EntityPicker::CleanPriorities();
}

EntityMessage::eResult EntityMgr::PostMessage(EntityID id, const EntityMessage& msg)
{
	EntityMap::iterator ei = mEntities.find(id);
	if (ei == mEntities.end())
	{
		ocError << "Can't find entity with ID '" << id << "'";
		return EntityMessage::RESULT_ERROR;
	}
	if (EntityHandle::IsPrototypeID(id))
	{
		// the prototype nor its copy can't receive any messages
		return EntityMessage::RESULT_IGNORED;
	}
	if (msg.type != EntityMessage::INIT && msg.type != EntityMessage::POST_INIT && !ei->second->mFullyInited)
	{
		ocError << "Entity with ID '" << id << "' is not initialized -> can't post messages";
		return EntityMessage::RESULT_ERROR;
	}
	if (msg.type != EntityMessage::INIT && msg.type == EntityMessage::POST_INIT && ei->second->mFullyInited)
	{
		ocError << "Entity with ID '" << id << "' is already initialized";
		return EntityMessage::RESULT_ERROR;
	}
	if (msg.type == EntityMessage::POST_INIT)
	{
		ei->second->mFullyInited = true;
	}

	EntityMessage::eResult result = EntityMessage::RESULT_IGNORED;
	for (EntityComponentsIterator iter = mComponentMgr->GetEntityComponents(id); iter.HasMore(); ++iter)
	{
		EntityMessage::eResult r = (*iter)->HandleMessage(msg);
		if ((r == EntityMessage::RESULT_ERROR)
			|| (r == EntityMessage::RESULT_OK && result == EntityMessage::RESULT_IGNORED))
			result = r;
	}
	return result;
}

void EntityMgr::BroadcastMessage(const EntityMessage& msg)
{
	for (EntityMap::const_iterator i = mEntities.begin(); i!=mEntities.end(); ++i)
		PostMessage(i->first, msg);
}

EntityHandle EntityMgr::CreateEntity(EntityDescription& desc, PropertyList& out)
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
	EntityHandle h;
	if (desc.mID != INVALID_ENTITY_ID)
	{
		h = EntityHandle::CreateHandleFromID(desc.mID);
	}
	else
	{
		if (isPrototype) h = EntityHandle::CreateUniquePrototypeHandle();
		else h = EntityHandle::CreateUniqueHandle();
	}

	bool dependencyFailure = false;
	set<eComponentType> createdComponentTypes;
	for (ComponentDependencyList::const_iterator cmpDescIt=desc.mComponents.begin(); cmpDescIt!=desc.mComponents.end(); ++cmpDescIt)
	{
		eComponentType cmpType = *cmpDescIt;

		// create the component
		ComponentID cmpID = mComponentMgr->CreateComponent(h, cmpType);
		Component* cmp = mComponentMgr->GetEntityComponent(h, cmpID);
		OC_ASSERT(cmp);

		// check dependencies of the component
		ComponentDependencyList depList;
		cmp->GetRTTI()->EnumComponentDependencies(depList);
		for (ComponentDependencyList::const_iterator depIt=depList.begin(); depIt!=depList.end(); ++depIt)
			if (createdComponentTypes.find(*depIt) == createdComponentTypes.end())
				dependencyFailure = true;

		// take a note that this component type was already created
		createdComponentTypes.insert(cmpType);
	}

	// inits etity attributes
	mEntities[h.GetID()] = new EntityInfo(desc.mType, desc.mName, EntityHandle(desc.mPrototype));
	if (desc.mKind == EntityDescription::EK_PROTOTYPE) mPrototypes[h.GetID()] = new PrototypeInfo();

	// link the entity to its prototype
	if (mPrototypes.find(desc.mPrototype) != mPrototypes.end())
	{
		LinkEntityToPrototype(h.GetID(), desc.mPrototype);
	}

	// security checks
	if (dependencyFailure)
	{
		ocError << "Component dependency failure on entity '" << h << "' of type '" << desc.mType << "'";
		DestroyEntity(h);
		return h; // do like nothing's happened, but don't enum properties or they will access invalid memory
	}
	if (!GetEntityProperties(h.GetID(), out, PA_INIT))
	{
		ocError << "Can't get properties for created entity of type" << desc.mType;
	}

	return h;
}


void EntityMgr::DestroyEntity(const EntityHandle h)
{
	mEntityDestroyQueue.push_back(h.GetID());
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

void EntityMgr::DestroyEntityImmediately( const EntityID id, const bool erase )
{
	PrototypeMap::iterator protIt = mPrototypes.find(id);
	if (protIt != mPrototypes.end())
	{
		if (erase) DestroyEntityImmediately(protIt->second->mCopy, erase);
		else DestroyEntity(protIt->second->mCopy); // if we can't erase the entity from the map, it has to be deleted in the next round

		delete protIt->second;
		protIt->second = 0;

		if (erase) mPrototypes.erase(protIt);
	}

	EntityMap::iterator entityIt = mEntities.find(id);
	if (entityIt != mEntities.end())
	{
		mComponentMgr->DestroyEntityComponents(id);

		if (entityIt->second->mPrototype.IsValid())
		{
			UnlinkEntityFromPrototype(id);
		}

		delete entityIt->second;
		entityIt->second = 0;

		if (erase) mEntities.erase(entityIt);
	}
}

EntitySystem::eEntityType EntitySystem::EntityMgr::GetEntityType( const EntityHandle h ) const
{
	EntityMap::const_iterator ei = mEntities.find(h.GetID());
	if (ei == mEntities.end())
	{
		ocError << "Can't find entity " << h;
		return ET_UNKNOWN;
	}
	return ei->second->mType;
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

bool EntitySystem::EntityMgr::GetEntityProperties( const EntityID entityID, PropertyList& out, const PropertyAccessFlags flagMask ) const
{
	out.clear();

	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(entityID); it.HasMore(); ++it)
	{
		(*it)->GetRTTI()->EnumProperties(*it, out, flagMask);
	}

	return true;
}

PropertyHolder EntitySystem::EntityMgr::GetEntityProperty( const EntityID id, const StringKey key, const PropertyAccessFlags flagMask /*= 0xff*/ ) const
{
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(id); it.HasMore(); ++it)
	{
		AbstractProperty* prop = (*it)->GetRTTI()->GetProperty(key, flagMask);
		if (prop) return PropertyHolder(*it, prop);
	}


	// property not found, print some info about why
	ocError << "EntityMgr: unknown property '" << key << "'";
	PropertyList propertyList;
	string propertiesString;
	GetEntityProperties(id, propertyList, flagMask);
	for (PropertyList::iterator it=propertyList.begin(); it!=propertyList.end(); )
	{
		propertiesString += it->second.GetName();
		++it;
		if (it==propertyList.end()) propertiesString += ".";
		else propertiesString += ", ";
	}
	ocError << "Available properties: " << propertiesString;

	// return an invalid holder
	return PropertyHolder();
}

EntitySystem::EntityHandle EntitySystem::EntityMgr::FindFirstEntity( const string& name )
{
	for(EntityMap::const_iterator i=mEntities.begin(); i!=mEntities.end(); ++i)
		if (i->second->mName.compare(name) == 0)
			return i->first;
	return EntityHandle::Null;
}

void EntitySystem::EntityMgr::LoadEntityPropertyFromXML( PrototypeInfo* prototypeInfo, PropertyList::iterator propertyIter, ResourceSystem::XMLResourcePtr xml, ResourceSystem::XMLNodeIterator& xmlPropertyIterator, PropertyList& properties )
{
	// the property was stored in the XML file for the prototype, so it's a shared property
	if (prototypeInfo) prototypeInfo->mSharedProperties.insert(propertyIter->first);

	PropertyHolder prop = propertyIter->second;

	if (prop.GetType() == PT_VECTOR2_ARRAY)
	{
		string lengthParam;
		vector<Vector2> vertices;
		for (ResourceSystem::XMLNodeIterator vertIt=xml->IterateChildren(xmlPropertyIterator); vertIt!=xml->EndChildren(xmlPropertyIterator); ++vertIt)
		{
			if ((*vertIt).compare("Vertex") == 0)
				vertices.push_back(vertIt.GetChildValue<Vector2>());
			else if ((*vertIt).compare("LengthParam") == 0)
				lengthParam = vertIt.GetChildValue<string>();
			else
				ocError << "XML:Entity: Expected 'Vertex' or 'LengthParam', found '" << *vertIt << "'";
		}
		if (lengthParam.length() == 0)
		{
			ocError << "XML:Entity: LengthParam of an array not specified";
		}
		else if (properties.find(lengthParam) == properties.end())
		{
			ocError << "XML:Entity: LengthParam of name '" << lengthParam << "' not found in entity";
		}
		else
		{
			// the property was stored in the XML file for the prototype, so it's a shared property
			if (prototypeInfo) prototypeInfo->mSharedProperties.insert(lengthParam);

			properties[lengthParam].SetValue<uint32>(vertices.size());
			Vector2* vertArray = new Vector2[vertices.size()];
			for (uint32 i=0; i<vertices.size(); ++i)
				vertArray[i] = vertices[i];
			prop.SetValue<Vector2*>(vertArray);
		}
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
	eEntityType type = DetectEntityType(entIt.GetAttribute<string>("Type"));
	desc.Init(type);
	if (entIt.HasAttribute("Name"))	desc.SetName(entIt.GetAttribute<string>("Name"));
	if (entIt.HasAttribute("ID")) desc.SetDesiredID(entIt.GetAttribute<EntityID>("ID"));
	if (entIt.HasAttribute("Prototype")) desc.SetPrototype(entIt.GetAttribute<EntityID>("Prototype"));
	if (isPrototype) desc.SetKind(EntityDescription::EK_PROTOTYPE);

	// add component types
	for (ResourceSystem::XMLNodeIterator cmpIt=xml->IterateChildren(entIt); cmpIt!=xml->EndChildren(entIt); ++cmpIt)
		if ((*cmpIt).compare("Component") == 0)
			desc.AddComponent(DetectComponentType(cmpIt.GetAttribute<string>("Type")));

	// create the entity
	PropertyList properties;
	EntityHandle entity = gEntityMgr.CreateEntity(desc, properties);
	PrototypeInfo* prototypeInfo = 0;
	if (isPrototype)
	{
		OC_ASSERT(mPrototypes.find(entity.GetID()) != mPrototypes.end());
		prototypeInfo = mPrototypes[entity.GetID()];
	}

	// set properties loaded from the file
	for (ResourceSystem::XMLNodeIterator cmpIt=xml->IterateChildren(entIt); cmpIt!=xml->EndChildren(entIt); ++cmpIt)
	{
		// skip unwanted data
		if ((*cmpIt).compare("Component") != 0)
			continue;

		for (ResourceSystem::XMLNodeIterator xmlPropertyIter=xml->IterateChildren(cmpIt); xmlPropertyIter!=xml->EndChildren(cmpIt); ++xmlPropertyIter)
		{
			// skip unwanted data
			if ((*xmlPropertyIter).compare("Type") == 0)
				continue;
			PropertyList::iterator propertyIter = properties.find(*xmlPropertyIter);
			if (propertyIter == properties.end())
			{
				ocError << "XML:Entity: Unknown entity property '" << *xmlPropertyIter << "' (it might not be marked as initable (PA_INIT))";
			}
			else
			{
				LoadEntityPropertyFromXML(prototypeInfo, propertyIter, xml, xmlPropertyIter, properties);
			}
		}
	}

	// finish init
	// note: the prototype copy can't be created here since all prototypes must be loaded first before any copy is created.
	//       otherwise it would screw up their IDs.
	entity.FinishInit();
}

bool EntitySystem::EntityMgr::LoadFromResource( ResourceSystem::ResourcePtr res, const bool isPrototype )
{
    ResourceSystem::XMLResourcePtr xml = boost::static_pointer_cast<ResourceSystem::XMLResource>(res);
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

bool EntitySystem::EntityMgr::HasEntityComponentOfType(const EntityHandle h, const eComponentType componentType)
{
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(h); it.HasMore(); ++it)
	{
		if ((*it)->GetType() == componentType) return true;
	}
	return false;
}

bool EntitySystem::EntityMgr::GetEntityComponentTypes(const EntityID id, ComponentTypeList& out)
{
	out.clear();
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(id); it.HasMore(); ++it)
	{
		out.push_back((*it)->GetType());
	}
	return true;
}

void EntitySystem::EntityMgr::UpdatePrototypeCopy( const EntityID prototype )
{
	EntityMap::iterator entityIter = mEntities.find(prototype);
	if (entityIter == mEntities.end())
	{
		ocError << "Can't update prototype copy; can't find entity " << prototype;
		return;
	}
	PrototypeMap::iterator prototypeIter = mPrototypes.find(prototype);
	if (prototypeIter == mPrototypes.end())
	{
		ocError << "Can't update prototype copy; entity " << prototype << " is not a prototype";
		return;
	}

	EntityInfo* entityInfo = entityIter->second;
	PrototypeInfo* prototypeInfo = prototypeIter->second;

	// recreate the copy from scratch
	if (mEntities.find(prototypeInfo->mCopy) != mEntities.end())
	{
		DestroyEntityImmediately(prototypeInfo->mCopy, true);
	}
	EntityDescription desc;
	desc.Init(entityInfo->mType);
	desc.SetKind(EntityDescription::EK_PROTOTYPE_COPY);
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(prototype); it.HasMore(); ++it)
	{
		desc.AddComponent((*it)->GetType());
	}
	PropertyList copyPropList;
	EntityHandle copyHandle = CreateEntity(desc, copyPropList);
	prototypeInfo->mCopy = copyHandle.GetID();

	// copy the properties
	PropertyList propList;
	GetEntityProperties(prototype, propList, PA_INIT); // only initable properties can be copied
	for (PropertyList::iterator it=propList.begin(); it!=propList.end(); ++it)
	{
		PropertyList::iterator copyPropIter = copyPropList.find(it->first);
		OC_ASSERT(copyPropIter != copyPropList.end());
		copyPropIter->second.CopyFrom(it->second);
	}
}

void EntitySystem::EntityMgr::UpdatePrototypeInstances( const EntityID prototype )
{
	for (EntityMap::iterator it=mEntities.begin(); it!=mEntities.end(); ++it)
	{
		if (it->second->mPrototype.GetID() == prototype)
		{
			UpdatePrototypeInstance(prototype, it->first, false);
		}
	}
}

void EntitySystem::EntityMgr::UpdatePrototypeInstance( const EntityID prototype, const EntityID instance, const bool forceOverwrite )
{
	PropertyList prototypeProperties;
	GetEntityProperties(prototype, prototypeProperties);
	OC_ASSERT(mPrototypes.find(prototype) != mPrototypes.end());
	PrototypeInfo* prototypeInfo = mPrototypes[prototype];

	// update shared properties
	for (PropertyList::iterator protPropIter=prototypeProperties.begin(); protPropIter!=prototypeProperties.end(); ++protPropIter)
	{
		if (prototypeInfo->mSharedProperties.find(protPropIter->first) == prototypeInfo->mSharedProperties.end())
			continue;

		StringKey propertyKey = protPropIter->first;
		PropertyHolder instanceProperty = GetEntityProperty(instance, propertyKey);
		PropertyHolder copyProperty = GetEntityProperty(prototypeInfo->mCopy, propertyKey);
		PropertyHolder prototypeProperty = protPropIter->second;
		
		// if the property has different value from the prototype copy, then it means the property was specialized
		// by the user and we should leave it alone
		if (forceOverwrite || copyProperty.IsEqual(instanceProperty))
		{
			instanceProperty.CopyFrom(prototypeProperty);
		}
	}

}

bool EntitySystem::EntityMgr::IsEntity( const EntityID id ) const
{
	return mEntities.find(id) != mEntities.end();
}

bool EntitySystem::EntityMgr::IsEntityPrototype( const EntityID id ) const
{
	if (!EntityHandle::IsPrototypeID(id))
		return false;

	return mPrototypes.find(id) != mPrototypes.end();
}

EntitySystem::ComponentID EntitySystem::EntityMgr::AddComponentToEntity( const EntityID id, const eComponentType componentType )
{
	OC_ASSERT(mComponentMgr);
	return mComponentMgr->CreateComponent(id, componentType);
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

void EntitySystem::EntityMgr::DestroyEntityComponent( const EntityID id, const ComponentID componentToDestroy )
{
	OC_ASSERT(mComponentMgr);
	EntityMap::iterator entityIter = mEntities.find(id);
	if (entityIter == mEntities.end())
	{
		ocError << "Entity " << id << " not found";
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
				ocInfo << "Unlinking entity from prototype because of component destruction";
				UnlinkEntityFromPrototype(id);
			}
		}
	}

	mComponentMgr->DestroyComponent(id, componentToDestroy);
}

int32 EntitySystem::EntityMgr::GetNumberOfEntityComponents( const EntityID id ) const
{
	OC_DASSERT(mComponentMgr);
	return mComponentMgr->GetNumberOfEntityComponents(id);
}

void EntitySystem::EntityMgr::LinkEntityToPrototype( const EntityID id, const EntityID prototype )
{
	PrototypeMap::iterator protIt = mPrototypes.find(prototype);
	if (protIt == mPrototypes.end())
	{
		ocError << "Cannot find prototype " << prototype << " to link";
		return;
	}

	if (protIt->second->mInstancesCount == 0) UpdatePrototypeCopy(prototype);
	protIt->second->mInstancesCount++;
	UpdatePrototypeInstance(prototype, id, true);
}

void EntitySystem::EntityMgr::UnlinkEntityFromPrototype( const EntityID id )
{
	EntityMap::iterator entityIt = mEntities.find(id);
	if (entityIt == mEntities.end())
	{
		ocError << "Cannot unlink entity " << id << " ; not found";
		return;
	}

	if (!entityIt->second->mPrototype.IsValid())
	{
		ocError << "Entity " << id << " is not linked to any prototype";
		return;
	}

	EntityID parentPrototypeID = entityIt->second->mPrototype.GetID();
	PrototypeMap::iterator parentPrototypeIter = mPrototypes.find(parentPrototypeID);
	if (parentPrototypeIter == mPrototypes.end())
	{
		ocError << "Entity prototype not found: " << parentPrototypeID;
		return;
	}

	parentPrototypeIter->second->mInstancesCount--;
}