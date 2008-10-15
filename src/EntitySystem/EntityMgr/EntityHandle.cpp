#include "EntityHandle.h"

using namespace EntitySystem;

EntityHandle& EntityHandle::operator=(const EntityHandle& rhs)
{ 
	if (this != &rhs)
		mEntityID = rhs.mEntityID; 
	return *this;
}


EntityHandle EntityHandle::CreateUniqueHandle()
{
	//TODO this should check if the sLastID+1 entity exists
	return EntityHandle(++sLastID);
}

