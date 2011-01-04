/// This class serves as a key into maps and other structures where we want to
/// index data using strings, but we need high speed as well. The string value is
/// hashed and the result is then used as a decimal representation of the string.
/// This class is registered as a value type.
/// @see array_StringKey
class StringKey
{
public:

	/// Default constructor.
	StringKey();

	/// Copy constructor. Creates new instance of StringKey with the data copied from toCopy.
	StringKey(const StringKey& toCopy);

	/// Constructs the key from a standard string.
	StringKey(const string& str);

	/// Assignment operator. Copies data into the current instance.
	StringKey& operator=(const StringKey& toCopy);

	/// Returns true if the given key is equal to this key.
	bool operator==(const StringKey& toCompare) const;

	/// Converts the key to string.
	string ToString() const;

};
