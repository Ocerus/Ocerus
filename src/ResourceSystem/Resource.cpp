#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "Resource.h"
#include "../Common.h"
#include "../Utility/DataContainer.h"
#include <vector>

using namespace ResourceSystem;

Resource::Resource(): mState(STATE_UNINITIALIZED), mInputStream(0) {}

Resource::~Resource()
{
	if (mInputStream)
	{
		DYN_DELETE mInputStream;
		mInputStream = 0;
	}
}

InputStream& Resource::OpenInputStream(eInputStreamMode mode)
{
	assert(mState != STATE_UNINITIALIZED);
	assert(boost::filesystem::exists(mFilePath) && "Resource file not found.");
	mInputStream = DYN_NEW boost::filesystem::ifstream(mFilePath, InputStreamMode(mode));
	assert(mInputStream);
	return *mInputStream;
}

void Resource::CloseInputStream()
{
	assert(mState != STATE_UNINITIALIZED);
	assert(mInputStream);
	mInputStream->close();
	DYN_DELETE mInputStream;
	mInputStream = 0;
}

bool Resource::Load()
{
	assert(mState == STATE_INITIALIZED);
	mState = STATE_LOADING;
	bool result = LoadImpl();
	if (result)
	{
		mState = STATE_LOADED;
	}
	else
	{
		mState = STATE_INITIALIZED;
		gLogMgr.LogMessage("Resource '" + mName + "' coult NOT be loaded", LOG_ERROR);
	}
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
	if (!result)
	{
		// we have a problem
		gLogMgr.LogMessage("Resource '" + mName + "' could NOT be unloaded", LOG_ERROR);
		assert(result && "Resource could NOT be unloaded");
	}
	return result;
}

void ResourceSystem::Resource::EnsureLoaded( void )
{
	if (mState != STATE_LOADED)
		Load();
}

void ResourceSystem::Resource::GetRawInputData( DataContainer& outData )
{
	outData.Release();
	InputStream& is = OpenInputStream(ISM_BINARY);
	std::vector<uint8> tmp;
	while (is.good())
		tmp.push_back(is.get());
	uint8* buffer = DYN_NEW uint8[tmp.size()];
	uint8* bufferPos = buffer;
	for (std::vector<uint8>::const_iterator i=tmp.begin(); i!=tmp.end(); ++i)
		*(bufferPos++) = *i;
	outData.SetData(buffer, tmp.size());
}