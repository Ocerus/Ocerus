#include "Common.h"
#include "UnitTests.h"
#include "Memory/FreeList.h"

using namespace Memory;

SUITE(FreeList)
{
	TEST(DefaultIntegers)
	{
		FreeList<int> freeList;
		int* number = freeList.Allocate();
		freeList.Free(number);
	}
}