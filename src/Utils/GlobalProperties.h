/// @file
/// Globally accessible variables stored in an organized way.

#ifndef GlobalProperties_h__
#define GlobalProperties_h__

#include "Base.h"
#include "StringKey.h"

namespace Utils
{
    /// Globally accessible variables identified by string identifiers.
    /// Only pointer can be stored this way. So, to make use of this you must have the data accessible somewhere
    /// else as well.
    class GlobalProperties
    {
    public:

        /// Sets new key-pointer pair.
        /// Don't store null pointers as it will cause assert.
        static void SetPointer(const StringKey key, void* value) {
            mProperties[key] = value;
        }

		/// Clears the pointer from the properties.
		static void RemovePointer(const StringKey key) {
			mProperties.erase(key);
		}

        /// Returns a stored pointer identified by the given string key.
        template<typename T>
        static T* GetPointer(const StringKey key) {
            PropertyMap::const_iterator it = mProperties.find(key);
            OC_ASSERT_MSG(it != mProperties.end(), "Global property not found.");
            return (T*)(it->second);
        }

        /// Returns reference to the object stored using a pointer identified by the given string key.
        /// If the stored pointer is null, an assert will appear.
        template<typename T>
        static T& Get(const StringKey key) {
            PropertyMap::const_iterator it = mProperties.find(key);
            OC_ASSERT_MSG(it != mProperties.end(), "Global property not found.");
            T* ptr = (T*)(it->second);
            OC_ASSERT_MSG(ptr, "Global property pointer is null.");
            return *ptr;
        }

    private:

        typedef map<StringKey, void*> PropertyMap;

        static PropertyMap mProperties;

    };
}

#endif // GlobalProperties_h__
