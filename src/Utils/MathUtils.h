/// @file
/// Helper math functions.

#ifndef _MATHUTILS_H_
#define _MATHUTILS_H_

#include "../Setup/Settings.h"

namespace Utils
{
	/// Helper math functions.
	namespace MathUtils
	{
		inline float32 Random(const float32 min, const float32 max);

		inline float32 Abs(const float32 num) { return b2Abs(num); }
		inline int32 Abs(const int32 num) { return num<0?-num:num; }

		template<typename T>
		inline T Min(const T num1, const T num2) { return b2Min(num1, num2); }
		
		template<typename T>
		inline T Max(const T num1, const T num2) { return b2Max(num1, num2); }

		template<typename T>
		inline void Swap(T& num1, T& num2) { b2Swap(num1, num2); }

		inline int32 Round(const float32 num) { return static_cast<int32>(floorf(num<0?(num-0.5f):(num+0.5f))); }

		inline int64 Round(const float64 num) { return static_cast<int64>(floor(num<0?(num-0.5f):(num+0.5f))); }

		inline int32 Floor(const float32 num) { return static_cast<int32>(floorf(num)); }

		inline int32 Ceiling(const float32 num) { return static_cast<int32>(ceilf(num)); }

		inline float32 Sqr(const float32 num) { return num*num; }

		inline float32 Sqrt(const float32 num) { return b2Sqrt(num); }

		inline float32 Distance(const Vector2& first, const Vector2& second) { return b2Distance(first, second); }

		inline float32 DistanceSquared(const Vector2& first, const Vector2& second) { return b2DistanceSquared(first, second); }

		float32 AngleDistance(const float32 angle1, const float32 angle2);

		inline Vector2 Multiply(const Matrix22& mat, const Vector2& vec) { return b2Mul(mat, vec); }

		inline Vector2 Multiply(const XForm& xf, const Vector2& vec) { return b2Mul(xf, vec); }

		inline float32 Sin(const float32 angle) { return sinf(angle); }

		inline float32 Cos(const float32 angle) { return cosf(angle); }

		inline float32 Tan(const float32 angle) { return tanf(angle); }

		inline float32 ArcTan(const float32 x) { return atanf(x); }

		inline float32 ArcSin(const float32 x) { return asinf(x); }

		inline float32 Dot(const Vector2& vec1, const Vector2& vec2) { return b2Dot(vec1, vec2); }

		inline float32 Cross(const Vector2& vec1, const Vector2& vec2) { return b2Cross(vec1, vec2); }

		float32 Clamp(const float32 num, const float32 min, const float32 max);

		inline float32 ClampAngle(const float32 angle) { return Clamp(angle, 0.0f, TWO_PI); }

		bool IsAngleInRange(const float32 angle, const float32 min, const float32 max);

		float32 Wrap(const float32 num, const float32 min, const float32 max);

		inline float32 WrapAngle(const float32 angle) { return Wrap(angle, 0.0f, TWO_PI); }

		float32 ComputePolygonArea(const Vector2* poly, const uint32 len);

		float32 Angle(const Vector2& vec1, const Vector2& vec2 = Vector2(1.0f, 0.0f));

		inline float32 RadToDeg(const float32 radians) { return radians * 180 / MathUtils::PI; }

		Vector2 VectorFromAngle(const float32 angle, const float32 size = 1.0f);

		inline bool IsPowerOfTwo(const uint32 num) { return b2IsPowerOfTwo(num); }

	}
}


//-----------------------------------------------------------------------------
// Implementation

const int32 RAND_LIMIT = 32767;

float32 Utils::MathUtils::Random(const float32 min, const float32 max)
{
	float32 r = (float32)(rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = (max - min) * r + min;
	return r;
}


#endif