#include "Common.h"
#include "UnitTests.h"
#include "../GlobalProperties.h"

SUITE(GlobalProperties)
{
	TEST(StoreInt)
	{
		int* myInt = new int(10);
		Utils::GlobalProperties::SetPointer("my-int", myInt);
		CHECK_EQUAL(myInt, Utils::GlobalProperties::GetPointer<int>("my-int"));
		CHECK_EQUAL(*myInt, Utils::GlobalProperties::Get<int>("my-int"));
		delete myInt;
	}

	TEST(StoreChar)
	{
		char* myChar = new char('z');
		Utils::GlobalProperties::SetPointer("my-char", myChar);
		CHECK_EQUAL(myChar, Utils::GlobalProperties::GetPointer<char>("my-char"));
		CHECK_EQUAL(*myChar, Utils::GlobalProperties::Get<char>("my-char"));
		delete myChar;
	}
}

