#include "Common.h"
#include "UnitTests.h"
#include "../StringConverter.h"

SUITE(StringConverter)
{
	TEST(IntConvert)
	{
		int32 a = -1234567890;
		CHECK_EQUAL("-1234567890", StringConverter::ToString(a));
		int32 b = StringConverter::FromString<int32>("-1234567890");
		CHECK_EQUAL(a, b);
	}

	TEST(FloatConvert)
	{
		float64 a = -12345.6789;
		CHECK_EQUAL("-12345.7", StringConverter::ToString(a));
		float64 b = StringConverter::FromString<float64>("-12345.6789");
		CHECK_CLOSE(a, b, 0.0001);
	}

	TEST(CharConvert)
	{
		const char* a = "test string";
		string b = "test string";
		CHECK_EQUAL(b, StringConverter::ToString(a));
		char* c = StringConverter::FromString<char*>(b);
		CHECK_EQUAL(a, c);
		delete c;
	}

	TEST(StringConvert)
	{
		string a = "test string";
		CHECK_EQUAL("test string", StringConverter::ToString(a));
		CHECK_EQUAL("test string", StringConverter::FromString<string>(a));
	}

	TEST(BoolConvert)
	{
		bool a = true;
		CHECK_EQUAL("true", StringConverter::ToString(a));
		bool b = StringConverter::FromString<bool>("true");
		CHECK_EQUAL(a, b);
		a = false;
		CHECK_EQUAL("false", StringConverter::ToString(a));
		b = StringConverter::FromString<bool>("false");
		CHECK_EQUAL(a, b);
	}

	TEST(VectorConvert)
	{
		Vector2 b = StringConverter::FromString<Vector2>("1.2 3.4");
		CHECK_CLOSE(1.2f, b.x, 0.001);
		CHECK_CLOSE(3.4f, b.y, 0.001);
	}

	TEST(ColorConvert)
	{
		GfxSystem::Color b = StringConverter::FromString<GfxSystem::Color>("1 2 3 4");
		CHECK_EQUAL(1, b.r);
		CHECK_EQUAL(2, b.g);
		CHECK_EQUAL(3, b.b);
		CHECK_EQUAL(4, b.a);
	}
}

