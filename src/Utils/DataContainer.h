/// @file
/// Carries arbitrary data and its size.

#ifndef DataContainer_h__
#define DataContainer_h__

#include "Base.h"

/// Utility classes and functions.
namespace Utils
{
	/// This class stores a pointer to a data buffer and its size. It is used to retrieve data from a function
	/// where it is passed via reference.
	/// Note that the class doesn't allocate nor deallocate any memory until requested by calling Release.
	class DataContainer
	{
	public:

		DataContainer(void): mData(0), mSize (0) {}

		/// Note that the destructor doesn't destroy the carried data.
		~DataContainer(void) {}

		/// Sets the data to be carried. The pointer to such data must be obtained
		/// with new[] operator.
		inline void SetData(uint8* data, int32 size) { mData = data; mSize = size; }

		/// Returns the carried data pointer.
		inline uint8* GetData(void) { return mData; }

		/// Returns the carried data pointer.
		inline const uint8* GetData(void) const { return mData; }

		/// Returns the size of the carried data.
		inline int32 GetSize(void) const { return mSize; }

		/// Destroys the carried data.
		void Release(void)
		{
			if (!mData)
				return;
			delete[] mData;
			mData = 0;
			mSize = 0;
		}

	private:
		uint8* mData;
		int32 mSize;
	};
}

#endif // DataContainer_h__