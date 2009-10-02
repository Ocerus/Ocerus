/// @file
/// Helper math functions and constants.

#ifndef _MATHUTILS_H_
#define _MATHUTILS_H_

#include "Settings.h"
#include <math.h>

namespace Utils
{
	/// Helper math functions and constants.
	namespace MathUtils
	{
		const float32 PI = b2_pi;

		const float32 TWO_PI = 2.0f*PI;

		const float32 HALF_PI = 0.5f*PI;

		inline float32 Random(const float32 min, const float32 max) { return b2Random(min, max); }

		inline float32 Abs(const float32 num) { return b2Abs(num); }
		inline int32 Abs(const int32 num) { return num<0?-num:num; }

		inline float32 Min(const float32 num1, const float32 num2) { return b2Min(num1, num2); }

		inline void Swap(float32& num1, float32& num2) { b2Swap(num1, num2); }

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

		Vector2 VectorFromAngle(const float32 angle, const float32 size = 1.0f);

		inline bool IsPowerOfTwo(const uint32 num) { return b2IsPowerOfTwo(num); }

	}
}

#endif