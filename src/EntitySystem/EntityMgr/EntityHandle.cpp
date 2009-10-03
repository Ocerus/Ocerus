#include "Common.h"
#include "EntityHandle.h"

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
	BS_DASSERT_MSG(mEntityID, "Invalid entity handle");
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
	return gEntityMgr.GetEntityType(*this);
}

void EntitySystem::EntityHandle::FinishInit( void )
{
	// avoid ignored message by using EntityMgr directly
	gEntityMgr.PostMessage(*this, EntityMessage(EntityMessage::TYPE_POST_INIT));
}

bool EntitySystem::EntityHandle::GetProperties( PropertyList& out, const PropertyAccessFlags mask )
{
	return gEntityMgr.GetEntityProperties(*this, out, mask);
}

PropertyHolderMediator EntitySystem::EntityHandle::GetProperty( const StringKey key, const PropertyAccessFlags mask ) const
{
	return gEntityMgr.GetEntityProperty(*this, key, mask);
}

bool EntitySystem::EntityHandle::Exists( void ) const
{
	return gEntityMgr.EntityExists(*this);
}

EntitySystem::EntityHandle::~EntityHandle( void )
{

}

EntityMessage::eResult EntityHandle::PostMessage(const EntityMessage::eType type, void* data)
{
	BS_DASSERT(IsValid());
	/*if (!IsValid())
	{
		gLogMgr.LogMessage("PostMesage: Invalid entity", LOG_ERROR);
		return EntityMessage::RESULT_ERROR;
	}*/
	EntityMessage::eResult result = gEntityMgr.PostMessage(*this, EntityMessage(type, data));
	//TODO tady hlasit TYPe netity misto jejiho ID
	if (result == EntityMessage::RESULT_ERROR)
		gLogMgr.LogMessage("Message '", type, "' on entity '", mEntityID, "' returned error", LOG_ERROR);
	else if (result == EntityMessage::RESULT_IGNORED)
		gLogMgr.LogMessage("Message '", type, "' on entity '", mEntityID, "' returned warning", LOG_WARNING);
	return result;
}