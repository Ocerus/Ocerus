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

EntityHandle EntityMgr::CreateEntity(const EntityDescription& desc)
{
	assert(mComponentMgr);
	EntityDescription::ComponentDescriptionsList::const_iterator i = desc.mComponentDescriptions.begin();
	assert(i!=desc.mComponentDescriptions.end());
	EntityHandle h = EntityHandle::CreateUniqueHandle();
	//TODO check if the handle is really unique
	for (; i!=desc.mComponentDescriptions.end(); ++i)
	{
		bool created = mComponentMgr->CreateComponent(h, **i); // fuj
		assert(created);
	}
	mEntities.insert(std::pair<EntityID, eEntityType>(h.GetID(), desc.mType));
	return h;
}


void EntityMgr::DestroyEntity(EntityHandle h)
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

EntitySystem::eEntityType EntitySystem::EntityMgr::GetEntityType( EntityHandle h ) const
{
	EntityMap::const_iterator ei = mEntities.find(h.GetID());
	if (ei == mEntities.end())
	{
		gLogMgr.LogMessage("Can't find entity");
		return ET_UNKNOWN;
	}
	return ei->second;
}