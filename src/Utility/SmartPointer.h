#ifndef _SMARTPOINTER_H_
#define _SMARTPOINTER_H_

#include "Settings.h"

template<class T> class SmartPointer
{
protected:
	T* mPointer;
	uint32* mUseCountPtr;
public:
	SmartPointer(): mPointer(0), mUseCountPtr(0) {}

    template< class Y>
	explicit SmartPointer(Y* pointer): mPointer(pointer), mUseCountPtr(DYN_NEW_T(uint32)(1)) {}
		
	SmartPointer(const SmartPointer& r): mPointer(0), mUseCountPtr(0)
	{
	    mPointer = r.mPointer;
		mUseCountPtr = r.mUseCountPtr; 
	    // Handle zero pointer gracefully to manage STL containers
	    if(mUseCountPtr)
		    ++(*mUseCountPtr); 
	}

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
		
	template< class Y>
	SmartPointer(const SmartPointer<Y>& r): mPointer(0), mUseCountPtr(0)
	{
	    mPointer = r.GetPointer();
		mUseCountPtr = r.GetUseCountPtr();
	    // Handle zero pointer gracefully to manage STL containers
	    if(mUseCountPtr)
			++(*mUseCountPtr);
	}

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

	inline T& operator*() const { assert(mPointer); return *mPointer; }
	inline T* operator->() const { assert(mPointer); return mPointer; }

	inline T* Get() const { return mPointer; }
	inline T* GetPointer() const { return mPointer; }

	inline bool IsUnique() const { return mUseCount == 1; }

	inline uint32 GetUseCount() const { assert(mUseCountPtr); return *mUseCountPtr; }
	inline uint32* GetUseCountPtr() const { return mUseCountPtr; }

	inline bool IsNull(void) const { return mPointer == 0; }
    inline void SetNull(void)
	{ 
		if (mPointer)
		{
			Release();
			mPointer = 0;
		}
       }

protected:

	inline void Release(void)
    {
		bool destroyThis = false;
		if (--(*mUseCountPtr) == 0) 
			destroyThis = true;
		if (destroyThis)
			Destroy();
    }
    
	virtual void Destroy(void)
    {
		// IF YOU GET A CRASH HERE, YOU FORGOT TO FREE UP POINTERS BEFORE SHUTTING DOWN
        // Use SetNull() before shutdown or make sure your pointer goes out of scope to avoid this.
		DYN_DELETE mPointer;
		DYN_DELETE mUseCountPtr;
    }
	
	virtual void Swap(SmartPointer<T> &other) 
	{
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
	return std::less<const void*>()(a.Get(), b.Get());
}

#endif