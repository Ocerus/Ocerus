/// @file
/// Copy-on-write shared pointer pattern implementation.
/// @remarks
/// Implementation based on the code from More C++ Idioms wikibook [1].
/// [1] http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Copy-on-write

#ifndef _COWPTR_H_
#define _COWPTR_H_

#include "../Setup/Settings.h"
#include <boost/shared_ptr.hpp>

namespace Utils
{
	/// A copy-on-write shared pointer pattern implementation.
	/// This pointer works like shared pointer; whenever the pointer is copied,
	/// only reference count changes. However, this pointer assures that data
	/// shall be copied as soon as necessary.

	template<class T> class COWPtr
	{
	public:
		typedef boost::shared_ptr<T> RefPtr;

	private:
		RefPtr mSharedPtr;

	protected:
		inline void detach()
		{
			T* tmp = mSharedPtr.get();
			if (tmp != 0 && !mSharedPtr.unique())
			{
				mSharedPtr = RefPtr(new T(*tmp));
			}
		}

	public:
		/// Constructs a copy-on-write pointer from regular pointer.
		/// As soon as the copy-on-write pointer is created, it is not safe to
		/// use the regular pointer.
		COWPtr(T* t): mSharedPtr(t) {}

		/// Constructs a copy-on-write pointer from shared pointer.
		COWPtr(const RefPtr& refPtr): mSharedPtr(refPtr) {}

		/// Constructs a copy of the other copy-on-write pointer. The copy constructor
		/// is very fast, as it only changes the reference counter.
		COWPtr(const COWPtr<T>& other): mSharedPtr(other.mSharedPtr) {}

		/// Assigns the other copy-on-write pointer to this pointer.
		COWPtr& operator=(const COWPtr<T>& other)
		{
			mSharedPtr = other.mSharedPtr; // no need to check for self-assignment with boost::shared_ptr
			return *this;
		}

		/// Dereferences the pointer for const operations.
		const T& operator*() const
		{
			return *mSharedPtr;
		}

		/// Dereferences the pointer for non-const operations.
		/// Data are copied, if necessary.
		T& operator*()
		{
			detach();
			return *mSharedPtr;
		}

		/// This operator is provided for convenience.
		/// @see operator*() const
		const T* operator->() const
		{
			return mSharedPtr.operator->();
		}

		/// This operator is provided for convenience.
		/// @see operator*()
		T* operator->()
		{
			detach();
			return mSharedPtr.operator->();
		}
	};
}

#endif // _COWPTR_H_