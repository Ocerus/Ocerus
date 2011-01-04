/// 2D vector representation. It is registered as a value type.
class Vector2
{
public:

	/// X coordinate.
	float32 x;

	/// Y coordinate.
	float32 y;

	/// Default constructor nulls the vector.
	Vector2();

	/// Copy constructor.
	Vector2(const Vector2& toCopy);

	/// Constructs the vector with the given coordinate.
	Vector2(float32 x, float32 y);

	/// Adds a vector to this vector.
	void operator+=(const Vector2& toAdd);

	/// Subtracts a vector from this vector.
	void operator-=(const Vector2& toSubtract);

	/// Multiplies this vector by a scalar.
	void operator*=(float32 scalar);

	/// Negates the vector and returns the result. This vector is not changed.
	Vector2 operator-() const;

	/// Returns true if the vectors are equal.
	bool operator==(const Vector2& toCompare) const;

	/// Returns the sum of the vectors.
	Vector2 operator+(const Vector2& summand) const;

	/// Returns the difference between the vectors.
	Vector2 operator-(const Vector2& subtrahend) const;

	/// Returns the product of a scalar and this vector.
	Vector2 operator*(float32 scalar) const;

	/// Returns the length of this vector (the norm).
	float32 Length() const;

	/// Returns the length squared of this vector.
	float32 LengthSquared() const;

	/// Sets the given coordinates to this vector.
	void Set(float32 x, float32 y);

	/// Nulls this vector.
	void SetZero();

	/// Converts this vector into a unit vector and returns the length it had previously.
	float32 Normalize();

	/// Returns true if this vector contains finite coordinates.
	bool IsValid() const;

	/// Returns the scalar product of this and the given vectors. This vector is unchanged.
	float32 Dot(const Vector2& other) const;

};