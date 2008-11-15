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
	for (EntitySet::const_iterator i = mEntitySet.begin(); i!=mEntitySet.end(); ++i)
		PostMessage(*i, msg);
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
	mEntitySet.insert(h.mEntityID);
	return h;
}


void EntityMgr::DestroyEntity(EntityHandle h)
{
	assert(mComponentMgr);
	mComponentMgr->DestroyEntityComponents(h);
	mEntitySet.erase(h.mEntityID);
}

void EntityMgr::DestroyAllEntities()
{
	assert(mComponentMgr);
	for (EntitySet::const_iterator i = mEntitySet.begin(); i!=mEntitySet.end(); ++i)
		mComponentMgr->DestroyEntityComponents(*i);
	mEntitySet.clear();
}