#include "Common.h"
#include "UnitTests.h"
#include "Memory/ClassAllocation.h"

using namespace Memory;

SUITE(ClassAllocation)
{
	struct DefaultAlloc: ClassAllocation<DefaultAlloc, ALLOCATION_HEAP_MAIN> { int32* mNothing; };
	struct PooledAlloc: ClassAllocation<PooledAlloc, ALLOCATION_POOLED> { int32* mNothing; };

	TEST(DefaultAllocation)
	{
		DefaultAlloc* testClass = new DefaultAlloc();

		for (int32 i=0; i<100; ++i)
		{
			delete new DefaultAlloc();
		}			

		delete testClass;
	}

	TEST(PooledAllocation)
	{
		PooledAlloc* testClass = new PooledAlloc();

		for (int32 i=0; i<100; ++i)
		{
			delete new PooledAlloc();
		}			

		delete testClass;
	}
}