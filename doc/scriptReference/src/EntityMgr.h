/// Mangement of all game entities. It is registered as a no-handle reference. So, the only
/// way you can access it is to use the ::gEntityMgr global variable.
class EntityMgr
{
public:

	/// Creates new entity as described in the given description. Returns its handle.
	EntityHandle CreateEntity(const EntityDescription& description);

	/// Creates new entity from the given prototype. The new entity will have the given name. Returns its handle.
	EntityHandle InstantiatePrototype(EntityHandle prototype, const string& name);

	/// Duplicates the given entity. The new entity will have the given name. Returns its handle.
	EntityHandle DuplicateEntity(EntityHandle toDuplicate, const string& name);

	/// Destroys the given entity if it exists.
	void DestroyEntity(EntityHandle toDestroy);

	/// Returns true if the entity exists.
	bool EntityExists(EntityHandle entity) const;

	/// Returns the handle to the first entity of the given name.
	EntityHandle FindFirstEntity(const string& name);

	/// Returns the handle to the entity of the given ID.
	EntityHandle GetEntity(EntityID id) const;

	/// Returns true if the entity is completely initialized and is ready to use.
	bool IsEntityInited(EntityHandle toTest) const;

	/// Returns true if the entity is a prototype.
	bool IsEntityPrototype(EntityHandle toTest) const;

	/// Assigns the given entity to the given prototype.
	void LinkEntityToPrototype(EntityHandle entity, EntityHandle prototype);

	/// Destroys the link between the entity and its prototype.
	void UnlinkEntityFromPrototype(EntityHandle toUnlink);

	/// Returns true if the property of the given prototype is marked as shared.
	bool IsPrototypePropertyShared(EntityHandle prototype, StringKey property) const;

	/// Marks the property as shared among instances of the prototype.
	void SetPrototypePropertyShared(EntityHandle prototype, StringKey property);

	/// Marks the property as non-shared among instances of the prototype.
	void SetPrototypePropertyNonShared(EntityHandle prototype, StringKey property);

	/// Propagates the current state of properties of the prototype into its instances.
	void UpdatePrototypeInstances(EntityHandle prototype);

	/// Returns true if the entity has the given property. It considers only properties
	/// with the access flags from the given mask.
	bool HasEntityProperty(EntityHandle entity, StringKey property, PropertyAccessFlags accessMask) const;

	/// Returns true if the component of the given entity has the given property. It considers
	/// only properties with the access flags from the given mask.
	bool HasEntityComponentProperty(EntityHandle entity, ComponentID component, StringKey property, PropertyAccessFlags accessMask) const;

	/// Sends a message to all entities.
	void BroadcastMessage(eEntityMessageType messageType, PropertyFunctionParameters messageParameters = PropertyFunctionParameters());

	/// Returns true if the given entity has a component of the given type.
	bool HasEntityComponentOfType(EntityHandle entity, eComponentType componentType);

	/// Returns the number of components attached to the given entity.
	int32 GetNumberOfEntityComponents(EntityHandle entity) const;

	/// Adds a component of the specified type to the entity. Returns ID of the new component.
	ComponentID AddComponentToEntity(EntityHandle entity, eComponentType componentType);

	/// Destroy the given component of the given entity.
	void DestroyEntityComponent(EntityHandle entity, ComponentID component);

};