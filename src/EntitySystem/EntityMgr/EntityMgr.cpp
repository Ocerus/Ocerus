#include "Common.h"
#include "EntityMgr.h"
#include "EntityDescription.h"
#include "../ComponentMgr/ComponentMgr.h"
#include "../ComponentMgr/Component.h"
#include "../../ResourceSystem/XMLResource.h"

using namespace EntitySystem;

EntityMgr::EntityMgr()
{
	gLogMgr.LogMessage("*** EntityMgr init ***");

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
		gLogMgr.LogMessage("Can't find entity", id, LOG_ERROR);
		return EntityMessage::RESULT_ERROR;
	}
	if (msg.type != EntityMessage::TYPE_POST_INIT && !ei->second->mFullyInited)
	{
		gLogMgr.LogMessage("Entity '", id, "' is not initialized -> can't post messages", LOG_ERROR);
		return EntityMessage::RESULT_ERROR;
	}
	if (msg.type == EntityMessage::TYPE_POST_INIT && ei->second->mFullyInited)
	{
		gLogMgr.LogMessage("Entity '", id, "' is already initialized", LOG_ERROR);
		return EntityMessage::RESULT_ERROR;
	}
	if (msg.type == EntityMessage::TYPE_POST_INIT)
		ei->second->mFullyInited = true;

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
	BS_ASSERT(mComponentMgr);

	if (desc.mComponents.size() == 0)
	{
		gLogMgr.LogMessage("Attempting to create an entity without components");
		return EntityHandle::Null; // no components, so we can't create the entity
	}

	// create the handle for the new entity
	EntityHandle h = EntityHandle::CreateUniqueHandle();
	//TODO check if the handle is really unique

	bool dependencyFailure = false;
	set<eComponentType> createdComponentTypes;

	for (ComponentDependencyList::const_iterator cmpDescIt=desc.mComponents.begin(); cmpDescIt!=desc.mComponents.end(); ++cmpDescIt)
	{
		eComponentType cmpType = *cmpDescIt;

		// create the component
		Component* cmp = mComponentMgr->CreateComponent(h, cmpType);
		BS_ASSERT(cmp);
		
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
		gLogMgr.LogMessage("Component dependency failure on entity '", h.GetID(), "' of type '", desc.mType, "'", LOG_ERROR);
		DestroyEntity(h);
		return h; // do like nothing's happened, but don't enum properties or they will access invalid memory
	}
	if (!GetEntityProperties(h.GetID(), out, PROPACC_INIT))
	{
		gLogMgr.LogMessage("Can't get properties for created entity of type", desc.mType, LOG_ERROR);
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
		EntityMap::const_iterator mapIt = mEntities.find(*it);
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
	BS_ASSERT(mComponentMgr);
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
		gLogMgr.LogMessage("Can't find entity", h.GetID(), LOG_ERROR);
		return ET_UNKNOWN;
	}
	return ei->second->mType;
}

bool EntitySystem::EntityMgr::GetEntityProperties( const EntityHandle h, PropertyList& out, const PropertyAccessFlags flagMask )
{
	return mComponentMgr->GetEntityProperties(h, out, flagMask);
}

bool EntitySystem::EntityMgr::IsEntityInited( const EntityHandle h ) const
{
	EntityMap::const_iterator ei = mEntities.find(h.GetID());
	if (ei == mEntities.end())
	{
		gLogMgr.LogMessage("Can't find entity", h.GetID(), LOG_ERROR);
		return false;
	}
	return ei->second->mFullyInited;
}

PropertyHolderMediator EntitySystem::EntityMgr::GetEntityProperty( const EntityHandle h, const StringKey key, const PropertyAccessFlags flagMask /*= 0xff*/ ) const
{
	return mComponentMgr->GetEntityProperty(h, key, flagMask);
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
		gLogMgr.LogMessage("XML:Can't load file", LOG_ERROR);
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
			// init properties
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
						gLogMgr.LogMessage("XML:Entity:Unknown entity property '", *propIt, "'", LOG_ERROR);
					}
					else
					{
						PropertyHolder p = prop->second;
						//TODO tenhle switch asi bude chtit premistit nekam, odkud se na to dostane i LUA
						//nejlip do PropertyHolder a hodnotu tam predat jako string a rozparsovat az uvnitr
						//V Lue ale jinak rozparsovat EntityHandle, pac tam to bude asi primo EntityHandle
						switch (p.GetType())
						{
						case PROPTYPE_BOOL:
							p.SetValue(propIt.GetChildValue<bool>());
							break;
						case PROPTYPE_FLOAT32:
							p.SetValue(propIt.GetChildValue<float32>());
							break;
						case PROPTYPE_INT16:
							p.SetValue(propIt.GetChildValue<int16>());
							break;
						case PROPTYPE_INT32:
							p.SetValue(propIt.GetChildValue<int32>());
							break;
						case PROPTYPE_INT64:
							p.SetValue(propIt.GetChildValue<int64>());
							break;
						case PROPTYPE_INT8:
							p.SetValue(propIt.GetChildValue<int8>());
							break;
						case PROPTYPE_UINT16:
							p.SetValue(propIt.GetChildValue<uint16>());
							break;
						case PROPTYPE_UINT32:
							p.SetValue(propIt.GetChildValue<uint32>());
							break;
						case PROPTYPE_UINT64:
							p.SetValue(propIt.GetChildValue<uint64>());
							break;
						case PROPTYPE_UINT8:
							p.SetValue(propIt.GetChildValue<uint8>());
							break;
						case PROPTYPE_STRING:
							p.SetValue(propIt.GetChildValue<string>().c_str());
							break;
						case PROPTYPE_STRING_KEY:
							p.SetValue<StringKey>(propIt.GetChildValue<string>());
							break;
						case PROPTYPE_COLOR:
							p.SetValue(propIt.GetChildValue<GfxSystem::Color>());
							break;
						case PROPTYPE_VECTOR2:
							p.SetValue(propIt.GetChildValue<Vector2>());
							break;
						case PROPTYPE_VECTOR2_REFERENCE:
							p.SetValue<Vector2&>(propIt.GetChildValue<Vector2>());
							break;
						case PROPTYPE_VECTOR2_ARRAY:
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
										gLogMgr.LogMessage("XML:Entity:Expected 'Vertex' or 'LengthParam', found '", *vertIt, "'", LOG_ERROR);
								}
								if (lengthParam.length() == 0)
									gLogMgr.LogMessage("XML:Entity:LengthParam of an array not specified", LOG_ERROR);
								else if (props.find(lengthParam) == props.end())
									gLogMgr.LogMessage("XML:Entity:LengthParam of name '", lengthParam, "' not found in entity", LOG_ERROR);
								else
								{
									props[lengthParam].SetValue<uint32>(vertices.size());
									Vector2* vertArray = new Vector2[vertices.size()];
									for (uint32 i=0; i<vertices.size(); ++i)
										vertArray[i] = vertices[i];
									p.SetValue<Vector2*>(vertArray);
								}
							}
							break;
						case PROPTYPE_ENTITYHANDLE:
							{
								EntityHandle e = FindFirstEntity(propIt.GetChildValue<string>());
								if (!e.IsValid())
									gLogMgr.LogMessage("XML:Entity:Entity for property '", *propIt, "' of name '", propIt.GetChildValue<string>(), "' was not found", LOG_WARNING);
								p.SetValue(e);
							}
							break;
						default:
							gLogMgr.LogMessage("XML:Entity:Can't parse property type '", p.GetType(), "'", LOG_ERROR);
						}
					}
				}
			}
			// finish init
			entity.FinishInit();
		}
		else
		{
			gLogMgr.LogMessage("XML:Expected 'Entity', found '", *entIt, "'", LOG_ERROR);
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

void EntitySystem::EntityMgr::EnumerateEntities( vector<EntityHandle>& out, const eEntityType desiredType )
{
	out.clear();
	for (EntityMap::const_iterator it=mEntities.begin(); it!=mEntities.end(); ++it)
		if (desiredType == NUM_ENTITY_TYPES || it->second->mType == desiredType)
			out.push_back(it->first);
}

EntitySystem::EntityMgr::EntityInfo::EntityInfo( void ):
	mType(ET_UNKNOWN), 
	mFullyInited(false)
{

}