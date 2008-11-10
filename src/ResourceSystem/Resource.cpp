#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "Resource.h"
#include "../Common.h"
#include "../Utility/DataContainer.h"
#include <vector>
#include <string>

using namespace ResourceSystem;

Resource::Resource(): mState(STATE_UNINITIALIZED), mInputFileStream(0), mInputMemoryStream(0), mMemoryLoc(0), mMemoryLen(0) {}

Resource::~Resource()
{
	if (mInputFileStream)
	{
		DYN_DELETE mInputFileStream;
		mInputFileStream = 0;
	}
	if (mInputMemoryStream)
	{
		DYN_DELETE mInputMemoryStream;
		mInputMemoryStream = 0;
	}
}

InputStream& Resource::OpenInputStream(eInputStreamMode mode)
{
	// currently opens the stream from a file
	assert(mState != STATE_UNINITIALIZED);
	if (mMemoryLoc)
	{
		mInputMemoryStream = DYN_NEW InputMemoryStream(std::string(static_cast<char*>(mMemoryLoc), mMemoryLen));
		assert(mInputMemoryStream);
		return *mInputMemoryStream;
	}
	else
	{
		assert(boost::filesystem::exists(mFilePath) && "Resource file not found.");
		mInputFileStream = DYN_NEW boost::filesystem::ifstream(mFilePath, InputStreamMode(mode));
		assert(mInputFileStream);
		return *mInputFileStream;
	}
}

void Resource::CloseInputStream()
{
	assert(mState != STATE_UNINITIALIZED);
	if (mMemoryLoc)
	{
		assert(mInputMemoryStream);
		DYN_DELETE mInputMemoryStream;
		mInputMemoryStream = 0;
	}
	else
	{
		assert(mInputFileStream);
		mInputFileStream->close();
		DYN_DELETE mInputFileStream;
		mInputFileStream = 0;
	}
}

bool Resource::Load()
{
	// wraps around LoadImpl and does some additional work
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
	// wraps around UnloadImpl and does some additional work
	assert(mState != STATE_UNINITIALIZED);
	if (mState == STATE_INITIALIZED)
		return true; // true as the data are not loaded
	mState = STATE_UNLOADING;
	bool result = UnloadImpl();
	mState = STATE_INITIALIZED;
	if (!result)
	{
		// we have a real problem if we can't dealloc a resource
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
	if (mMemoryLoc)
	{
		// if we point to a memory location, then fill this in
		uint8* tmpbuf = DYN_NEW uint8[mMemoryLen];
		memcpy(tmpbuf, mMemoryLoc, mMemoryLen);
		outData.SetData(tmpbuf, mMemoryLen);
	}
	else
	{
		// otherwise read data from a stream into a buffer
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
}
