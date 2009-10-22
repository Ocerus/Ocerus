#include "UnitTests.h"
#include "MathUtils.h"

SUITE(MathUtils)
{
	TEST(PI)
	{
		CHECK_CLOSE(3.1415926535897932384626433832795f, MathUtils::PI, 0.000000000000001f);
		CHECK_CLOSE(1.5707963267948966192313216916398f, MathUtils::HALF_PI, 0.000000000001f);
	}

	TEST(AbsoluteValue)
	{
		CHECK_EQUAL(MathUtils::Abs(11), 11);
		CHECK_EQUAL(MathUtils::Abs(-11), 11);
		CHECK_EQUAL(MathUtils::Abs(11.0f), 11.0f);
		CHECK_EQUAL(MathUtils::Abs(-11.0f), 11.0f);
		CHECK_EQUAL(MathUtils::Abs(FLOAT32_MAX), FLOAT32_MAX);
		CHECK_EQUAL(MathUtils::Abs(FLOAT32_MIN), FLOAT32_MIN);
		CHECK_EQUAL(MathUtils::Abs(-FLOAT32_MAX), FLOAT32_MAX);
		CHECK_EQUAL(MathUtils::Abs(-FLOAT32_MIN), FLOAT32_MIN);
		CHECK_EQUAL(MathUtils::Abs(FLOAT32_EPSILON), FLOAT32_EPSILON);
		CHECK_EQUAL(MathUtils::Abs(-FLOAT32_EPSILON), FLOAT32_EPSILON);
	}

	TEST(Minimum)
	{
		CHECK_EQUAL(MathUtils::Min(1, 10), 1);
		CHECK_EQUAL(MathUtils::Min(-1, 10), -1);
		CHECK_EQUAL(MathUtils::Min(1, -10), -10);
		CHECK_EQUAL(MathUtils::Min(FLOAT32_MIN, FLOAT32_MAX), FLOAT32_MIN);
		CHECK_EQUAL(MathUtils::Min(FLOAT32_MIN, -FLOAT32_MAX), -FLOAT32_MAX);
		CHECK_EQUAL(MathUtils::Min(-FLOAT32_MIN, -FLOAT32_MAX), -FLOAT32_MAX);
	}

	//TODO A lot of tests are missing.

	TEST(VectorFromAngle)
	{
		CHECK_EQUAL(MathUtils::VectorFromAngle(0, 1.0f).x, 1.0f);
	}
}
