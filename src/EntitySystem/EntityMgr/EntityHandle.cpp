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

EntityMessage::eResult EntityHandle::PostMessage(const EntityMessage::eType type, void* data)
{
	assert(IsValid());
	return gEntityMgr.PostMessage(*this, EntityMessage(type, data));
}