#include "Common.h"
#include "UnitTests.h"
#include "Memory/StlPoolAllocator.h"
#include <list>
#include <set>
#include <map>

using namespace Memory;

SUITE(StlPoolAllocator)
{
	template<typename _Ty>
	struct pooled_list // list node
	{  
		// these definitions are taken from VS STL to exactly mimick what will be allocated by the container
		// note that if the structure will be too small, an assert will be raised during the construction
		struct _Node;
		friend struct _Node;
		typedef _Node *_Nodeptr;	// _Node allocator must have ordinary pointers
		struct _Node
		{	
			_Nodeptr _Next;	// successor node, or first element if head
			_Nodeptr _Prev;	// predecessor node, or last element if head
			_Ty _Myval;	// the stored value, unused if head
		};

		typedef typename _Node allocable;
		typedef std::list< _Ty, StlPoolAllocator<_Ty, allocable> > type; 
	};


	template<typename _Key, typename _Ty>
	struct pooled_map 
	{ 
		// these definitions are taken from VS STL to exactly mimick what will be allocated by the container
		// note that if the structure will be too small, an assert will be raised during the construction
		typedef typename std::map<_Key, _Ty>::value_type value_type;
		struct _Node;
		friend struct _Node;
		typedef _Node *_Nodeptr;	// _Node allocator must have ordinary pointers
		struct _Node // tree node
		{	
			_Nodeptr _Left;	// left subtree, or smallest element if head
			_Nodeptr _Parent;	// parent, or root of tree if head
			_Nodeptr _Right;	// right subtree, or largest element if head
			value_type _Myval;	// the stored value, unused if head
			char _Color;	// _Red or _Black, _Black if head
			char _Isnil;	// true only if head (also nil) node
		};

		typedef typename _Node allocable;
		typedef std::map< _Key, _Ty, std::less<_Key>, StlPoolAllocator<value_type, allocable> > type; 
	};


	template<typename _Key>
	struct pooled_set 
	{ 
		// these definitions are taken from VS STL to exactly mimick what will be allocated by the container
		// note that if the structure will be too small, an assert will be raised during the construction
		typedef typename std::set<_Key>::value_type value_type;
		struct _Node;
		friend struct _Node;
		typedef _Node *_Nodeptr;	// _Node allocator must have ordinary pointers
		struct _Node // tree node
		{	
			_Nodeptr _Left;	// left subtree, or smallest element if head
			_Nodeptr _Parent;	// parent, or root of tree if head
			_Nodeptr _Right;	// right subtree, or largest element if head
			value_type _Myval;	// the stored value, unused if head
			char _Color;	// _Red or _Black, _Black if head
			char _Isnil;	// true only if head (also nil) node
		};

		typedef typename _Node allocable;
		typedef std::set< _Key, std::less<_Key>, StlPoolAllocator<value_type, allocable> > type; 
	};


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
