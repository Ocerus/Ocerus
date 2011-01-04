/// Generic parameters passed to a message or a function accessed via the
/// properties system. Thanks to the << operators the function parameter can
/// be passed as
/// @code
/// PropertyFunctionParameters() << param1 << param2 ....
/// @endcode
/// This class is registered as a value type.
class PropertyFunctionParameters
{
public:

	/// Constructs empty parameters.
	PropertyFunctionParameters();

	/// Assignment operator.
	PropertyFunctionParameters& operator=(const PropertyFunctionParameters& toCopy);

	/// Returns true if the parameters are the same as the given parameters.
	bool operator==(const PropertyFunctionParameters& toCompare) const;

#define PFP_METHODS(TYPE) \
	/** Add new parameter of the TYPE type and return the new parameters list. */ \
	PropertyFunctionParameters operator<<(const TYPE& toAdd); \
	\
	/** Add new parameter of the array_##TYPE type and return the new parameters list. */ \
	PropertyFunctionParameters operator<<(const array_##TYPE& toAdd);


	PFP_METHODS(bool);
	PFP_METHODS(int8);
	PFP_METHODS(int16);
	PFP_METHODS(int32);
	PFP_METHODS(int64);
	PFP_METHODS(uint8);
	PFP_METHODS(uint16);
	PFP_METHODS(uint32);
	PFP_METHODS(uint64);
	PFP_METHODS(float32);
	PFP_METHODS(Vector2);
	PFP_METHODS(Point);
	PFP_METHODS(string);
	PFP_METHODS(StringKey);
	PFP_METHODS(EntityHandle);
	PFP_METHODS(Color);
	PFP_METHODS(eKeyCode);

};