/// @file
/// Entry point to the Entity system.

#ifndef _ENTITYMGR_H_
#define _ENTITYMGR_H_

#include "Base.h"
#include "Singleton.h"
#include "EntityHandle.h"
#include "EntityEnums.h"
#include "../ComponentMgr/ComponentEnums.h"
#include "../ComponentMgr/ComponentID.h"
#include "Properties/PropertyAccess.h"

/// Macro for easier use.
#define gEntityMgr EntitySystem::EntityMgr::GetSingleton()

namespace EntitySystem
{
	/// A list of types of components.
	typedef vector<eComponentType> ComponentTypeList;

	// Forward declaration of internal structs.
	struct EntityInfo;
	struct PrototypeInfo;

	/// This class manages all game entities like weapons, enemy ships, projectiles, etc. Manipulation of entities is
	///	done via entity handles.
	class EntityMgr : public Singleton<EntityMgr>
	{
	public:

		/// Default constructor.
		EntityMgr(void);

		/// Default destructor.
		~EntityMgr(void);

		/// Creates new entity accorindgly to its description and returns its handle.
		EntityHandle CreateEntity(EntityDescription& desc);

		/// Destroys a specified entity if it exists.
		void DestroyEntity(const EntityHandle h);

		/// Loads all entities from an XML resource.
		bool LoadFromResource(ResourceSystem::ResourcePtr res, const bool isPrototype = false);

		/// Posts a message to an entity.
		inline EntityMessage::eResult PostMessage(EntityHandle h, const EntityMessage& msg) { return PostMessage(h.GetID(), msg); }

		/// Sends a message to all entities.
		void BroadcastMessage(const EntityMessage& msg);

		/// Returns the type of a specified entity.
		eEntityType GetEntityType(const EntityHandle h) const;

		/// Returns true if the entity exists.
		bool EntityExists(const EntityHandle h) const;

		/// Returns true if the entity was fully initialized.
		bool IsEntityInited(const EntityHandle h) const;

		/// Returns true if the entity is a prototype.
		inline bool IsEntityPrototype(const EntityHandle h) const { return IsEntityPrototype(h.GetID()); }

		/// Returns true if the entity is a prototype.
		bool IsEntityPrototype(const EntityID id) const;

		/// Assigns the given entity to the prototype.
		void LinkEntityToPrototype(const EntityID id, const EntityID prototype);

		/// Destroys the link between the component and its prototype.
		void UnlinkEntityFromPrototype(const EntityID id);

		/// Returns true if the ID belongs to an existing entity.
		bool IsEntity(const EntityID id) const;

		/// Retrieves properties of an entity. A filter related to properties' flags can be specified.
		inline bool GetEntityProperties(const EntityHandle entity, PropertyList& out, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const { return GetEntityProperties(entity.GetID(), out, flagMask); }

		/// Retrieves properties of an entity. A filter related to properties' flags can be specified.
		bool GetEntityProperties(const EntityID entityID, PropertyList& out, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const;

		/// Returns true if the component of the entity has the given property.
		bool HasEntityComponentProperty(const EntityHandle h, const ComponentID componentID, const StringKey key, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const;

		/// Returns true if the entity has the given property.
		bool HasEntityProperty(const EntityHandle h, const StringKey key, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const;

		/// Retrieves a property of an entity. A filter related to properties' flags can be specified.
		inline PropertyHolder GetEntityProperty(const EntityHandle h, const StringKey key, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const { return GetEntityProperty(h.GetID(), key, flagMask); }

		/// Retrieves a property of an entity. A filter related to properties' flags can be specified.
		PropertyHolder GetEntityProperty(const EntityID id, const StringKey key, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const;

		/// Retrieves properties of a component of an entity. A filter related to properties' flags can be specified.
		bool GetEntityComponentProperties(const EntityHandle entity, const ComponentID component, PropertyList& out, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const;

		/// Retrieves a property of a component of an entity. A filter related to properties' flags can be specified.
		PropertyHolder GetEntityComponentProperty(const EntityHandle entity, const ComponentID component, const StringKey propertyKey, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const;

		/// Returns true if the property of the prototype is marked as shared (and thus propagated to instances).
		bool IsPrototypePropertyShared(const EntityHandle prototype, const StringKey testedProperty) const;

		/// Marks the property as shared among instances of the prototype.
		void SetPrototypePropertyShared(const EntityHandle prototype, const StringKey propertyToMark);

		/// Marks the property as non shared among instances of the prototype.
		void SetPrototypePropertyNonShared(const EntityHandle prototype, const StringKey propertyToMark);

		/// Returns EntityHandle to an entity of a specified ID. If there are more of them, then returns the first one.
		EntityHandle FindFirstEntity(const string& name);

		/// Returns true if the given entity has a component of the given type.
		bool HasEntityComponentOfType(const EntityHandle h, const eComponentType componentType);

		/// Fills the given list with types of components present in the given entity. Returns true if everything was ok.
		inline bool GetEntityComponentTypes(const EntityHandle h, ComponentTypeList& out) { return GetEntityComponentTypes(h.GetID(), out); }

		/// Fills the given list with types of components present in the given entity. Returns true if everything was ok.
		bool GetEntityComponentTypes(const EntityID id, ComponentTypeList& out);

		/// Returns the number of components attached to the entity.
		inline int32 GetNumberOfEntityComponents(const EntityHandle h) const { return GetNumberOfEntityComponents(h.GetID()); }

		/// Returns the number of components attached to the entity.
		int32 GetNumberOfEntityComponents(const EntityID id) const;

		/// Adds a component of the specified type to the entity. Returned is an ID of the new component.
		inline ComponentID AddComponentToEntity(const EntityHandle h, const eComponentType componentType) { return AddComponentToEntity(h.GetID(), componentType); }

		/// Adds a component of the specified type to the entity. Returned is an ID of the new component.
		ComponentID AddComponentToEntity(const EntityID id, const eComponentType componentType);

		/// Destroyes a component of the entity.
		inline void DestroyEntityComponent(const EntityHandle h, const ComponentID componentToDestroy) { DestroyEntityComponent(h.GetID(), componentToDestroy); }

		/// Destroyes a component of the entity.
		void DestroyEntityComponent(const EntityID id, const ComponentID componentToDestroy);

		/// Actually destroyes all entities marked for destruction.
		void ProcessDestroyQueue(void);

		/// Destroys all entities in the manager.
		void DestroyAllEntities(void);

	private:
	
		typedef hash_map<EntityID, EntityInfo*> EntityMap;
		typedef hash_map<EntityID, PrototypeInfo*> PrototypeMap;
		typedef vector<EntityID> EntityQueue;

		ComponentMgr* mComponentMgr;
		EntityMap mEntities;
		PrototypeMap mPrototypes;
		EntityQueue mEntityDestroyQueue;

		/// Posts a message to an entity. It is the only way entities can communicate with each other apart from the properties.
		EntityMessage::eResult PostMessage(EntityID id, const EntityMessage& msg);

		/// Destructs an entity completely and removes it from the system.
		/// @param erase If set to true, the entity will be removed from the entity map as well.
		void DestroyEntityImmediately(const EntityID id, const bool erase);
		
		/// Saves the current properties into the storage of shared properties of the prototype.
		void UpdatePrototypeCopy(const EntityID prototype);

		/// Propagates the current state of properties of the prototype to the specified instances.
		void UpdatePrototypeInstance(const EntityID prototype, const EntityID instance, const bool forceOverwrite);

		/// Propagates the current state of properties of the prototype to its instances.
		void UpdatePrototypeInstances(const EntityID prototype);

		/// Load an entity from the XML file given a node iterator to its node.
		void LoadEntityFromXML( ResourceSystem::XMLNodeIterator &entIt, const bool isPrototype, ResourceSystem::XMLResourcePtr xml );

		/// Load a property for the given entity from a XML file.
		void LoadEntityPropertyFromXML( const EntityID entityID, const ComponentID componentID, PrototypeInfo* prototypeInfo, ResourceSystem::XMLResourcePtr xml, ResourceSystem::XMLNodeIterator& xmlPropertyIterator );
	};
}

#endif