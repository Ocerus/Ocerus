/// @file
/// Entry point to the Entity system.

#ifndef _ENTITYMGR_H_
#define _ENTITYMGR_H_

#include "Base.h"
#include "Singleton.h"
#include "EntityHandle.h"
#include "../ComponentMgr/ComponentEnums.h"
#include "../ComponentMgr/ComponentID.h"
#include "../ComponentMgr/Component.h"
#include "../ComponentMgr/ComponentMgr.h"
#include "Properties/PropertyAccess.h"
#include "ResourceSystem/XMLOutput.h"

/// Macro for easier use.
#define gEntityMgr EntitySystem::EntityMgr::GetSingleton()

/// Entity system manages game entities and their parts.
/// It takes care of their logical representation and leaves optimization of other tasks to other parts of the system.
/// For example, the rendering system can use an optimization data structure for faster visibility culling of entities.
/// The main class is EntityMgr which provides the basic interface of the communication with entities as well as reading
/// their attributes.
namespace EntitySystem
{
	/// A list of types of components.
	typedef vector<eComponentType> ComponentTypeList;

	/// A list of IDs of components.
	typedef vector<ComponentID> ComponentIdList;

	/// A list of entity handles.
	typedef vector<EntityHandle> EntityList;

	// Forward declaration of internal structs. Don't move them to Forwards.h.
	struct EntityInfo;
	struct PrototypeInfo;

	/// This class manages all game entities like weapons, enemy ships, projectiles, etc.
	/// Manipulation with entities is done via entity handles. It provides the interface of the communication with
	/// entities as well as their components.
	class EntityMgr : public Singleton<EntityMgr>
	{
	public:

		/// Default constructor.
		EntityMgr(void);

		/// Default destructor.
		~EntityMgr(void);


		/// @name Entity manipulation
		//@{

		/// Creates new entity accordingly to its description and returns its handle.
		EntityHandle CreateEntity(EntityDescription& desc);
		
		/// Creates new entity from prototype.
		EntityHandle InstantiatePrototype(const EntityHandle prototype, const Vector2& position = Vector2_Zero, const string& newName = "");
		
		/// Duplicates the entity with a new name.
		/// If a name is not specified, the old name is used.
		EntityHandle DuplicateEntity(const EntityHandle oldEntity, const string& newName = "");

		/// Destroys a specified entity if it exists.
		void DestroyEntity(const EntityHandle entityToDestroy);

		/// Returns true if the entity exists.
		bool EntityExists(const EntityHandle h) const;

		/// Returns EntityHandle to the first entity of a specified name.
		EntityHandle FindFirstEntity(const string& name);

		/// Returns EntityHandle to the entity with specified id.
		EntityHandle GetEntity(EntityID id) const;

		/// Actually destroyes all entities marked for destruction.
		void ProcessDestroyQueue(void);

		/// Destroys all entities in the manager.
		/// @param includingPrototypes If true even the prototypes are deleted.
		/// @param deleteTransients If true, deletes also transient entities.
		void DestroyAllEntities(bool includingPrototypes, bool deleteTransients);

		/// Retrieves handles of entities that have a specific component.
		void GetEntitiesWithComponent(EntityList& out, const eComponentType componentType, bool prototypes = false);

		/// Retrieves handles of all entities.
		void GetEntities(EntityList& out, bool prototypes = false);

		/// Returns the number of non-trasient non-prototype entities.
		size_t GetNumberOfNonTransientEntities() const;

		//@}


		/// @name Entity persistance
		//@{
		
		/// Loads all entities from a XML resource.
		bool LoadEntitiesFromResource(ResourceSystem::ResourcePtr res, const bool isPrototype = false);

		/// Saves all entities to a XML stream.
		bool SaveEntitiesToStorage(ResourceSystem::XMLOutput& storage, const bool isPrototype = false, const bool evenTransient = false) const;

		/// Loads all prototypes from the default file.
		bool LoadPrototypes();

		/// Saves all prototypes into the default file.
		bool SavePrototypes();

		//@}


		/// @name Entity attributes
		//@{

		/// Returns name of the entity.
		string GetEntityName(const EntityHandle& h) const;

		/// Sets name of the entity.
		void SetEntityName(const EntityHandle& h, const string& entityName);

		/// Returns true if the entity was fully initialized.
		bool IsEntityInited(const EntityHandle h) const;
		
		/// Returns whether the entity should not be saved.
		bool IsEntityTransient(const EntityHandle& h) const;
		
		/// Sets whether the entity should not be saved.
		void SetEntityTransient(const EntityHandle& h, const bool transient);

		/// User defined tag assigned to the entity.
		EntityTag GetEntityTag(const EntityHandle& h) const;

		/// User defined tag assigned to the entity.
		void SetEntityTag(const EntityHandle& h, const EntityTag tag);

		//@}



		/// @name Prototypes
		//@{

		/// Returns true if the entity is a prototype.
		bool IsEntityPrototype(const EntityHandle entity) const;

		/// Returns true if the entity is linked to a prototype.
		bool IsEntityLinkedToPrototype(const EntityHandle entity) const;

		/// Returns the prototype the entity is linked to.
		EntityHandle GetEntityPrototype(const EntityHandle entity);

		/// Returns EntityHandle to the first prototype of the specified name.
		EntityHandle FindFirstPrototype(const string& name);

		/// Assigns the given entity to the prototype.
		void LinkEntityToPrototype(const EntityHandle entity, const EntityHandle prototype);

		/// Destroys the link between the component and its prototype.
		void UnlinkEntityFromPrototype(const EntityHandle entity);

		/// Returns true if the property of the prototype is marked as shared (and thus propagated to instances).
		bool IsPrototypePropertyShared(const EntityHandle prototype, const StringKey testedProperty) const;

		/// Marks the property as shared among instances of the prototype.
		void SetPrototypePropertyShared(const EntityHandle prototype, const StringKey propertyToMark);

		/// Marks the property as non shared among instances of the prototype.
		void SetPrototypePropertyNonShared(const EntityHandle prototype, const StringKey propertyToMark);

		/// Propagates the current state of properties of the prototype to its instances.
		void UpdatePrototypeInstances(const EntityHandle prototype);

		/// Create a prototype from the entity. Returns the handle to the prototype or null if there was error.
		EntityHandle ExportEntityToPrototype(const EntityHandle entity);

		/// Retrieves handles of all entity prototypes.
		void GetPrototypes(EntityList& out);

		//@}



		/// @name Properties
		//@{

		/// Retrieves properties of an entity. A filter related to properties' flags can be specified.
		bool GetEntityProperties(const EntityHandle entity, PropertyList& out, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const;

		/// Retrieves properties of a component of an entity. A filter related to properties' flags can be specified.
		bool GetEntityComponentProperties(const EntityHandle entity, const ComponentID component, PropertyList& out, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const;

		/// Returns true if the entity has the given property.
		bool HasEntityProperty(const EntityHandle entity, const StringKey key, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const;

		/// Returns true if the component of the entity has the given property.
		bool HasEntityComponentProperty(const EntityHandle entity, const ComponentID componentID, const StringKey key, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const;

		/// Retrieves a property of an entity. A filter related to properties' flags can be specified.
		PropertyHolder GetEntityProperty(const EntityHandle entity, const StringKey key, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const;

		/// Retrieves a property of a component of an entity. A filter related to properties' flags can be specified.
		PropertyHolder GetEntityComponentProperty(const EntityHandle entity, const ComponentID component, const StringKey propertyKey, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const;

		/// Register a dynamic property to a component of an entity.
		template <class T>
		bool RegisterDynamicPropertyOfEntityComponent(const EntityHandle entity, const ComponentID component,
			const StringKey propertyKey, const PropertyAccessFlags accessFlags, const string& comment);

		/// Register a dynamic property to a component of an entity.
		bool RegisterDynamicPropertyOfEntityComponent(Reflection::ePropertyType propertyType, 
			const EntityHandle entity, const ComponentID component, const StringKey propertyKey,
			const PropertyAccessFlags accessFlags, const string& comment);

		/// Unregister a dynamic property of a component of an entity.
		bool UnregisterDynamicPropertyOfEntityComponent(const EntityHandle entity, const ComponentID component,
			const StringKey propertyKey);

		//@}



		/// @name Messages
		//@{

		/// Posts a message to an entity.
		inline EntityMessage::eResult PostMessage(EntityHandle h, const EntityMessage& msg) { return PostMessage(h.GetID(), msg); }

		/// Sends a message to all entities.
		void BroadcastMessage(const EntityMessage& msg);

		/// Sends a message to all entities.
		inline void BroadcastMessage(const EntityMessage::eType type) {  BroadcastMessage(EntityMessage(type, Reflection::PropertyFunctionParameters())); }

		/// Sends a message to all entities.
		inline void BroadcastMessage(const EntityMessage::eType type, Reflection::PropertyFunctionParameters data) {  BroadcastMessage(EntityMessage(type, data)); }

		//@}



		/// @name Components
		//@{

		/// Returns true if the given entity has a component of the given type.
		bool HasEntityComponentOfType(const EntityHandle entity, const eComponentType componentType);

		/// Returns the type of the given component in the given entity, or CT_INVALID if component is not found.
		eComponentType GetEntityComponentType(const EntityHandle entity, const ComponentID componentID);

		/// Fills the given list with types of components present in the given entity. Returns true if everything was ok.
		bool GetEntityComponentTypes(const EntityHandle entity, ComponentTypeList& out);

		/// Fills the given list with IDs of component of the given type in the entity. Returns true if everything was ok.
		bool GetEntityComponentsOfType(const EntityHandle entity, const eComponentType desiredType, ComponentIdList& out);

		/// Returns the number of components attached to the entity.
		int32 GetNumberOfEntityComponents(const EntityHandle entity) const;

		/// Adds a component of the specified type to the entity. Returned is an ID of the new component.
		ComponentID AddComponentToEntity(const EntityHandle entity, const eComponentType componentType);

		/// Returns true if the component can be destroyed. For example, it could violate dependencies among components.
		bool CanDestroyEntityComponent(const EntityHandle entity, const ComponentID componentToDestroy);

		/// Destroys a component of the entity.
		void DestroyEntityComponent(const EntityHandle entity, const ComponentID componentToDestroy);

		/// Returns an ID of the first entity component of a specific type or -1 if does not exist.
		ComponentID GetEntityComponent(const EntityHandle entity, const eComponentType type);

		/// Returns a pointer to the first component of the given type. Returns null if no such exists.
		Component* GetEntityComponentPtr(const EntityHandle entity, const eComponentType type);

		/// Returns a pointer to the specified component. Returns null if no such exists.
		Component* GetEntityComponentPtr(const EntityHandle entity, const ComponentID id);

		//@}


	private:

		typedef hash_map<EntityID, EntityInfo*> EntityMap;
		typedef hash_map<EntityID, PrototypeInfo*> PrototypeMap;
		typedef vector<EntityID> EntityQueue;

		ComponentMgr* mComponentMgr;
		EntityMap mEntities;
		PrototypeMap mPrototypes;
		EntityQueue mEntityDestroyQueue;

		/// Posts a message to an entity. It is the only way entities can communicate with each other apart from the properties.
		EntityMessage::eResult PostMessage(EntityID targetEntity, const EntityMessage& msg);

		/// Destructs an entity completely and removes it from the system.
		/// @param erase If set to true, the entity will be removed from the entity map as well.
		void DestroyEntityImmediately(const EntityID entityToDestroy, const bool erase);

		/// Propagates the current state of properties of the prototype to the specified instances.
		void UpdatePrototypeInstance(const EntityID prototype, const EntityID instance);

		/// Marks all (appliable) properties of the prototype component as shared.
		void MarkPrototypePropertiesShared(const EntityHandle entity, ComponentID cid);

		/// Returns true if the property can be marked at shared when the prototype is created.
		bool IsPrototypePropertyAppliableToBeShared(const PropertyHolder prop) const;

		/// Load an entity from the XML file given a node iterator to its node.
		void LoadEntityFromXML(ResourceSystem::XMLNodeIterator& entIt, const bool isPrototype);

		/// Load a property for the given entity from a XML file.
		void LoadEntityPropertyFromXML(const EntityID entityID, const ComponentID componentID, PrototypeInfo* prototypeInfo, ResourceSystem::XMLNodeIterator& xmlPropertyIterator);

		/// Save and entity to the XML file.
		bool SaveEntityToStorage(const EntityID entityID, ResourceSystem::XMLOutput& storage, const bool isPrototype, const bool evenTransient) const;
	};
}


//-----------------------------------------------------------------------------
// Implementation

template <class T>
bool EntitySystem::EntityMgr::RegisterDynamicPropertyOfEntityComponent( const EntityHandle entity, const ComponentID component, const StringKey propertyKey, const PropertyAccessFlags accessFlags, const string& comment )
{
	Component* cmp = mComponentMgr->GetEntityComponent(entity.GetID(), component);
	return cmp->RegisterDynamicProperty<T>(propertyKey, accessFlags, comment);
}


#endif