/// @file
/// Represents an entity in the system.

#ifndef _ENTITYHANDLE_H_
#define _ENTITYHANDLE_H_

#include "Base.h"
#include "EntityMessage.h"
#include "Properties/PropertyAccess.h"
#include "../ComponentMgr/ComponentID.h"

namespace EntitySystem
{
	/// Entity identifier. Positive IDs determine actual entities while negative IDs determine entity prototypes.
	typedef int32 EntityID;

	/// Invalid ID for an entity.
	const EntityID INVALID_ENTITY_ID = 0;

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
		bool IsValid(void) const { return mEntityID != INVALID_ENTITY_ID; }

		/// Returns true if this entity still exists in the system.
		bool Exists(void) const;

		/// Sets this handle to an invalid state.
		void Invalidate(void) { mEntityID = INVALID_ENTITY_ID; }

		/// Finishes the initialization of this entity. Must be called once only!
		void FinishInit(void);

		/// Retrieves properties of this entity. A filter related to properties' flags can be specified.
		bool GetProperties(PropertyList& out, const PropertyAccessFlags mask = PA_FULL_ACCESS);

		/// Retrieves a function of this entity. A filter related to the function's flags can be specified.
		PropertyHolder GetFunction(const StringKey key, const PropertyAccessFlags mask = PA_FULL_ACCESS) const;

		/// Retrieves a property of this entity. A filter related to properties' flags can be specified.
		PropertyHolder GetProperty(const StringKey key, const PropertyAccessFlags mask = PA_FULL_ACCESS) const;

		/// Retrieves a property of a component of this entity. A filter related to properties' flags can be specified.
		PropertyHolder GetComponentProperty(const ComponentID componentID, const StringKey key, const PropertyAccessFlags mask = PA_FULL_ACCESS) const;

		/// Sends a message to this entity.
		EntityMessage::eResult PostMessage(const EntityMessage& msg);

		/// Sends a message to this entity.
		inline EntityMessage::eResult PostMessage(const EntityMessage::eType type) {  return PostMessage(EntityMessage(type, Reflection::PropertyFunctionParameters())); }

		/// Sends a message to this entity.
		inline EntityMessage::eResult PostMessage(const EntityMessage::eType type, Reflection::PropertyFunctionParameters data) {  return PostMessage(EntityMessage(type, data)); }

		/// Returns the internal ID of this entity.
		inline EntityID GetID(void) const;

		/// Invalid handle representing no entity in the system.
		static const EntityHandle Null;

	private:

		friend class EntityMgr;
		friend class ComponentMgr;

		EntityID mEntityID;

		static EntityID sLastFreeID;
		static EntityHandle CreateUniqueHandle();
		static EntityHandle CreateUniquePrototypeHandle();
		static EntityHandle CreateHandleFromID(const EntityID id);
		static bool IsPrototypeID(const EntityID id);
		static EntityID GetMaxID(void) { return sLastFreeID; }
		static void IncID(EntityID& id);
		static void DecID(EntityID& id);

		/// New entities can be created only by the EntityMgr, that's why it's private.
		EntityHandle(EntityID ID): mEntityID(ID) {}
	};


	inline EntityID EntityHandle::GetID(void) const
	{
		OC_DASSERT_MSG(IsValid(), "Invalid entity handle");
		return mEntityID;
	}

}

#endif
