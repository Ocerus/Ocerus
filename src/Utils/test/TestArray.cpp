#include "Common.h"
#include "UnitTests.h"
#include "../Array.h"

SUITE(Array)
{
	TEST(ConstructorAndDestructor)
	{
		Array<int32>* testArray = new Array<int32>(1024);

		{
			Array<int32> testArray(1024);
		} // the destructor is called here
		
		delete testArray;
	}


	TEST(ReadAndWriteBracketOperators)
	{
		Array<int32>* testArray = new Array<int32>(1024);
		for (int32 i=0; i<1024; ++i)
			(*testArray)[i] = 0;
		(*testArray)[0] = (*testArray)[1023] = (*testArray)[10] = 123456;
		CHECK_EQUAL((*testArray)[0], 123456);
		CHECK_EQUAL((*testArray)[10], 123456);
		CHECK_EQUAL((*testArray)[1023], 123456);

		const Array<int32>* testConstArray = testArray;
		CHECK_EQUAL((*testConstArray)[0], 123456);
		CHECK_EQUAL((*testConstArray)[10], 123456);
		CHECK_EQUAL((*testConstArray)[1023], 123456);
	}


	TEST(ArrayOfPointers)
	{
		Array<int32*> testArray(16);
		for (int32 i=0; i<16; ++i)
			testArray[i] = new int32(i);
		for (int32 i=0; i<16; ++i)
			CHECK_EQUAL(*testArray[i], i);
		for (int32 i=0; i<16; ++i)
			delete testArray[i];
	}
}