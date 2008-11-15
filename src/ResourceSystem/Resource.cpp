#include "Common.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "Resource.h"
#include "../Utility/DataContainer.h"
#include <vector>
#include <string>

using namespace ResourceSystem;

Resource::Resource(): mState(STATE_UNINITIALIZED), mInputFileStream(0), mIsManual(false) {}

Resource::~Resource()
{
	if (mState >= STATE_LOADING)
		gLogMgr.LogMessage("Memory leak detected - resource '" + mName +"' was not unloaded before destroyed", LOG_ERROR);
	if (mInputFileStream)
	{
		DYN_DELETE mInputFileStream;
		mInputFileStream = 0;
	}
}

InputStream& Resource::OpenInputStream(eInputStreamMode mode)
{
	// currently opens the stream from a file
	assert(mState != STATE_UNINITIALIZED);
	assert(boost::filesystem::exists(mFilePath) && "Resource file not found.");
	mInputFileStream = DYN_NEW boost::filesystem::ifstream(mFilePath, InputStreamMode(mode));
	assert(mInputFileStream);
	return *mInputFileStream;
}

void Resource::CloseInputStream()
{
	assert(mState != STATE_UNINITIALIZED);
	assert(mInputFileStream);
	mInputFileStream->close();
	DYN_DELETE mInputFileStream;
	mInputFileStream = 0;
}

bool Resource::Load()
{
	// wraps around LoadImpl and does some additional work
	assert(mState == STATE_INITIALIZED);
	if (mIsManual)
		return false; // manual resources must be loaded by the user
	mState = STATE_LOADING;
	gLogMgr.LogMessage("Loading resource '"+mName+"'");
	bool result = LoadImpl();
	if (result)
	{
		mState = STATE_LOADED;
		gLogMgr.LogMessage("Resource '"+mName+"' loaded");
	}
	else
	{
		mState = STATE_INITIALIZED;
		gLogMgr.LogMessage("Resource '" + mName + "' coult NOT be loaded", LOG_ERROR);
	}
	return result;
}

bool Resource::Unload(bool allowManual)
{
	// wraps around UnloadImpl and does some additional work
	assert(mState != STATE_UNINITIALIZED);
	if (mState == STATE_INITIALIZED)
		return true; // true as the data are not loaded
	if (mIsManual && !allowManual)
	{
		gLogMgr.LogMessage("Can't unloade manual resource '"+mName+"'");
		return false; // we can't unload manual resources as we won't be able to reload them later
	}
	mState = STATE_UNLOADING;
	gLogMgr.LogMessage("Unloading resource '"+mName+"'");
	bool result = UnloadImpl();
	mState = STATE_INITIALIZED;
	if (!result)
	{
		// we have a real problem if we can't dealloc a resource
		gLogMgr.LogMessage("Resource '" + mName + "' could NOT be unloaded", LOG_ERROR);
		assert(result && "Resource could NOT be unloaded");
	} else 
	{
		gLogMgr.LogMessage("Resource '"+mName+"' unloaded");
	}
	return result;
}

void ResourceSystem::Resource::EnsureLoaded( void )
{
	if (mState != STATE_LOADED)
	{
		if (IsManual())
		{
			gLogMgr.LogMessage("Access to non-loaded manual resource '" + mName +"'", LOG_ERROR);
			assert(!"Access to non-loaded manual resource");
		}
		Load();
	}
}

void ResourceSystem::Resource::GetRawInputData( DataContainer& outData )
{
	outData.Release();
	InputStream& is = OpenInputStream(ISM_BINARY);
	std::vector<uint8> tmp;
	while (is.good())
		tmp.push_back(is.get());
	uint8* buffer = DYN_NEW uint8[tmp.size() - 1];
	uint8* bufferPos = buffer;
	for (std::vector<uint8>::const_iterator i=tmp.begin(); (i + 1)!=tmp.end(); ++i)
		*(bufferPos++) = *i;
	outData.SetData(buffer, tmp.size() - 1);
}
