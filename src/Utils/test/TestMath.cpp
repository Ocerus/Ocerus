#include "Common.h"
#include "UnitTests.h"
#include "MathUtils.h"

SUITE(MathUtils)
{
	TEST(PI)
	{
		CHECK_CLOSE(3.1415926535897932384626433832795f, MathUtils::PI, numeric_limits<float>::epsilon());
		CHECK_CLOSE(1.5707963267948966192313216916398f, MathUtils::HALF_PI, numeric_limits<float>::epsilon());
	}

	TEST(AbsoluteValue)
	{
		CHECK_EQUAL(MathUtils::Abs(11), 11);
		CHECK_EQUAL(MathUtils::Abs(-11), 11);
		CHECK_EQUAL(MathUtils::Abs(11.0f), 11.0f);
		CHECK_EQUAL(MathUtils::Abs(-11.0f), 11.0f);
		CHECK_EQUAL(MathUtils::Abs(numeric_limits<float>::max()), numeric_limits<float>::max());
		CHECK_EQUAL(MathUtils::Abs(numeric_limits<float>::min()), numeric_limits<float>::min());
		CHECK_EQUAL(MathUtils::Abs(-numeric_limits<float>::max()), numeric_limits<float>::max());
		CHECK_EQUAL(MathUtils::Abs(-numeric_limits<float>::min()), numeric_limits<float>::min());
		CHECK_EQUAL(MathUtils::Abs(numeric_limits<float>::epsilon()), numeric_limits<float>::epsilon());
		CHECK_EQUAL(MathUtils::Abs(-numeric_limits<float>::epsilon()), numeric_limits<float>::epsilon());
	}

	TEST(Minimum)
	{
		CHECK_EQUAL(MathUtils::Min(1, 10), 1);
		CHECK_EQUAL(MathUtils::Min(-1, 10), -1);
		CHECK_EQUAL(MathUtils::Min(1, -10), -10);
		CHECK_EQUAL(MathUtils::Min(numeric_limits<float>::min(), numeric_limits<float>::max()), numeric_limits<float>::min());
		CHECK_EQUAL(MathUtils::Min(numeric_limits<float>::min(), -numeric_limits<float>::max()), -numeric_limits<float>::max());
		CHECK_EQUAL(MathUtils::Min(-numeric_limits<float>::min(), -numeric_limits<float>::max()), -numeric_limits<float>::max());
	}

	TEST(Angle)
	{
		Vector2 v1(0,1);
		Vector2 v2(1,0);
		CHECK_CLOSE(MathUtils::HALF_PI, MathUtils::Angle(v1,v2), numeric_limits<float>::epsilon());
	}

	TEST(AngleDistance)
	{
		CHECK_CLOSE(0, MathUtils::AngleDistance(MathUtils::HALF_PI, MathUtils::HALF_PI), numeric_limits<float>::epsilon());
		CHECK_CLOSE(MathUtils::HALF_PI, MathUtils::AngleDistance(MathUtils::PI, MathUtils::HALF_PI), numeric_limits<float>::epsilon());
		CHECK_CLOSE(MathUtils::HALF_PI, MathUtils::AngleDistance(-MathUtils::PI, MathUtils::HALF_PI), numeric_limits<float>::epsilon());
		//not very nice precision but ok
		CHECK_CLOSE(MathUtils::HALF_PI + 1.0f, MathUtils::AngleDistance(-10*MathUtils::PI, MathUtils::HALF_PI + 1.0f), 0.000001f);
		CHECK_CLOSE(MathUtils::PI - 1.0f, MathUtils::AngleDistance(-20*MathUtils::PI, MathUtils::PI + 1.0f), 0.00001f);
	}

	TEST(IsAngleInRange)
	{
		CHECK(MathUtils::IsAngleInRange(MathUtils::HALF_PI, 0.0f, MathUtils::PI));
		CHECK(MathUtils::IsAngleInRange(MathUtils::HALF_PI, 0.0f, MathUtils::HALF_PI));
		CHECK(MathUtils::IsAngleInRange(0, 0.0f, MathUtils::HALF_PI));
		CHECK(MathUtils::IsAngleInRange(MathUtils::HALF_PI, MathUtils::HALF_PI, MathUtils::HALF_PI));
		CHECK(MathUtils::IsAngleInRange(MathUtils::HALF_PI, MathUtils::PI*2, MathUtils::HALF_PI*10));
	}
	

	TEST(VectorFromAngle)
	{
		CHECK_EQUAL(MathUtils::VectorFromAngle(0, 1.0f).x, 1.0f);
		
		Vector2 vector(MathUtils::Random(0.0f, 10.0f), MathUtils::Random(0.0f, 10.0f));
		float32 angle = MathUtils::Angle(vector,Vector2(1,0));

		//lower precision
		CHECK_CLOSE(vector.x, MathUtils::VectorFromAngle(angle, vector.Length()).x, 0.0001f);
		CHECK_CLOSE(vector.y, MathUtils::VectorFromAngle(angle, vector.Length()).y, 0.0001f);
	}

	//TODO some tests are missing.
}
