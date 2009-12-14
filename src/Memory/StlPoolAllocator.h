/// @file
/// This is an implementation of STL allocator using freelists / memory pooling.
/// @remarks
/// The code is based on the code from http://www.sjbrown.co.uk/2004/05/01/pooled-allocators-for-the-stl/

#ifndef StlPoolAllocator_h__
#define StlPoolAllocator_h__

#include "FreeList.h"
#include <boost/shared_ptr.hpp>

namespace Memory
{
	/// An STL-compliant pooled allocator. It should speed up allocations inside tree-like structures as list, map, set, etc.
	/// @param T Type name of the values stored in the container.
	/// @param Allocable Structure which size is the maximum of what can be allocated by the container.
	template<typename T, typename Allocable>
	class StlPoolAllocator
	{
	public:

		typedef size_t		size_type;			///< A type that can represent the size of the largest object in the allocation model.
		typedef ptrdiff_t	difference_type;	///< A type that can represent the difference between any two pointers in the allocation model.

		typedef T			value_type;			///< Identical to T.
		typedef T*			pointer;			///< Pointer to T;
		typedef T const*	const_pointer;		///< Pointer to const T.
		typedef T&			reference;			///< Reference to T.
		typedef T const&	const_reference;	///< Reference to const T.

		/// A struct to construct an allocator for a different type.
		template<typename U> 
		struct rebind { typedef StlPoolAllocator<U, Allocable> other; };

		/// Creates a pooled allocator.
		StlPoolAllocator(): mPool(new Pool())
		{
			OC_ASSERT_MSG(sizeof(T) <= sizeof(Allocable), "Desired items of the pool are too big. Make Allocable bigger.");
		}

		/// Creates a pooled allocator to the argument's pool.
		template<typename U>
		StlPoolAllocator( const StlPoolAllocator<U, Allocable>& rhs )
		{
			OC_ASSERT_MSG(sizeof(T) <= sizeof(Allocable), "Desired items of the pool are too big. Make Allocable bigger.");
			mPool = rhs.mPool;
		}

		/// Assignment operator.
		StlPoolAllocator& operator=(const StlPoolAllocator& rhs)
		{
			mPool = rhs.mPool;
		}

		/// Default destructor.
		~StlPoolAllocator()	{}

		/// The largest value that can meaningfully passed to allocate.
		size_type max_size() const { return 0xffffffff; }

		/// Memory is allocated for \c count objects of type \c T but objects are not constructed.
		/// This function may raise an appropriate exception. The result is a random access iterator.
		pointer allocate( size_type count, std::allocator<void>::const_pointer /*hint*/ = 0 )
		{
			OC_DASSERT_MSG(count == 1, "Usage of STL pooled allocator for structures allocating more then 1 items at once");
			return (pointer)mPool->Allocate();
		}

		/// Deallocates memory allocated by allocate.
		/// All \c count objects in the area pointed by \c block must be destroyed prior to this call. 
		/// \c count must match the value passed to allocate to obtain this memory. Does not throw exceptions. 
		/// \c block must not be null.
		void deallocate( pointer block, size_type count ) throw()
		{
			OC_ASSERT_MSG( block, "Null pointer to deallocate" );
			OC_DASSERT_MSG(count == 1, "Usage of STL pooled allocator for structures allocating more then 1 items at once");
			mPool->Free( (Allocable*)block );
		}

		/// Constructs an element of \c T at the given pointer.
		/// Effect: new( element ) T( arg )
		void construct( pointer element, T const& arg )
		{
			new( element ) T( arg );
		}

		/// Destroys an element of \c T at the given pointer.
		/// Effect: element->~T()
		void destroy( pointer element )
		{
			element->~T();
		}

		/// Returns the address of the given reference.
		pointer address( reference element ) const
		{
			return &element;
		}

		/// Returns the address of the given reference.
		const_pointer address( const_reference element ) const
		{
			return &element;
		}

		typedef FreeList<Allocable, Policies::LinkedListAllocation<Allocable>, Policies::NullConstruction<Allocable>, Policies::DoubleGrowth<16> > Pool;

		/// The pool for this allocator.
		boost::shared_ptr<Pool> mPool;
	};


	/// Returns true if objects allocated from one pool can be deallocated from the other.
	template<typename T, typename U, typename Allocable>
	bool operator==( StlPoolAllocator<T, Allocable> const& left, StlPoolAllocator<U, Allocable> const& right )
	{
		return left.mPool == right.mPool;
	}

	/// Returns true if objects allocated from one pool cannot be deallocated from the other.
	template<typename T, typename U, typename Allocable>
	bool operator!=( StlPoolAllocator<T, Allocable> const& left, StlPoolAllocator<U, Allocable> const& right )
	{
		return left.mPool != right.mPool;
	}

}

#endif // StlPoolAllocator_h__
