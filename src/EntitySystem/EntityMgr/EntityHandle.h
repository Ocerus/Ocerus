#ifndef _ENTITYHANDLE_H_
#define _ENTITYHANDLE_H_

#include "../../Utility/Settings.h"
#include "EntityMessage.h"
#include "EntityEnums.h"

class PropertyList;

namespace EntitySystem
{

	typedef uint32 EntityID;

	/// @name Forward declarations.
	//@{
	class EntityMgr;
	//@}

	/** This class represents one unique entity in the entity system.
	*/
	class EntityHandle
	{
	public:
		/// Default ctor will init the handle to an invalid state
		EntityHandle(void);
		/// Only copy ctor is enabled. We want new entities to be added only by EntityMgr.
		EntityHandle(const EntityHandle& handle);
		~EntityHandle(void) {}

		/// Enables assigning handles.
		EntityHandle& operator=(const EntityHandle& rhs);

		/// Enables comparing handles.
		bool operator==(const EntityHandle& rhs);

		/// Returns true if this handle is valid (not null).
		bool IsValid(void) const { return mEntityID != 0; }

		/// Sets this handle to invalid state.
		void Invalidate(void) { mEntityID = 0; }

		/// Finishes initialization of this entity. Must be called once only!
		void FinishInit(void);

		/// Retrieves properties of this entity.
		bool GetProperties(PropertyList& out, uint8 mask = 0xff);

		/// Sends a message to this entity.
		EntityMessage::eResult PostMessage(const EntityMessage::eType type, void* data = 0);

		/// Returns type of this entity.
		eEntityType GetType(void) const;

		static const EntityHandle Null;

	private:
		friend class ComponentMgr;
		friend class EntityMgr;

		/// New entities can be created only by the EntityMgr.
		//@{
		EntityHandle(EntityID ID): mEntityID(ID) {}
		static EntityHandle CreateUniqueHandle();
		//@}

		/// Getter
		EntityID GetID(void) const;
		/// ID identifying this entity.
		EntityID mEntityID;
		/// Last ID which was assigned to an entity.
		static EntityID sLastID;
	};
}

#endif
