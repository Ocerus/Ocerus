#ifndef _SMARTPOINTER_H_
#define _SMARTPOINTER_H_

#include "Settings.h"

namespace Utils
{
	/// @brief A Smart pointer pattern implementation.
	/// @remarks This pointer counts its uses and deletes the data it is pointing at only if nobody
	/// points to them. This prevents the use of invalid pointers. Use whenever you acces a shared resource.
	template<class T> class SmartPointer
	{
	private:
		T* mPointer;
		uint32* mUseCountPtr;
	public:

		/// Default constructor.
		SmartPointer(): mPointer(0), mUseCountPtr(0) {}

		/// Constructs new smart pointer from a raw pointer.
		template< class Y>
		explicit SmartPointer(Y* pointer): mPointer(pointer), mUseCountPtr(DYN_NEW_T(uint32)(1)) {}

		/// Copy constructor.
		SmartPointer(const SmartPointer& r): mPointer(0), mUseCountPtr(0)
		{
			mPointer = r.mPointer;
			mUseCountPtr = r.mUseCountPtr; 
			// Handle zero pointer gracefully to manage STL containers
			if(mUseCountPtr)
				++(*mUseCountPtr); 
		}

		/// Assignment operator.
		SmartPointer& operator=(const SmartPointer& r)
		{
			if (mPointer == r.mPointer)
				return *this;
			// Swap current data into a local copy
			// this ensures we deal with rhs and this being dependent
			SmartPointer<T> tmp(r);
			Swap(tmp);
			return *this;
		}

		/// Conversion constructor.
		template< class Y>
		SmartPointer(const SmartPointer<Y>& r): mPointer(0), mUseCountPtr(0)
		{
			mPointer = static_cast<T*>(r.GetPointer());
			mUseCountPtr = r.GetUseCountPtr();
			// Handle zero pointer gracefully to manage STL containers
			if(mUseCountPtr)
				++(*mUseCountPtr);
		}

		/// Conversion operator.
		template< class Y>
		SmartPointer& operator=(const SmartPointer<Y>& r)
		{
			if (mPointer == r.mPointer)
				return *this;
			// Swap current data into a local copy
			// this ensures we deal with rhs and this being dependent
			SmartPointer<T> tmp(r);
			Swap(tmp);
			return *this;
		}

		virtual ~SmartPointer() { Release(); }

		/// Dereference operator.
		inline T& operator*() const { BS_DASSERT(mPointer); return *mPointer; }

		/// Dereference operator.
		inline T* operator->() const { BS_DASSERT(mPointer); return mPointer; }

		/// Returns the raw pointer.
		inline T* Get() const { return mPointer; }

		/// Returns the raw pointer.
		inline T* GetPointer() const { return mPointer; }

		/// True if there exists only one pointer to the shared resource.
		inline bool IsUnique() const { return *mUseCountPtr == 1; }

		/// Returns the number of smart pointers pointing to the resource.
		inline uint32 GetUseCount() const { BS_DASSERT(mUseCountPtr); return *mUseCountPtr; }

		/// Returns the pointer to the number of smart pointers pointing to the resource.
		inline uint32* GetUseCountPtr() const { return mUseCountPtr; }

		/// True if the raw pointer is null.
		inline bool IsNull(void) const { return mPointer == 0; }

		/// @brief Nulls the raw pointer.
		/// @remarks The data the raw pointer points at are deallocated during the process.
		inline void SetNull(void)
		{ 
			if (mPointer)
			{
				Release();
				mPointer = 0;
			}
		}

	protected:

		/// Deallocates the data the raw pointer points at.
		inline void Release(void)
		{
			if (mPointer)
			{
				bool destroyThis = false;
				if (--(*mUseCountPtr) == 0) 
					destroyThis = true;
				if (destroyThis)
					Destroy();
			}
		}

		/// Destroys this pointer together with the data it points at.
		virtual void Destroy(void)
		{
			// IF YOU GET A CRASH HERE, YOU FORGOT TO FREE UP POINTERS BEFORE SHUTTING DOWN
			// Use SetNull() before shutdown or make sure your pointer goes out of scope to avoid this.
			DYN_DELETE mPointer;
			DYN_DELETE mUseCountPtr;
		}

		/// Swaps two smart pointers.
		virtual void Swap(SmartPointer<T> &other) 
		{
			//TODO std pryc
			std::swap(mPointer, other.mPointer);
			std::swap(mUseCountPtr, other.mUseCountPtr);
		}
	};

	template<class T, class U> inline bool operator==(SmartPointer<T> const& a, SmartPointer<U> const& b)
	{
		return a.Get() == b.Get();
	}

	template<class T, class U> inline bool operator!=(SmartPointer<T> const& a, SmartPointer<U> const& b)
	{
		return a.Get() != b.Get();
	}

	template<class T, class U> inline bool operator<(SmartPointer<T> const& a, SmartPointer<U> const& b)
	{
		//TODO std pryc
		return std::less<const void*>()(a.Get(), b.Get());
	}
}

#endif