/// Represents 32-bit color. It is registered as a value type.
class Color
{
public:

	/// Red component of the color.
	uint8 r;

	/// Green component of the color.
	uint8 g;

	/// Blue component of the color.
	uint8 b;

	/// Alpha component of the color.
	uint8 a;

	/// Default constructor creates dark opaque color.
	Color();

	/// Creates the color from the given components.
	Color(uint8 r, uint8 g, uint8 b, uint8 a = 255);

	/// Creates the color from a 32-bit representation.
	Color(uint32 color);

	/// Returns true if the colors are the same.
	bool operator==(const Color& toCompare) const;

	/// Returns the 32-bit representation of the color.
	uint32 GetARGB() const;
};