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

	TEST(RawArrayPointer)
	{
		Array<int32> testArray(16);
		int32* rawArray = testArray.GetRawArrayPtr();
		for (int32 i=0; i<16; ++i)
			rawArray[i] = i;
		for (int32 i=0; i<16; ++i)
			CHECK_EQUAL(rawArray[i], i);
	}

	TEST(Clear)
	{
		Array<int32> testArray(16);
		for (int32 i=0; i<testArray.GetSize(); ++i)
			testArray[i] = i;
		testArray.Clear();
		CHECK_EQUAL(testArray.GetSize(), 0);
	}

	TEST(Resize)
	{
		Array<int32> testArray(16);
		for (int32 i=0; i<testArray.GetSize(); ++i)
			testArray[i] = i;
		testArray.Resize(10);
		for (int32 i=0; i<testArray.GetSize(); ++i)
			CHECK_EQUAL(testArray[i], i);
		testArray.Resize(16);
		for (int32 i=0; i<10; ++i)
			CHECK_EQUAL(testArray[i], i);
	}

	TEST(CopyFrom)
	{
		Array<int32> firstArray(16);
		for (int32 i=0; i<firstArray.GetSize(); ++i)
			firstArray[i] = i;
		Array<int32> secondArray;
		secondArray.CopyFrom(firstArray);
		for (int32 i=0; i<secondArray.GetSize(); ++i)
			CHECK_EQUAL(secondArray[i], i);
	}

}