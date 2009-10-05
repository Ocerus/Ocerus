/// @file
/// Globally accessible variables stored in an organized way.

#ifndef GlobalProperties_h__
#define GlobalProperties_h__

#include "Base.h"

namespace Utils
{
	class GlobalProperties
	{
	public:

		static void SetPointer(const StringKey key, void* value)
		{
			mProperties[key] = value;
		}

		template<typename T>
		static T* GetPointer(const StringKey key)
		{
			PropertyMap::const_iterator it = mProperties.find(key);
			BS_ASSERT_MSG(it != mProperties.end(), "Global property not found.");
			return (T*)(it->second);
		}

		template<typename T>
		static T& Get(const StringKey key)
		{
			PropertyMap::const_iterator it = mProperties.find(key);
			BS_ASSERT_MSG(it != mProperties.end(), "Global property not found.");
			T* ptr = (T*)(it->second);
			BS_ASSERT_MSG(ptr, "Global property pointer is null.");
			return *ptr;
		}

	private:

		typedef map<StringKey, void*> PropertyMap;

		static PropertyMap mProperties;

	};
}

#endif // GlobalProperties_h__