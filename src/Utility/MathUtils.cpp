#include "Common.h"
#include "MathUtils.h"

float32 MathUtils::ComputePolygonArea(const Vector2* poly, const uint32 len)
{
	float32 area = 0.0f;
	for (uint32 j=0, i=len-1, k=len-2; j<len; k=i, i=j, ++j)
		area += poly[i].x * (poly[j].y-poly[k].y);
	return 0.5f * area;
}

float32 MathUtils::Angle( const Vector2& a, const Vector2& b )
{
	// note: I flipped a,b cos I need the angle in an inverse manner
	float32 cos = Dot(b, a) / (b.Length()*a.Length());
	cos = Clamp(cos, -1.0f, 1.0f);
	if (Cross(b, a) < 0)
		return -acosf(cos);
	return acosf(cos);
}

float32 MathUtils::Clamp( const float32 num, const float32 min, const float32 max )
{
	assert(max>=min);
	float32 res = num;
	if (res > max)
		res = max;
	else if (res < min)
		res = min;
	return res;
}

float32 MathUtils::Wrap( const float32 num, const float32 min, const float32 max )
{
	assert(max>=min);
	float32 res = num;
	float32 d = max - min;
	while (res > max)
		res -= d;
	while (res < min)
		res += d;
	return res;
}

float32 MathUtils::AngleDistance( const float32 angle1, const float32 angle2 )
{
	float32 normAng1 = WrapAngle(angle1);
	float32 normAng2 = WrapAngle(angle2);
	float32 d1 = Abs(normAng1 - normAng2);
	float32 d2 = Abs(normAng1 - TWO_PI - normAng2);
	float32 d3 = Abs(normAng1 + TWO_PI - normAng2);
	return Min(Min(d1, d2), d3);
}

bool MathUtils::IsAngleInRange( const float32 angle, const float32 min, const float32 max )
{
	float32 normAng = WrapAngle(angle);
	float32 normMin = WrapAngle(min);
	float32 normMax = WrapAngle(max);
	bool swapped = false;
	if (normMin > normMax)
	{
		swapped = true;
		Swap(normMin, normMax);
	}
	bool result = normAng >= normMin && normAng <= normMax;
	if (swapped)
		result = !result;
	return result;
}

Vector2 MathUtils::VectorFromAngle( const float32 angle, const float32 size /*= 1.0f*/ )
{
	return Multiply(Matrix22(angle), Vector2(size, 0.0f));
}

float MathUtils::FloatSwap(float f)
{
	union
	{
		float f;
		unsigned char b[4];
	} dat1, dat2;

	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
}

int MathUtils::LongSwap(int i)
{
	unsigned char b1, b2, b3, b4;

	b1 = i & 255;
	b2 = ( i >> 8 ) & 255;
	b3 = ( i>>16 ) & 255;
	b4 = ( i>>24 ) & 255;

	//return ((int)b1 << 24) + ((int)b2 << 16) + ((int)b3 << 8) + b4;
	return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
}