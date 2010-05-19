#include "Common.h"
#include "UnitTests.h"
#include "../FormatText.h"

using namespace StringSystem;

SUITE(FormatText)
{
	TEST(Simple)
	{
		TextData test = FormatText("First ") << "second " << "third";
		CHECK_EQUAL(test.c_str(), "First second third");
	}

	TEST(Params)
	{
		TextData test = FormatText("T3: %3% T1: %%1 T2: %2%") << "V1" << "V2" << "V3";
		CHECK_EQUAL(test.c_str(), "T3: V3% T1: %V1 T2: V2%");
	}
}