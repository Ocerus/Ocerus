#ifndef _ENTITYHANDLE_H_
#define _ENTITYHANDLE_H_

#include "../../Utility/Settings.h"
#include "../../Utility/StringKey.h"
#include "../../Utility/Properties/PropertyHolderMediator.h"
#include "EntityMessage.h"
#include "EntityEnums.h"

/// @name Forward declarations.
//@{
class PropertyList;
//@}

namespace EntitySystem
{

	/// @name Entity identifier.
	typedef uint32 EntityID;
	/// @name Team identifier.
	typedef EntityID TeamID;


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
		//@{
		bool operator==(const EntityHandle& rhs);
		bool operator!=(const EntityHandle& rhs);
		//@}

		/// @name Returns true if this handle is valid (not null).
		bool IsValid(void) const { return mEntityID != 0; }

		/// @name Returns true if this entity exists.
		bool Exists(void) const;

		/// @name Sets this handle to invalid state.
		void Invalidate(void) { mEntityID = 0; }

		/// @name Finishes initialization of this entity. Must be called once only!
		void FinishInit(void);

		/// @name Retrieves properties of this entity. A filter related to properties' flags can be specified.
		bool GetProperties(PropertyList& out, const PropertyAccessFlags mask = FULL_PROPERTY_ACCESS_FLAGS);

		/// @name Retrieves a property of this entity. A filter related to properties' flags can be specified.
		PropertyHolderMediator GetProperty(const StringKey key, const PropertyAccessFlags mask = FULL_PROPERTY_ACCESS_FLAGS) const;

		/// @name Sends a message to this entity.
		EntityMessage::eResult PostMessage(const EntityMessage::eType type, void* data = 0);

		/// @name Returns type of this entity.
		eEntityType GetType(void) const;

		/// @name Rerturns team this entity belongs to.
		TeamID GetTeam(void) const;

		/// @name Changes team this entity belongs to.
		//@{
		void SetTeam(const EntityHandle teamOwner);
		void SetTeam(const TeamID team);
		//@}

		static const EntityHandle Null;

	private:
		friend class ComponentMgr;
		friend class EntityMgr;

		/// @name New entities can be created only by the EntityMgr.
		//@{
		EntityHandle(EntityID ID): mEntityID(ID) {}
		static EntityHandle CreateUniqueHandle();
		static EntityID GetMaxID(void) { return sLastID; }
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
