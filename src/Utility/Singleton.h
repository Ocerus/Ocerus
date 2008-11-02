#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <cassert>

// disable VS warnings
#pragma warning (disable : 4311)
#pragma warning (disable : 4312)

/** Singleton pattern implementation taken from GPG.
*/
template <typename T> class Singleton
{
    static T* msSingleton;

public:
    Singleton(void)
    {
        assert( !msSingleton );
		msSingleton = static_cast<T*>(this);
    }
    
	~Singleton(void)
	{
		assert( msSingleton );
		msSingleton = 0;
	}
    
	static T& GetSingleton(void)
    {
		assert(msSingleton);
		return (*msSingleton);
	}

    static T* GetSingletonPtr(void)
    {  
		return (msSingleton);  
	}
};

template <typename T> T* Singleton <T>::msSingleton = 0;

#endif