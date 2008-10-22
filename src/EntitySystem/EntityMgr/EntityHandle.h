#ifndef _ENTITYHANDLE_H_
#define _ENTITYHANDLE_H_

#include "../../Utility/Settings.h"

namespace EntitySystem
{

	typedef uint32 EntityID;

	// forward declaration
	class EntityMgr;

	class EntityHandle
	{
	public:
		EntityHandle(const EntityHandle& handle) { mEntityID = handle.mEntityID; }
		~EntityHandle(void);

		EntityHandle& operator=(const EntityHandle& rhs);

		//TODO possibility to add helper methods for accessing components or sending messages
		//TODO possibly we could add a smartpointer to EntityNode holding this Handle

	private:
		EntityHandle(EntityID ID): mEntityID(ID) {}
		EntityHandle CreateUniqueHandle();

		EntityID mEntityID;
		static EntityID sLastID;
	};

	EntityID EntityHandle::sLastID = 0;
}

#endif
