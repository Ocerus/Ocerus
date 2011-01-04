

/// @name Global Functions
/** @ingroup globals */
//@{


/// @brief Returns the GUI window with the given name.
/// Returns null if no such exists.
Window@ GetWindow(const string& windowName);

/// Returns the text data from the strings manager given the group and the key.
const string& GetTextData(const StringKey& group, const StringKey& key);

/// Returns the text data from the strings manager given the key in the default group.
const string& GetTextData(const StringKey& key);


//@}





/// @name Math
/**	@ingroup globals */
//@{

/// @brief The PI constant.
const float32 PI;

/// Absolute value.
float32 Abs(float32 number);

/// Absolute value.
int32 Abs(int32 number);

/// Returns the angle of two vectors.
float32 Angle(const Vector2& a, const Vector2& b);

/// Minimum distance between two angles.
float32 AngleDistance(float32 a, float32 b);

/// Arcus sinus.
float32 ArcSin(float32 x);

/// Arcus tangent.
float32 ArcTan(float32 x);

/// Rounds the number up.
int32 Ceiling(float32 number);

/// Cosinus.
float32 Cos(float32 x);

/// Returns x if it's in [min, max]. Otherwise it returns one of the limits.
float32 Clamp(float32 x, float32 min, float32 max);

/// Returns x if it's in [0, 2PI]. Otherwise it returns one of the limits.
float32 ClampAngle(float32 x);

/// Magnitude of the cross product of two vectors.
float32 Cross(const Vector2& a, const Vector2& b);

/// Distance between two vectors.
float32 Distance(const Vector2& a, const Vector2& b);

/// Squared distance between two vectors.
float32 DistanceSquared(const Vector2& a, const Vector2& b);

/// Dot product of two vectors.
float32 Dot(const Vector2& a, const Vector2& b);

/// Rounds the number down.
int32 Floor(float32 number);

/// Returns true if the angle is in [min, max].
bool IsAngleInRange(float32 x, float32 min, float32 max);

/// Returns true if the number is a power of two.
bool IsPowerOfTwo(uint32 x);

/// Maximum of the two numbers.
float32 Max(float32 a, float32 b);

/// Maximum of the two numbers.
int32 Max(int32 a, int32 b);

/// Maximum of the two vectors.
Vector2 Max(const Vector2& a, const Vector2& b);

/// Minimum of the two numbers.
float32 Min(float32 a, float32 b);

/// Minimum of the two numbers.
int32 Min(int32 a, int32 b);

/// Minimum of the two vectors.
Vector2 Min(const Vector2& a, const Vector2& b);

/// Converts an angle in radians into degrees.
float32 RadToDeg(float32 angle);

/// Returns the given vector rotated by the given angle.
Vector2 RotateVector(const Vector2& vector, float32 angle);

/// Rounds the number.
int32 Round(float32 number);

/// Rounds the number.
int64 Round(float64 number);

/// Sinus.
float32 Sin(float32 x);

/// Square of the number.
float32 Sqr(float32 number);

/// Square root.
float32 Sqrt(float32 number);

/// Tangent.
float32 Tan(float32 x);

/// Creates vector from the given angle and size.
Vector2 VectorFromAngle(float32 angle, size);

/// Wraps x around [min, max], so that it's in the interval but retains some of the info it held.
/// This is done by continously subtracting or adding (max-min).
float32 Wrap(float32 x, float32 min, float32 max);

/// Wraps x around [0, 2PI], so that it's in the interval but retains some of the info it held.
/// This is done by continously subtracting or adding 2PI.
float32 WrapAngle(float32 x);


//@}







#define GLOBAL_FUNCTIONS(TYPE) \
/** @name Logging */ \
/** @ingroup globals */ \
/** @{ */ \
/** Writes a message of the TYPE type into the log and the console. */ \
void Println(const TYPE message); \
/** Writes a message of the array_##TYPE type into the log and the console. */ \
void Println(const array_##TYPE message); \
/** @} */

GLOBAL_FUNCTIONS(bool);
GLOBAL_FUNCTIONS(int8);
GLOBAL_FUNCTIONS(int16);
GLOBAL_FUNCTIONS(int32);
GLOBAL_FUNCTIONS(int64);
GLOBAL_FUNCTIONS(uint8);
GLOBAL_FUNCTIONS(uint16);
GLOBAL_FUNCTIONS(uint32);
GLOBAL_FUNCTIONS(uint64);
GLOBAL_FUNCTIONS(float32);
GLOBAL_FUNCTIONS(Vector2);
GLOBAL_FUNCTIONS(Point);
GLOBAL_FUNCTIONS(string);
GLOBAL_FUNCTIONS(StringKey);
GLOBAL_FUNCTIONS(EntityHandle);
GLOBAL_FUNCTIONS(Color);
GLOBAL_FUNCTIONS(eKeyCode);
