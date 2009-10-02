/// @file
/// Carries arbitrary data and its size.

#ifndef DataContainer_h__
#define DataContainer_h__

#include "Settings.h"

/// This namespace contains utility classes and functions.
namespace Utils
{
	/// This class stores a pointer to a data buffer and its size. It is used to retrieve data from a function where it is passed via reference.
	class DataContainer
	{
	public:

		DataContainer(void): mData(0), mSize (0) {}

		/// @remarks Note that the destructor doesn't destroy the carried data.
		~DataContainer(void) {}

		/// Sets the data to be carried.
		void SetData(uint8* data, int32 size) { mData = data; mSize = size; }

		/// Returns the carried data pointer.
		uint8* GetData(void) { return mData; }

		/// Returns the carried data pointer.
		const uint8* GetData(void) const { return mData; }

		/// Returns the size of the carried data.
		int32 GetSize(void) const { return mSize; }

		/// Destroys the carried data.
		void Release(void)
		{
			if (!mData)
				return;
			DYN_DELETE_ARRAY mData;
			mData = 0;
			mSize = 0;
		}

	private:
		uint8* mData;
		int32 mSize;
	};
}

#endif // DataContainer_h__