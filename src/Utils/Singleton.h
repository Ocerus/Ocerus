#ifndef _SINGLETON_H_
#define _SINGLETON_H_

// disable VS warnings
#pragma warning (disable : 4311)
#pragma warning (disable : 4312)

namespace Utils
{
	/// @brief A singleton pattern implementation taken from the Game Programming Gems.
	/// @remarks To make a class named C singleton, simply derive it from Singleton<C>.
	template <typename T> class Singleton
	{
	public:

		Singleton(void)
		{
			BS_ASSERT(!msSingleton);
			msSingleton = static_cast<T*>(this);
		}

		~Singleton(void)
		{
			BS_ASSERT(msSingleton);
			msSingleton = 0;
		}

		/// Constructs the singleton instance.
		static void CreateSingleton(void)
		{
			BS_ASSERT(!msSingleton);
			DYN_NEW T();
		}

		/// Destructs the singleton instance.
		static void DestroySingleton(void)
		{
			BS_ASSERT(msSingleton);
			DYN_DELETE msSingleton;
		}

		/// Returns reference to the singleton instance.
		static T& GetSingleton(void)
		{
			BS_DASSERT(msSingleton);
			return (*msSingleton);
		}

		/// Returns a pointer to the singleton instance.
		static T* GetSingletonPtr(void)
		{  
			return (msSingleton);  
		}

	private:
		static T* msSingleton;
	};
}

template <typename T> T* Singleton <T>::msSingleton = 0;

#endif