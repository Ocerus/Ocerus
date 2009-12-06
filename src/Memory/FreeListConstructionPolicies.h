/// @file
/// Free lists / memory pooling implementation taken from Game Programming Gems 5. Edited to fit with our code and
/// comment style.

#ifndef FreeListConstructionPolicies_h__
#define FreeListConstructionPolicies_h__

#include "Base.h"
#include "FreeListPolicyHelpers.h"

namespace Memory
{
	namespace Policies
	{
		/// This policy creates objects by calling the placement new operator on them. For destruction the destructor is called.
		template< typename T > 
		class PlacementNewConstruction
		{
		public:

			/// Calls the constructor on the object of type T residing at pMemory.
			inline static T* Create( void* pMemory )
			{
				return new( pMemory ) T;
			}

			/// Calls the constructor on the object of type T residing at pMemory.
			template< typename A1 > 
			inline static T* Create( void* pMemory, A1 a1 )
			{
				return new( pMemory ) T(a1);
			}

			/// Calls the constructor on the object of type T residing at pMemory.
			template< typename A1, typename A2 > 
			inline static T* Create( void* pMemory, A1 a1, A2 a2 )
			{
				return new( pMemory ) T(a1, a2);
			}

			/// Calls the constructor on the object of type T residing at pMemory.
			template< typename A1, typename A2, typename A3 > 
			inline static T* Create( void* pMemory, A1 a1, A2 a2, A3 a3 )
			{
				return new( pMemory ) T(a1, a2, a3);
			}

			/// Calls the constructor on the object of type T residing at pMemory.
			template< typename A1, typename A2, typename A3, typename A4 > 
			inline static T* Create( void* pMemory, A1 a1, A2 a2, A3 a3, A4 a4 )
			{
				return new( pMemory ) T(a1, a2, a3, a4);
			}

			/// Safely destroys an object of the type T (no memory is released here).
			inline static void Destroy( T* pItem )
			{
				pItem->~T();
			}
		};



		/// This policy makes no construction nor destruction.
		/// Note that the code won't compile if T won't have the default constructor defined.
		template< typename T >
		class NullConstruction
		{
		public:

			inline static T* Create( void* pMemory )
			{
				return reinterpret_cast< T* >( pMemory );
			}

			inline static void Destroy( T* pItem )
			{

			}

		};

	}
} // namespace Memory

#endif // FreeListConstructionPolicies_h__
