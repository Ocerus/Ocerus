/// @file
/// A singleton pattern implementation.

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#ifdef __WIN__
// disable VS warnings
#pragma warning (disable : 4311)
#pragma warning (disable : 4312)
#endif

namespace Utils
{
	/// A singleton pattern implementation taken from the Game Programming Gems.
	/// To make a class named C singleton, simply derive it from Singleton<C>.
	template <typename T> class Singleton
	{
	public:

		Singleton(void)
		{
			OC_ASSERT(!msSingleton);
			msSingleton = static_cast<T*>(this);
		}

		~Singleton(void)
		{
			OC_ASSERT(msSingleton);
			msSingleton = 0;
		}

		/// Constructs the singleton instance.
		static void CreateSingleton(void)
		{
			OC_ASSERT(!msSingleton);
			new T();
		}

		/// Constructs the singleton for abstract class. F must implement T.
		template <typename F>
		static void CreateSingleton(void)
		{
			OC_ASSERT(!msSingleton);
			new F();
		}

		/// Destructs the singleton instance.
		static void DestroySingleton(void)
		{
			OC_ASSERT(msSingleton);
			delete msSingleton;
		}

		/// Returns true if the singleton exists.
		static bool SingletonExists(void)
		{
			return msSingleton != 0;
		}

		/// Returns reference to the singleton instance.
		static T& GetSingleton(void)
		{
			OC_DASSERT(msSingleton);
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