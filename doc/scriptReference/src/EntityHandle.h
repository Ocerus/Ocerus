/// This class represents one unique entity in the entity system.
/// It provides most of the means of communication with the entity it represents. But basically it just calls
/// EntityMgr. So for the full list it's better to look there.
class EntityHandle
{
public:

	/// Default constructor will initialize the handle to an invalid state.
	EntityHandle();

	/// Only the copy constructor is enabled. New entities should be added only by the EntityMgr.
	EntityHandle(const EntityHandle& toCopy);

	/// Assignment operator.
	EntityHandle& operator=(const EntityHandle& toCopy);

	/// Returns true if the given handle is the same as this handle.
	bool operator==(const EntityHandle& toCompare) const;

	/// Returns true if this handle is valid (not null).
	bool IsValid() const;

	/// Returns true if this entity still exists in the system.
	bool Exists() const;

	/// Returns the internal ID of this entity.
	EntityID GetID() const;

	/// Returns the name of this entity.
	string GetName() const;

	/// Returns the tag of this entity.
	EntityTag GetTag() const;

	/// Sets the tag of this entity.
	void SetTag(EntityTag toSet);

	/// Sends a message to this entity.
	eEntityMessageResult PostMessage(eEntityMessageType type, PropertyFunctionParameters = PropertyFunctionParameters());

	/// Calls a function on this entity with the given parameters.
	void CallFunction(string& functionName, PropertyFunctionParameters parameters);

	/// @name Property Manipulation
	//@{
	/// Unregisters existing property of the given name. Returns true if successful.
	bool UnregisterDynamicProperty(const string& propertyName);
	//@}

#define EH_METHODS(TYPE) \
	/** @name Property Manipulation */ \
	/** @{ */ \
	/** Returns the TYPE value of an entity property. */ \
	TYPE Get_##TYPE(const string& propertyName); \
	/** Sets the TYPE value of an entity property. */ \
	void Set_##TYPE(const string& propertyName, TYPE value); \
	/** Returns the array_##TYPE value of an entity property. */ \
	array_##TYPE Get_array_##TYPE(const string& propertyName) const; \
	/** Returns the const array_##TYPE value of an entity property. */ \
	const array_##TYPE Get_const_array_##TYPE(const string& propertyName) const; \
	/** Sets the array_##TYPE value of an entity property. */ \
	void Set_array_##TYPE(const string& propertyName, array_##TYPE value); \
	/** Registers a new property of the given name of the TYPE type. Access flags control how the property
	can be accessed. Returns true if the operation was successful. */ \
	bool RegisterDynamicProperty_##TYPE(const string& propertyName, PropertyAccessFlags access, const string& description); \
	/** @} */


	EH_METHODS(bool);
	EH_METHODS(int8);
	EH_METHODS(int16);
	EH_METHODS(int32);
	EH_METHODS(int64);
	EH_METHODS(uint8);
	EH_METHODS(uint16);
	EH_METHODS(uint32);
	EH_METHODS(uint64);
	EH_METHODS(float32);
	EH_METHODS(Vector2);
	EH_METHODS(Point);
	EH_METHODS(string);
	EH_METHODS(StringKey);
	EH_METHODS(EntityHandle);
	EH_METHODS(Color);
	EH_METHODS(eKeyCode);

};