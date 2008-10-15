#ifndef _ENTITYHANDLE_H_
#define _ENTITYHANDLE_H_

#include "../Utility/Settings.h"

namespace EntitySystem
{
	// forward declaration
	class EntityMgr;

	class EntityHandle
	{
	public:
		EntityHandle(const EntityHandle& handle) { mEntityID = handle.mEntityID; }
		~EntityHandle();

		EntityHandle& operator=(const EntityHandle& rhs);

		//TODO possibility to add helper methods for accessing components or sending messages
		//TODO possibly we could add a smartpointer to EntityNode holding this Handle

	private:
		friend EntityMgr;

		EntityHandle(uint32 ID): mEntityID(ID) {}
		EntityHandle CreateUniqueHandle();

		uint32 mEntityID;
		static uint32 sLastID;
	};

	uint32 EntityHandle::sLastID = 0;
}

#endif
