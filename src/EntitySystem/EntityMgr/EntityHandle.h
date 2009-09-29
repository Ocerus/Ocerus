/// @file
/// Represents an entity in the system.

#ifndef _ENTITYHANDLE_H_
#define _ENTITYHANDLE_H_

#include "Settings.h"
#include "StringKey.h"
#include "../../Utility/Properties/PropertyHolderMediator.h"
#include "EntityMessage.h"
#include "EntityEnums.h"

class PropertyList;

namespace EntitySystem
{

	/// Entity identifier.
	typedef uint32 EntityID;

	/// Entity team identifier.
	typedef EntityID TeamID;

	class EntityMgr;

	/// This class represents one unique entity in the entity system.
	class EntityHandle
	{
	public:

		/// Default ctor will init the handle to an invalid state
		EntityHandle(void);

		/// Only the copy ctor is enabled. We want new entities to be added only by the EntityMgr.
		EntityHandle(const EntityHandle& handle);

		~EntityHandle(void);

		EntityHandle& operator=(const EntityHandle& rhs);

		bool operator==(const EntityHandle& rhs);

		bool operator!=(const EntityHandle& rhs);

		/// Returns true if this handle is valid (not null).
		bool IsValid(void) const { return mEntityID != 0; }

		/// Returns true if this entity still exists in the system.
		bool Exists(void) const;

		/// Sets this handle to an invalid state.
		void Invalidate(void) { mEntityID = 0; }

		/// Finishes the initialization of this entity. Must be called once only!
		void FinishInit(void);

		/// Retrieves properties of this entity. A filter related to properties' flags can be specified.
		bool GetProperties(PropertyList& out, const PropertyAccessFlags mask = FULL_PROPERTY_ACCESS_FLAGS);

		/// Retrieves a property of this entity. A filter related to properties' flags can be specified.
		PropertyHolderMediator GetProperty(const StringKey key, const PropertyAccessFlags mask = FULL_PROPERTY_ACCESS_FLAGS) const;

		/// Sends a message to this entity.
		EntityMessage::eResult PostMessage(const EntityMessage::eType type, void* data = 0);

		/// Returns the type of this entity.
		eEntityType GetType(void) const;

		/// Returns the team this entity belongs to.
		TeamID GetTeam(void) const;

		/// Changes the team this entity belongs to.
		void SetTeam(const EntityHandle teamOwner);

		/// Changes the team this entity belongs to.
		void SetTeam(const TeamID team);

		/// Invalid handle representing no entity in the system.
		static const EntityHandle Null;

	private:

		static EntityID sLastID;

		EntityID mEntityID;

		friend class ComponentMgr;
		friend class EntityMgr;

		/// New entities can be created only by the EntityMgr, that's why it's private.
		EntityHandle(EntityID ID): mEntityID(ID) {}

		static EntityHandle CreateUniqueHandle();

		static EntityID GetMaxID(void) { return sLastID; }

		EntityID GetID(void) const;
	};
}

#endif
