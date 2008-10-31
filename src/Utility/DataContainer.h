#ifndef DataContainer_h__
#define DataContainer_h__

#include "Settings.h"

class DataContainer
{
public:
	DataContainer(): mData(0), mSize(0)	{}

	~DataContainer(void) { Release(); }

	void SetData(uint8* data, int32 size) { mData = data; mSize = size; }

	uint8* GetData(void) { return mData; }
	const uint8* GetData(void) const { return mData; }
	int32 GetSize(void) const { return mSize; }

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