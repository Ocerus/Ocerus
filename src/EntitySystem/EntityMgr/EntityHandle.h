#ifndef _ENTITYHANDLE_H_
#define _ENTITYHANDLE_H_

#include "../../Utility/Settings.h"
#include "EntityMessage.h"
#include "EntityEnums.h"

class PropertyList;

namespace EntitySystem
{

	typedef uint32 EntityID;

	/// @name  Forward declarations.
	//@{
	class EntityMgr;
	//@}

	/** This class represents one unique entity in the entity system.
	*/
	class EntityHandle
	{
	public:
		/// @name Default ctor will init the handle to an invalid state
		EntityHandle(void);
		/// @name Only copy ctor is enabled. We want new entities to be added only by EntityMgr.
		EntityHandle(const EntityHandle& handle);
		~EntityHandle(void) {}

		/// @name Enables assigning handles.
		EntityHandle& operator=(const EntityHandle& rhs);

		/// @name Enables comparing handles.
		bool operator==(const EntityHandle& rhs);

		/// @name Returns true if this handle is valid (not null).
		bool IsValid(void) const { return mEntityID != 0; }

		/// @name Sets this handle to invalid state.
		void Invalidate(void) { mEntityID = 0; }

		/// @name Finishes initialization of this entity. Must be called once only!
		void FinishInit(void);

		/// @name Retrieves properties of this entity.
		bool GetProperties(PropertyList& out, uint8 mask = 0xff);

		/// @name Sends a message to this entity.
		EntityMessage::eResult PostMessage(const EntityMessage::eType type, void* data = 0);

		/// @name Returns type of this entity.
		eEntityType GetType(void) const;

		static const EntityHandle Null;

	private:
		friend class ComponentMgr;
		friend class EntityMgr;

		/// @name New entities can be created only by the EntityMgr.
		//@{
		EntityHandle(EntityID ID): mEntityID(ID) {}
		static EntityHandle CreateUniqueHandle();
		//@}

		/// @name Getter
		EntityID GetID(void) const;
		/// @name ID identifying this entity.
		EntityID mEntityID;
		/// @name Last ID which was assigned to an entity.
		static EntityID sLastID;
	};
}

#endif
