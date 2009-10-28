#include "Common.h"
#include "EntityMgr.h"
#include "EntityDescription.h"
#include "../ComponentMgr/ComponentMgr.h"
#include "../ComponentMgr/Component.h"
#include "../../ResourceSystem/XMLResource.h"

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

	if (desc.mComponents.size() == 0)
	{
		ocWarning << "Attempting to create an entity without components";
		return EntityHandle::Null; // no components, so we can't create the entity
	}

	// create the handle for the new entity
	EntityHandle h = EntityHandle::CreateUniqueHandle();

	bool dependencyFailure = false;
	set<eComponentType> createdComponentTypes;

	for (ComponentDependencyList::const_iterator cmpDescIt=desc.mComponents.begin(); cmpDescIt!=desc.mComponents.end(); ++cmpDescIt)
	{
		eComponentType cmpType = *cmpDescIt;

		// create the component
		Component* cmp = mComponentMgr->CreateComponent(h, cmpType);
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

	// inits the attributes for the new components
	mEntities[h.GetID()] = new EntityInfo(desc.mType, desc.mID);

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
			DestroyEntity(mapIt);
			mEntities.erase(mapIt);
		}
	}
	mEntityDestroyQueue.clear();
}

void EntityMgr::DestroyAllEntities()
{
	OC_ASSERT(mComponentMgr);
	for (EntityMap::const_iterator i = mEntities.begin(); i!=mEntities.end(); ++i)
		DestroyEntity(i);
	mEntities.clear();
}

void EntityMgr::DestroyEntity( EntityMap::const_iterator it )
{
	if (it != mEntities.end())
	{
		mComponentMgr->DestroyEntityComponents(it->first);
		delete it->second;
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

bool EntitySystem::EntityMgr::GetEntityProperties( const EntityHandle h, PropertyList& out, const PropertyAccessFlags flagMask ) const
{
	out.clear();

	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(h); it.HasMore(); ++it)
	{
		(*it)->GetRTTI()->EnumProperties(*it, out, flagMask);
	}
	
	return true;
}

PropertyHolder EntitySystem::EntityMgr::GetEntityProperty( const EntityHandle h, const StringKey key, const PropertyAccessFlags flagMask /*= 0xff*/ ) const
{
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(h); it.HasMore(); ++it)
	{
		AbstractProperty* prop = (*it)->GetRTTI()->GetProperty(key, flagMask);
		if (prop) return PropertyHolder(*it, prop);
	}


	// property not found, print some info about why
	ocError << "EntityMgr: unknown property '" << key << "'";
	PropertyList propertyList;
	string propertiesString;
	GetEntityProperties(h, propertyList, flagMask);
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

EntitySystem::EntityHandle EntitySystem::EntityMgr::FindFirstEntity( const string& ID )
{
	for(EntityMap::const_iterator i=mEntities.begin(); i!=mEntities.end(); ++i)
		if (i->second->mID.compare(ID) == 0)
			return i->first;
	return EntityHandle::Null;
}

bool EntitySystem::EntityMgr::LoadFromResource( ResourceSystem::ResourcePtr res )
{
	ResourceSystem::XMLResourcePtr xml = res;
	if (xml.IsNull())
	{
		ocError << "XML: Can't load data";
		return false;
	}

	for (ResourceSystem::XMLResource::NodeIterator entIt=xml->IterateTopLevel(); entIt!=xml->EndTopLevel(); ++entIt)
	{
		if ((*entIt).compare("Entity") == 0)
		{
			// init the entity description
			EntityDescription desc;
			eEntityType type = DetectEntityType(entIt.GetAttribute<string>("Type"));
			string ID = entIt.GetAttribute<string>("Name");
			desc.Init(type, ID);
			// add component types
			for (ResourceSystem::XMLResource::NodeIterator cmpIt=xml->IterateChildren(entIt); cmpIt!=xml->EndChildren(entIt); ++cmpIt)
				if ((*cmpIt).compare("Component") == 0)
					desc.AddComponent(DetectComponentType(cmpIt.GetAttribute<string>("Type")));
			// create the entity
			PropertyList props;
			EntityHandle entity = CreateEntity(desc, props);
			// set properties loaded from the file
			for (ResourceSystem::XMLResource::NodeIterator cmpIt=xml->IterateChildren(entIt); cmpIt!=xml->EndChildren(entIt); ++cmpIt)
			{
				// skip unwanted data
				if ((*cmpIt).compare("Component") != 0)
					continue;

				for (ResourceSystem::XMLResource::NodeIterator propIt=xml->IterateChildren(cmpIt); propIt!=xml->EndChildren(cmpIt); ++propIt)
				{
					// skip unwanted data
					if ((*propIt).compare("Type") == 0)
						continue;
					PropertyList::iterator prop = props.find(*propIt);
					if (prop == props.end())
					{
						ocError << "XML:Entity: Unknown entity property '" << *propIt, "'";
					}
					else
					{
						PropertyHolder p = prop->second;
						if (p.GetType() == PT_VECTOR2_ARRAY)
						{
							string lengthParam;
							vector<Vector2> vertices;
							for (ResourceSystem::XMLResource::NodeIterator vertIt=xml->IterateChildren(propIt); vertIt!=xml->EndChildren(propIt); ++vertIt)
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
							else if (props.find(lengthParam) == props.end())
							{
								ocError << "XML:Entity: LengthParam of name '" << lengthParam << "' not found in entity";
							}
							else
							{
								props[lengthParam].SetValue<uint32>(vertices.size());
								Vector2* vertArray = new Vector2[vertices.size()];
								for (uint32 i=0; i<vertices.size(); ++i)
									vertArray[i] = vertices[i];
								p.SetValue<Vector2*>(vertArray);
							}
						}
						else if (p.GetType() == PT_ENTITYHANDLE)
						{
							EntityHandle e = FindFirstEntity(propIt.GetChildValue<string>());
							if (!e.IsValid())
								ocError << "XML:Entity: Entity for property '" << *propIt << "' of name '" << propIt.GetChildValue<string>() << "' was not found";
							p.SetValue(e);
						}
						else
						{
							p.SetValueFromString(propIt.GetChildValue<string>());
						}
					}
				}
			}
			// finish init
			entity.FinishInit();
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

EntitySystem::EntityMgr::EntityInfo::EntityInfo( void ):
	mType(ET_UNKNOWN), 
	mFullyInited(false)
{

}

bool EntitySystem::EntityMgr::HasEntityComponentOfType(const EntityHandle h, const eComponentType componentType)
{
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(h); it.HasMore(); ++it)
	{
		if ((*it)->GetType() == componentType) return true;
	}
	return false;
}
	
bool EntitySystem::EntityMgr::GetEntityComponentTypes(const EntityHandle h, ComponentTypeList& out)
{
	out.clear();
	for (EntityComponentsIterator it=mComponentMgr->GetEntityComponents(h); it.HasMore(); ++it)
	{
		out.push_back((*it)->GetType());
	}
	return true;
}