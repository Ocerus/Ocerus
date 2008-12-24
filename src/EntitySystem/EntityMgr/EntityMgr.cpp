#include "Common.h"
#include "EntityMgr.h"
#include "EntityDescription.h"
#include "../ComponentMgr/ComponentMgr.h"
#include "../ComponentMgr/Component.h"

using namespace EntitySystem;

EntityMgr::EntityMgr()
{
	gLogMgr.LogMessage("*** EntityMgr init ***");

	mComponentMgr = DYN_NEW ComponentMgr();

	// this assumes EntityMgr is a singleton
	EntityPicker::SetupPriorities();
}

EntityMgr::~EntityMgr()
{
	DestroyAllEntities();
	assert(mComponentMgr);
	DYN_DELETE mComponentMgr;
}

EntityMessage::eResult EntityMgr::PostMessage(EntityID id, const EntityMessage& msg)
{
	EntityMap::iterator ei = mEntities.find(id);
	if (ei == mEntities.end())
	{
		gLogMgr.LogMessage("Can't find entity", id, LOG_ERROR);
		return EntityMessage::RESULT_ERROR;
	}
	if (msg.type != EntityMessage::TYPE_POST_INIT && !ei->second.fullyInited)
	{
		gLogMgr.LogMessage("Entity '", id, "' is not initialized -> can't post messages", LOG_ERROR);
		return EntityMessage::RESULT_ERROR;
	}
	if (msg.type == EntityMessage::TYPE_POST_INIT && ei->second.fullyInited)
	{
		gLogMgr.LogMessage("Entity '", id, "' is already initialized", LOG_ERROR);
		return EntityMessage::RESULT_ERROR;
	}
	if (msg.type == EntityMessage::TYPE_POST_INIT)
		ei->second.fullyInited = true;

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

EntityHandle EntityMgr::CreateEntity(const EntityDescription& desc, PropertyList& out)
{
	assert(mComponentMgr);
	EntityDescription::ComponentDescriptionsList::const_iterator i = desc.mComponents.begin();
	assert(i!=desc.mComponents.end());
	EntityHandle h = EntityHandle::CreateUniqueHandle();
	//TODO check if the handle is really unique

	bool dependencyFailure = false;
	std::set<eComponentType> cmpTypes;

	for (; i!=desc.mComponents.end(); ++i)
	{
		Component* cmp = mComponentMgr->CreateComponent(h, *i);
		assert(cmp);
		
		// check dependencies
		ComponentDependencyList depList;
		cmp->GetRTTI()->EnumComponentDependencies(depList);
		for (ComponentDependencyList::const_iterator depIt=depList.begin(); depIt!=depList.end(); ++depIt)
			if (cmpTypes.find(*depIt) == cmpTypes.end())
				dependencyFailure = true;

		cmpTypes.insert(*i);
	}
	mEntities[h.GetID()] = EntityInfo(desc.mType);

	if (dependencyFailure)
	{
		gLogMgr.LogMessage("Component dependency failure on entity '", h.GetID(), "' of type '", desc.mType, "'", LOG_ERROR);
		DestroyEntity(h);
		return h; // do like nothing happened, but don't enum properties or they will access invalid memory
	}

	if (!GetEntityProperties(h.GetID(), out, PROPACC_INIT))
		gLogMgr.LogMessage("Can't get properties for created entity of type", desc.mType, LOG_ERROR);


	return h;
}


void EntityMgr::DestroyEntity(const EntityHandle h)
{
	assert(mComponentMgr);
	mComponentMgr->DestroyEntityComponents(h);
	mEntities.erase(h.GetID());
}

void EntityMgr::DestroyAllEntities()
{
	assert(mComponentMgr);
	for (EntityMap::const_iterator i = mEntities.begin(); i!=mEntities.end(); ++i)
		mComponentMgr->DestroyEntityComponents(i->first);
	mEntities.clear();
}

EntitySystem::eEntityType EntitySystem::EntityMgr::GetEntityType( const EntityHandle h ) const
{
	EntityMap::const_iterator ei = mEntities.find(h.GetID());
	if (ei == mEntities.end())
	{
		gLogMgr.LogMessage("Can't find entity", h.GetID(), LOG_ERROR);
		return ET_UNKNOWN;
	}
	return ei->second.type;
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
	return ei->second.fullyInited;
}

PropertyHolder EntitySystem::EntityMgr::GetEntityProperty( const EntityHandle h, const StringKey key, const PropertyAccessFlags flagMask /*= 0xff*/ )
{
	return mComponentMgr->GetEntityProperty(h, key, flagMask);
}
