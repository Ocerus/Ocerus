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
	if (gEntityMgr.IsEntity(id))
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
	} while (gEntityMgr.IsEntity(id));
}

void EntitySystem::EntityHandle::DecID( EntityID& id )
{
	do 
	{
		--id;
		if (id >= 0) id = -1;
	} while (gEntityMgr.IsEntity(id));
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

EntitySystem::EntityHandle::EntityHandle( const EntityHandle& handle ): mEntityID(handle.mEntityID) {}

EntitySystem::EntityHandle::EntityHandle( void ): mEntityID(0) {}

bool EntitySystem::EntityHandle::operator==( const EntityHandle& rhs )
{
	return mEntityID == rhs.mEntityID;
}

bool EntitySystem::EntityHandle::operator!=( const EntityHandle& rhs )
{
	return mEntityID != rhs.mEntityID;
}

EntitySystem::eEntityType EntitySystem::EntityHandle::GetType( void ) const
{
	// if this is not optimized by the linker, then we have to inline it
	return gEntityMgr.GetEntityType(*this);
}

void EntitySystem::EntityHandle::FinishInit( void )
{
	// avoid ignored message by using EntityMgr directly for posting messages
	gEntityMgr.PostMessage(*this, EntityMessage(EntityMessage::INIT));
	gEntityMgr.PostMessage(*this, EntityMessage(EntityMessage::POST_INIT));
}

bool EntitySystem::EntityHandle::GetProperties( PropertyList& out, const PropertyAccessFlags mask )
{
	return gEntityMgr.GetEntityProperties(*this, out, mask);
}

PropertyHolder EntitySystem::EntityHandle::GetFunction( const StringKey key, const PropertyAccessFlags mask ) const
{
	// if this is not optimized by the linker, then we have to inline it in both EntityMgr and EntityHandle
	return gEntityMgr.GetEntityProperty(*this, key, mask);
}

PropertyHolder EntitySystem::EntityHandle::GetProperty( const StringKey key, const PropertyAccessFlags mask ) const
{
	// if this is not optimized by the linker, then we have to inline it in both EntityMgr and EntityHandle
	return gEntityMgr.GetEntityProperty(*this, key, mask);
}

Reflection::PropertyHolder EntitySystem::EntityHandle::GetComponentProperty( const ComponentID componentID, const StringKey key, const PropertyAccessFlags mask /*= PA_FULL_ACCESS*/ ) const
{
	// if this is not optimized by the linker, then we have to inline it in both EntityMgr and EntityHandle
	return gEntityMgr.GetEntityComponentProperty(*this, componentID, key, mask);
}

bool EntitySystem::EntityHandle::Exists( void ) const
{
	// if this is not optimized by the linker, then we have to inline it
	return gEntityMgr.EntityExists(*this);
}

EntitySystem::EntityHandle::~EntityHandle( void )
{

}

EntityMessage::eResult EntityHandle::PostMessage(const EntityMessage& msg)
{
	OC_DASSERT(IsValid());
	EntityMessage::eResult result = gEntityMgr.PostMessage(*this, msg);
	if (result == EntityMessage::RESULT_ERROR)
	{
		ocError << "Message '" << msg.type << "' on entity '" << mEntityID << "' of type '" << GetType() << "' returned error";
	}
	else if (result == EntityMessage::RESULT_IGNORED)
	{
		ocWarning << "Message '" << msg.type << "' on entity '" << mEntityID << "' of type '" << GetType() << "' was ignored";
	}
	return result;
}