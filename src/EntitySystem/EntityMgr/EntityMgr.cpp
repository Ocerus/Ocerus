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
	for (; i!=desc.mComponents.end(); ++i)
	{
		bool created = mComponentMgr->CreateComponent(h, *i);
		assert(created);
	}
	mEntities.insert(std::pair<EntityID, eEntityType>(h.GetID(), desc.mType));
	if (!GetEntityProperties(h.GetID(), out, PROPACC_INIT))
		gLogMgr.LogMessage("Can't get properties for created entity", h.GetID());
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
		gLogMgr.LogMessage("Can't find entity");
		return ET_UNKNOWN;
	}
	return ei->second;
}

bool EntitySystem::EntityMgr::GetEntityProperties( const EntityHandle h, PropertyList& out, const uint8 flagMask )
{
	return mComponentMgr->GetEntityProperties(h, out, flagMask);
}