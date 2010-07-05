#include "Common.h"
#include "EntityHandle.h"
#include "EntityMessage.h"

using namespace EntitySystem;

const EntityHandle EntityHandle::Null;
EntityID EntityHandle::sLastFreeID = 1;

EntityHandle EntityHandle::CreateUniqueHandle()
{
	EntityID newID = sLastFreeID;
	IncID(sLastFreeID);
	return EntityHandle(newID);
}

EntitySystem::EntityHandle EntitySystem::EntityHandle::CreateUniquePrototypeHandle()
{
	Core::Config& config = GlobalProperties::Get<Core::Config>("GlobalConfig");
	EntityID lastPrototypeID = config.GetInt32("LastFreePrototypeID", -1, "Entities");
	EntityID newID = lastPrototypeID;
	DecID(lastPrototypeID);
	config.SetInt32("LastFreePrototypeID", lastPrototypeID, "Entities");
	return EntityHandle(newID);
}

EntitySystem::EntityHandle EntitySystem::EntityHandle::CreateHandleFromID( const EntityID id )
{
	if (gEntityMgr.EntityExists(id))
	{
		ocError << "Cannot create entity with predetermined ID; another entity is already using the ID: " << id;
		return EntityHandle::Null;
	}

	if (IsPrototypeID(id))
	{
		Core::Config& config = GlobalProperties::Get<Core::Config>("GlobalConfig");
		EntityID lastPrototypeID = config.GetInt32("LastFreePrototypeID", -1, "Entities");
		if (lastPrototypeID == id)
		{
			DecID(lastPrototypeID);
			config.SetInt32("LastFreePrototypeID", lastPrototypeID, "Entities");
		}
	}
	else
	{
		if (sLastFreeID == id) IncID(sLastFreeID);
	}
	return EntityHandle(id);
}

void EntitySystem::EntityHandle::IncID( EntityID& id )
{
	do
	{
		++id;
		if (id <= 0) id = 1;
	} while (gEntityMgr.EntityExists(id));
}

void EntitySystem::EntityHandle::DecID( EntityID& id )
{
	do
	{
		--id;
		if (id >= 0) id = -1;
	} while (gEntityMgr.EntityExists(id));
}

bool EntitySystem::EntityHandle::IsPrototypeID( const EntityID id )
{
	return id < 0;
}

EntityHandle& EntityHandle::operator=(const EntityHandle& rhs)
{
	if (this != &rhs)
		mEntityID = rhs.mEntityID;
	return *this;
}

EntitySystem::EntityHandle::EntityHandle( void ): mEntityID(INVALID_ENTITY_ID) {}

bool EntitySystem::EntityHandle::operator==( const EntityHandle& rhs ) const
{
	return mEntityID == rhs.mEntityID;
}

bool EntitySystem::EntityHandle::operator!=( const EntityHandle& rhs ) const
{
	return mEntityID != rhs.mEntityID;
}

void EntitySystem::EntityHandle::FinishInit( void ) const
{
	// avoid ignored message by using EntityMgr directly for posting messages
	gEntityMgr.PostMessage(*this, EntityMessage(EntityMessage::INIT));
	gEntityMgr.PostMessage(*this, EntityMessage(EntityMessage::POST_INIT));
}

bool EntitySystem::EntityHandle::IsInited( void ) const
{
	return gEntityMgr.IsEntityInited(*this);
}

bool EntitySystem::EntityHandle::GetProperties( PropertyList& out, const PropertyAccessFlags mask ) const
{
	return gEntityMgr.GetEntityProperties(*this, out, mask);
}

PropertyHolder EntitySystem::EntityHandle::GetFunction( const StringKey key, const PropertyAccessFlags mask ) const
{
	return gEntityMgr.GetEntityProperty(*this, key, mask);
}

bool EntitySystem::EntityHandle::HasProperty( const StringKey key, const PropertyAccessFlags mask ) const
{
	return gEntityMgr.HasEntityProperty(*this, key, mask);
}

bool EntitySystem::EntityHandle::HasComponentProperty(const ComponentID componentID, const StringKey key, const PropertyAccessFlags mask ) const
{
  return gEntityMgr.HasEntityComponentProperty(*this, componentID, key, mask);
}

PropertyHolder EntitySystem::EntityHandle::GetProperty( const StringKey key, const PropertyAccessFlags mask ) const
{
	return gEntityMgr.GetEntityProperty(*this, key, mask);
}

Reflection::PropertyHolder EntitySystem::EntityHandle::GetComponentProperty( const ComponentID componentID, const StringKey key, const PropertyAccessFlags mask ) const
{
	return gEntityMgr.GetEntityComponentProperty(*this, componentID, key, mask);
}

bool EntitySystem::EntityHandle::UnregisterDynamicPropertyOfComponent(const ComponentID component, const StringKey propertyKey) const
{
	return gEntityMgr.UnregisterDynamicPropertyOfEntityComponent(*this, component, propertyKey);
}

bool EntitySystem::EntityHandle::Exists( void ) const
{
	return gEntityMgr.EntityExists(*this);
}

EntitySystem::EntityHandle::~EntityHandle( void )
{

}

string EntitySystem::EntityHandle::GetName( void ) const
{
	return gEntityMgr.GetEntityName(*this);
}

bool EntitySystem::EntityHandle::operator<( const EntityHandle& rhs ) const
{
	return mEntityID < rhs.mEntityID;
}

uint16 EntitySystem::EntityHandle::GetTag( void ) const
{
	return gEntityMgr.GetEntityTag(*this);
}

void EntitySystem::EntityHandle::SetTag( uint16 tag )
{
	gEntityMgr.SetEntityTag(*this, tag);
}

EntityMessage::eResult EntityHandle::PostMessage(const EntityMessage& msg) const
{
	OC_DASSERT(IsValid());
	EntityMessage::eResult result = gEntityMgr.PostMessage(*this, msg);
	if (result == EntityMessage::RESULT_ERROR)
	{
		ocError << "Message '" << msg.type << "' on entity '" << mEntityID << "' returned error";
	}
	else if (result == EntityMessage::RESULT_IGNORED)
	{
		ocWarning << "Message '" << msg.type << "' on entity '" << mEntityID << "' was ignored";
	}
	return result;
}