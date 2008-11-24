#ifndef _MATHUTILS_H_
#define _MATHUTILS_H_

#include "Settings.h"
#include <math.h>

namespace MathUtils
{
	const float32 PI = b2_pi;

	inline uint32 Round(const float32 num) { return static_cast<uint32>(floor(num+0.5)); }

	inline float32 Sqrt(const float32 num) { return b2Sqrt(num); }

	inline float32 Distance(const Vector2& first, const Vector2& second) { return b2Distance(first, second); }

	inline Vector2 Multiply(const Matrix22& mat, const Vector2& vec) { return b2Mul(mat, vec); }

	inline Vector2 Multiply(const XForm& xf, const Vector2& vec) { return b2Mul(xf, vec); }

	float32 ComputePolygonArea(const Vector2* poly, const uint32 len);

}

#endif