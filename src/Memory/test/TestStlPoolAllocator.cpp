#include "Common.h"
#include "UnitTests.h"
#include "Memory/StlPoolAllocator.h"
#include <list>
#include <set>
#include <map>

using namespace Memory;

SUITE(StlPoolAllocator)
{
	TEST(BasicUsage)
	{
		pooled_list<int32>::type pooledList;

		pooledList.push_back(INT_MAX);

		for (int32 i=0; i<1000; ++i)
			pooledList.push_back(i);

		CHECK_EQUAL(INT_MAX, pooledList.front());

		pooledList.clear();
	}

	TEST(Pointers)
	{
		pooled_list<int32*>::type pooledList;

		for (int32 i=0; i<100; ++i)
			pooledList.push_back(0);

		pooledList.clear();
	}

/*
	TEST(ItemSizeOverflow)
	{
		typedef std::list< int64, StlPoolAllocator<int64, int32> > WrongList; 
		// this is gonna throw assert
		WrongList pooledList;
	}
*/
/*
	TEST(VoidType)
	{
		// this won't compile, which is what we want
		pooled_list<void>::type pooledList;
	}
*/

	TEST(BasicMap)
	{
		pooled_map<int, int>::type pooledMap;
		pooledMap[0] = 42;
		pooledMap[1] = 48;
		pooledMap[3] = 53;
		pooledMap[0] = 94;
		pooledMap.erase( 3 );
		CHECK_EQUAL(94, pooledMap[0]);
		CHECK_EQUAL(48, pooledMap[1]);

		pooledMap.clear();
	}

	TEST(AdvancedMap)
	{
		pooled_map<int, int>::type pooledMap;

		for (int32 i=0; i<1000; ++i)
		{
			pooledMap[i%123] = i%123;
		}

		for (int32 i=0; i<1000; ++i)
		{
			CHECK_EQUAL(i%123, pooledMap[i%123]);
		}
	}

	TEST(AdvancedSet)
	{
		pooled_set<double>::type pooledSet;

		for( int i = 0; i < 1000; ++i )
		{
			pooledSet.insert((double)(i%123));
		}

		for( int i = 0; i < 123; ++i )
		{
			CHECK(pooledSet.find((double)i) != pooledSet.end());
		}
	}

}
