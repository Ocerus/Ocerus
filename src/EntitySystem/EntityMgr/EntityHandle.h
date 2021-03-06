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

	/// User defined tag assigned to entities.
	typedef uint16 EntityTag;

	/// Invalid ID for an entity.
	const EntityID INVALID_ENTITY_ID = 0;

	/// This class represents one unique entity in the entity system.
	/// It provides most of the means of communication with the entity it represents. But basically it just calls
	/// EntityMgr. So for the full list it's better to look there.
	class EntityHandle
	{
	public:

		/// Default constructor will initialize the handle to an invalid state
		EntityHandle(void);

		/// Only the copy constructor is enabled. New entities should be added only by the EntityMgr.
		/// The copy-constructor is inline, because we often pass EntityHandle by value into functions.
		inline EntityHandle(const EntityHandle& handle);

		/// Destructor.
		~EntityHandle(void);

		/// Assignment operator.
		EntityHandle& operator=(const EntityHandle& rhs);

		/// Comparison operator.
		bool operator==(const EntityHandle& rhs) const;

		/// Comparison operator.
		bool operator!=(const EntityHandle& rhs) const;

		/// Comparison operator.
		bool operator<(const EntityHandle& rhs) const;

		/// Returns true if this handle is not valid.
		inline bool operator!(void) const { return !IsValid(); }

		/// Returns true if this handle is valid (not null).
		inline bool IsValid(void) const { return mEntityID != INVALID_ENTITY_ID; }

		/// Returns true if this entity still exists in the system.
		bool Exists(void) const;

		/// Sets this handle to an invalid state.
		inline void Invalidate(void) { mEntityID = INVALID_ENTITY_ID; }

		/// Finishes the initialization of this entity. Must be called once only!
		void FinishInit(void) const;

		/// True if the entity is already initialized.
		bool IsInited(void) const;

		/// Retrieves properties of this entity. A filter related to properties' flags can be specified.
		bool GetProperties(PropertyList& out, const PropertyAccessFlags mask = PA_FULL_ACCESS) const;

		/// Retrieves a function of this entity. A filter related to the function's flags can be specified.
		PropertyHolder GetFunction(const StringKey key, const PropertyAccessFlags mask = PA_FULL_ACCESS) const;

		/// Returns true if the entity has the given property.
		bool HasProperty(const StringKey key, const PropertyAccessFlags mask = PA_FULL_ACCESS) const;
		
		/// Returns true if the component of the entity has the given property.
		bool HasComponentProperty(const ComponentID componentID, const StringKey key, const PropertyAccessFlags mask = PA_FULL_ACCESS) const;

		/// Retrieves a property of this entity. A filter related to properties' flags can be specified.
		PropertyHolder GetProperty(const StringKey key, const PropertyAccessFlags mask = PA_FULL_ACCESS) const;

		/// Retrieves a property of a component of this entity. A filter related to properties' flags can be specified.
		PropertyHolder GetComponentProperty(const ComponentID componentID, const StringKey key, const PropertyAccessFlags mask = PA_FULL_ACCESS) const;

		/// Register a dynamic property to a component of this entity.
		template <class T>
		bool RegisterDynamicPropertyOfComponent(const ComponentID component, 
			const StringKey propertyKey, const PropertyAccessFlags accessFlags, const string& comment) const;

        /// Unregister a dynamic property of a component of this entity.
		bool UnregisterDynamicPropertyOfComponent(const ComponentID component, const StringKey propertyKey) const;

		/// Sends a message to this entity.
		EntityMessage::eResult PostMessage(const EntityMessage& msg) const;

		/// Sends a message to this entity.
		inline EntityMessage::eResult PostMessage(const EntityMessage::eType type) const {  return PostMessage(EntityMessage(type, Reflection::PropertyFunctionParameters())); }

		/// Sends a message to this entity.
		inline EntityMessage::eResult PostMessage(const EntityMessage::eType type, Reflection::PropertyFunctionParameters data) const {  return PostMessage(EntityMessage(type, data)); }

		/// Returns the internal ID of this entity.
		inline EntityID GetID(void) const;

		/// Returns the name of this entity.
		string GetName(void) const;

		/// Returns the user tag of this entity.
		EntityTag GetTag(void) const;

		/// Sets the user tag of this entity.
		void SetTag(EntityTag tag);

		/// Invalid handle representing no entity in the system.
		static const EntityHandle Null;

		/// New entities can be created only by the EntityMgr, that's why it's private.
		/// We need to have this public for loading scene.
		EntityHandle(EntityID ID): mEntityID(ID) {}

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
	};

	inline EntityHandle::EntityHandle(const EntityHandle& handle): mEntityID(handle.mEntityID) {}

	inline EntityID EntityHandle::GetID(void) const
	{
		OC_DASSERT_MSG(IsValid(), "Invalid entity handle");
		return mEntityID;
	}
}
#endif
