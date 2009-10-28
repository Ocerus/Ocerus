#include "Common.h"
#include "EntityHandle.h"
#include "EntityMessage.h"

using namespace EntitySystem;

const EntityHandle EntityHandle::Null;
EntityID EntityHandle::sLastID = 0;

EntityHandle& EntityHandle::operator=(const EntityHandle& rhs)
{ 
	if (this != &rhs)
		mEntityID = rhs.mEntityID; 
	return *this;
}


EntityHandle EntityHandle::CreateUniqueHandle()
{
	return EntityHandle(++sLastID);
}


EntitySystem::EntityHandle::EntityHandle( const EntityHandle& handle ): mEntityID(handle.mEntityID) {}

EntitySystem::EntityHandle::EntityHandle( void ): mEntityID(0) {}

EntitySystem::EntityID EntitySystem::EntityHandle::GetID( void ) const
{
	OC_DASSERT_MSG(mEntityID, "Invalid entity handle");
	return mEntityID;
}

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