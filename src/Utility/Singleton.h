#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <cassert>

// disable VS warning
#pragma warning (disable : 4311)
#pragma warning (disable : 4312)

template <typename T> class Singleton
{
    static T* msSingleton;

public:
    Singleton(void)
    {
        assert( !msSingleton );
        int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
        msSingleton = (T*)((int)this + offset);
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