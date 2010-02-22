#include "Common.h"
#include "UnitTests.h"
#include "../StringKey.h"

SUITE(StringKey)
{
	TEST(StringEquality)
	{
		StringKey a = "Test String";
		CHECK_EQUAL("Test String", a.ToString());
		StringKey b = "Test String";
		CHECK_EQUAL(a, b);
		CHECK_EQUAL((size_t)a, (size_t)b);
		CHECK_EQUAL(a==b, true);
		StringKey c = "Another String";
		CHECK_EQUAL(a==c, false);
		StringKey d("Test String ++", 11);
		CHECK_EQUAL(a==d, true);
		StringKey e("Test String ++", 12);
		CHECK_EQUAL(a==e, false);
		e=a;
		CHECK_EQUAL(a==e, true);
	}
}
