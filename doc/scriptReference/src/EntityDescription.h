/// Info needed to create one instance of an entity. It is basically a collection of component descriptions.
/// This class is registered as a value type.
class EntityDescription
{
public:

	/// Default constructor.
	EntityDescription();

	/// Clears everything. Call this before each subsequent filling of the description.
	void Reset();

	/// Adds a new component specified by its type.
	void AddComponent(eComponentType type);

	/// Sets custom name for this entity.
	void SetName(const string& name);

	/// Sets this entity to be an ordinary entity or a prototype.
	void SetKind(eEntityDescriptionKind kind);

	/// Sets the prototype the entity is to be linked to.
	void SetPrototype(EntityHandle prototype);

	/// Sets the prototype the entity is to be linked to.
	void SetPrototype(EntityID prototype);

	/// Sets a desired ID for this entity. This is done automatically unless you overwrite it.
	void SetDesiredID(EntityID);

};