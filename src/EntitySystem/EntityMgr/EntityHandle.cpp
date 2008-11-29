#include "Common.h"
#include "EntityHandle.h"

using namespace EntitySystem;

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

EntitySystem::EntityID EntitySystem::EntityHandle::GetID( void )
{
	assert(mEntityID && "Invalid entity handle");
	return mEntityID;
}

bool EntitySystem::EntityHandle::operator==( const EntityHandle& rhs )
{
	return mEntityID == rhs.mEntityID;
}
EntityMessage::eResult EntityHandle::PostMessage(const EntityMessage::eType type, void* data)
{
	assert(IsValid());
	EntityMessage::eResult result = gEntityMgr.PostMessage(*this, EntityMessage(type, data));
	//TODO tady hlasit TYPe netity misto jejiho ID
	if (result == EntityMessage::RESULT_ERROR)
		gLogMgr.LogMessage("Message '", type, "' on entity '", mEntityID, "' returned error", LOG_ERROR);
	else if (result == EntityMessage::RESULT_IGNORED)
		gLogMgr.LogMessage("WARNING: Message '", type, "' on entity '", mEntityID, "' returned error");
	return result;
}