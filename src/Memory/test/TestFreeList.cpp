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


	// A simple test class 
	class Foo
	{
	public:
		Foo() 
			: a(10), x(-1.0f) 
		{
		}
		Foo( int a_ ) 
			: a(a_), x(2.0f) 
		{
		}
		Foo( float x_ ) 
			: a(0), x(x_) 
		{
		}

		Foo( int a_, float x1, float x2 = 1.0f )
			: a(a_)
		{
			x = x1 * x2;
		}

		~Foo()
		{
			a = -100;
			x = -100.0f;
		}

	private:
		int a;
		float x;
	};

	// Another test class.
	// NOTE that this class is the same size as Foo (assuming sizeof(int) == 4)
	class Bar
	{
	public:
		Bar() 
			: d(3.1415926535)
		{
		}

		~Bar()
		{
			d = 2.7182818;
		}

	private:
		double d;
	};

	template< class T, class FreeList > void RunSimpleTest( FreeList& freeList )
	{
		// Allocate a few objects
		T* pAt1 = freeList.Allocate();
		T* pAt2 = freeList.Allocate();
		T* pAt3 = freeList.Allocate();

		// Free one of them
		freeList.Free( pAt1 );

		// Allocate 10,000 objects quickly
		T* tempArray[10000];
		for( int ix = 0; ix < 10000; ++ix )
		{
			tempArray[ix] = freeList.Allocate();
		}

		// Return those 10,000 objects back to the freelist
		for( int ix = 0; ix < 10000; ++ix )
		{
			freeList.Free( tempArray[ix] );
		}

		// Free the rest
		freeList.Free(pAt2);
		freeList.Free(pAt3);

		// Check that there is nothing allocated in the freelist
		CHECK_EQUAL(freeList.GetNumberOfBlocksInUse(), (uint32)0);
	}


	TEST(DefaultClass)
	{
		FreeList< Foo > aDefaultTList;

		RunSimpleTest< Foo >( aDefaultTList );

		// Show that it can handle various constructors
		Foo* pFoo1 = aDefaultTList.Allocate( 123 );
		Foo* pFoo2 = aDefaultTList.Allocate( 1.414f );
		Foo* pFoo3 = aDefaultTList.Allocate( -321, 0.1f );

		// ... and destructor
		aDefaultTList.Free(pFoo1);
		aDefaultTList.Free(pFoo2);
		aDefaultTList.Free(pFoo3);
	}

	TEST(NullConstruction_LinkedList_DoubleGrowth)
	{
		FreeList< Foo, Policies::LinkedListAllocation<Foo>, Policies::NullConstruction<Foo>, Policies::DoubleGrowth<16> > aGPG4FreeList;

		RunSimpleTest< Foo >( aGPG4FreeList );
	}

	TEST(NullConstruction_StackAllocation_ConstantGrowth)
	{
		FreeList< Foo, Policies::StackAllocation<Foo>, Policies::NullConstruction<Foo>, Policies::ConstantGrowth<16,16,1024> > aGPG4FreeList;

		RunSimpleTest< Foo >( aGPG4FreeList );

		// Test the additional capabilities of the ConstructOnceStack
		// Note that these functions are not provided by all policies, but if aTList2 were ever changed to use
		// a different policy, these lines would produce compile errors instead of runtime errors.
		aGPG4FreeList.FreeAll();

		aGPG4FreeList.Reset();
	}

	TEST(PlacementNew_CompactableChunk_ConstantChunkSizeGrowth)
	{
		FreeList< Foo, Policies::CompactableChunkAllocation<Foo>, Policies::PlacementNewConstruction<Foo>, Policies::ConstantChunkSizeGrowth<Foo, 4096> > aCompactableTList;

		RunSimpleTest< Foo >( aCompactableTList );
	}

	TEST(SharedChunkAllocation)
	{
		FreeList< Foo, Policies::SharedChunkAllocation<Foo> > aSharedTList;

		RunSimpleTest< Foo >( aSharedTList );

		// Run another test with Bar in a shared chunk policy--notice that it uses the same underlying list as the 
		// FreeList above
		FreeList< Bar, Policies::SharedChunkAllocation<Bar> > aSecondSharedTList;

		RunSimpleTest< Bar >( aSecondSharedTList );
	}

}