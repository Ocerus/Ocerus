/// 2D point representation. It is registered as a value type.
class Point
{
public:

	/// X coordinate.
	int32 x;

	/// Y coordinate.
	int32 y;

	/// Default constructor nulls the point.
	Point();

	/// Copy constructor.
	Point(const Point& toCopy);

	/// Constructs the point from the given coordinates.
	Point(int32 x, int32 y);

	/// Returns the opposite point.
	Point operator-() const;

	/// Sets new coordinates to this point.
	void Set(int32 x, int32 y);
};