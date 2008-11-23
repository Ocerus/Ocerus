#include "Common.h"
#include "MathUtils.h"

float32 MathUtils::ComputePolygonArea(const Vector2* poly, const uint32 len)
{
	float32 area = 0.0f;
	for (uint32 j=0, i=len-1, k=len-2; j<len; k=i, i=j, ++j)
		area += poly[i].x * (poly[j].y-poly[k].y);
	return 0.5f * area;
}
