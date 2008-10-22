#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "Resource.h"
#include "../Common.h"

using namespace ResourceSystem;

Resource::Resource(): mState(STATE_UNINITIALIZED), mInputStream(0) {}

Resource::~Resource()
{
	if (mInputStream)
	{
		DELETE mInputStream;
		mInputStream = 0;
	}
}

InputStream& Resource::OpenInputStream()
{
	assert(mState != STATE_UNINITIALIZED);
	assert(boost::filesystem::exists(mFilePath) && "Resource file not found.");
	mInputStream = NEW boost::filesystem::ifstream(mFilePath);
	assert(mInputStream);
	return *mInputStream;
}

void Resource::CloseInputStream()
{
	assert(mState != STATE_UNINITIALIZED);
	assert(mInputStream);
	mInputStream->close();
	DELETE mInputStream;
	mInputStream = 0;
}

bool Resource::Load()
{
	assert(mState == STATE_INITIALIZED);
	mState = STATE_LOADING;
	bool result = LoadImpl();
	mState = STATE_LOADED;
	return result;
}

bool Resource::Unload()
{
	assert(mState != STATE_UNINITIALIZED);
	if (mState == STATE_INITIALIZED)
		return true; // true as the data are not loaded
	mState = STATE_UNLOADING;
	bool result = UnloadImpl();
	mState = STATE_INITIALIZED;
	return result;
}
