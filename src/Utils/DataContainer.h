#ifndef DataContainer_h__
#define DataContainer_h__

#include "Settings.h"

/** This class stores a pointer to a data buffer and its size. It is used to retrieve data from a function where it is passed via reference.
*/
class DataContainer
{
public:
	DataContainer(void): mData(0), mSize(0)	{}

	/// @name Note that the dtor should NOT destroy contained data.
	~DataContainer(void) {}

	/// @name Set the actual data.
	void SetData(uint8* data, int32 size) { mData = data; mSize = size; }

	/// @name Data getters.
	//@{
	uint8* GetData(void) { return mData; }
	const uint8* GetData(void) const { return mData; }
	int32 GetSize(void) const { return mSize; }
	//@}

	/// @name Destroy the data.
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

#endif // DataContainer_h__