#include "Common.h"
#include "UnitTests.h"
#include "../StringKey.h"

SUITE(StringKey)
{
	TEST(StringEquality)
	{
		StringKey a = "TEST STRING";
		CHECK_EQUAL("TEST STRING", (string)a);
		StringKey b = "TEST STRING";
		CHECK_EQUAL(a, b);
		CHECK_EQUAL((size_t)a, (size_t)b);
		CHECK_EQUAL(a==b, true);
		StringKey c = "ANOTHER STRING";
		CHECK_EQUAL(a==c, false);
		StringKey d("TEST STRING ++", 11);
		CHECK_EQUAL(a==d, true);
		StringKey e("TEST STRING ++", 12);
		CHECK_EQUAL(a==e, false);
		e=a;
		CHECK_EQUAL(a==e, true);
	}
}