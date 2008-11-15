#ifndef _ENTITYHANDLE_H_
#define _ENTITYHANDLE_H_

#include "../../Utility/Settings.h"

namespace EntitySystem
{

	typedef uint32 EntityID;

	/// Forward declaration.
	class EntityMgr;

	/** This class represents one unique entity in the entity system.
	*/
	class EntityHandle
	{
	public:
		/// Only copy ctor is enabled. We want new entities to be added only by EntityMgr.
		EntityHandle(const EntityHandle& handle) { mEntityID = handle.mEntityID; }
		~EntityHandle(void) {}

		/// Enables comparing handles.
		EntityHandle& operator=(const EntityHandle& rhs);

		//TODO possibility to add helper methods for accessing components or sending messages
		//TODO possibly we could add a smartpointer to EntityNode holding this Handle

	private:
		friend class ComponentMgr;
		friend class EntityMgr;

		/// New entities can be created only by the EntityMgr.
		//@{
		EntityHandle(EntityID ID): mEntityID(ID) {}
		static EntityHandle CreateUniqueHandle();
		//@}

		/// ID identifying this entity.
		EntityID mEntityID;
		/// Last ID which was assigned to an entity.
		static EntityID sLastID;
	};
}

#endif
