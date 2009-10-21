#include "Common.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "Resource.h"
#include "DataContainer.h"

using namespace ResourceSystem;

Resource::Resource(): mIsManual(false), mState(STATE_UNINITIALIZED), mInputFileStream(0) {}

Resource::~Resource()
{
	if (mState >= STATE_LOADING)
		gLogMgr.LogMessage("Memory leak detected - resource '" + mName +"' was not unloaded before destroyed", LOG_ERROR);
	if (mInputFileStream)
	{
		if (mInputFileStream->is_open())
			gLogMgr.LogMessage("Resource '" + mName + "' was not closed", LOG_ERROR);
		delete mInputFileStream;
		mInputFileStream = 0;
	}
}

InputStream& Resource::OpenInputStream(eInputStreamMode mode)
{
	BS_ASSERT(mState != STATE_UNINITIALIZED);
	BS_ASSERT_MSG(boost::filesystem::exists(mFilePath), "Resource file not found.");
	BS_ASSERT_MSG(!mInputFileStream, "Resource was not closed before reused");
	mInputFileStream = new boost::filesystem::ifstream(mFilePath, InputStreamMode(mode));
	BS_ASSERT(mInputFileStream);
	return *mInputFileStream;
}

void Resource::CloseInputStream()
{
	BS_ASSERT(mState != STATE_UNINITIALIZED);
	BS_ASSERT(mInputFileStream);
	mInputFileStream->close();
	delete mInputFileStream;
	mInputFileStream = 0;
}

bool Resource::Load()
{
	// wraps around LoadImpl and does some additional work
	BS_ASSERT(GetState() == STATE_INITIALIZED);
	if (mIsManual)
		return false; // manual resources must be loaded by the user
	SetState(STATE_LOADING);
	gLogMgr.LogMessage("Loading resource '"+mName+"'");
	bool result = LoadImpl();
	if (result)
	{
		SetState(STATE_LOADED);
		gLogMgr.LogMessage("Resource '"+mName+"' loaded");
	}
	else
	{
		SetState(STATE_INITIALIZED);
		gLogMgr.LogMessage("Resource '" + mName + "' coult NOT be loaded", LOG_ERROR);
	}
	return result;
}

bool Resource::Unload(bool allowManual)
{
	// wraps around UnloadImpl and does some additional work
	BS_ASSERT(mState != STATE_UNINITIALIZED);
	if (GetState() == STATE_INITIALIZED)
		return true; // true as the data are not loaded
	if (mIsManual && !allowManual)
	{
		gLogMgr.LogMessage("Can't unloade manual resource '"+mName+"'");
		return false; // we can't unload manual resources as we won't be able to reload them later
	}
	SetState(STATE_UNLOADING);
	gLogMgr.LogMessage("Unloading resource '"+mName+"'");
	bool result = UnloadImpl();
	SetState(STATE_INITIALIZED);
	if (!result)
	{
		// we have a real problem if we can't dealloc a resource
		gLogMgr.LogMessage("Resource '" + mName + "' could NOT be unloaded", LOG_ERROR);
		BS_ASSERT_MSG(result, "Resource could NOT be unloaded");
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
			BS_ASSERT(!"Access to non-loaded manual resource");
		}
		Load();
	}
}

void ResourceSystem::Resource::GetRawInputData( DataContainer& outData )
{
	outData.Release();
	vector<uint8*> tmps;
	const uint32 tmpMaxSize = 1024; // size of one tmp buffer
	uint32 tmpLastSize = 0; // size of the last tmp buffer in the vector
	uint32 bufferSize = 0; // resulting size
	InputStream& is = OpenInputStream(ISM_BINARY);
	while (is.good())
	{
		uint8* tmpBuf = new uint8[tmpMaxSize];
		is.read((char*)tmpBuf, tmpMaxSize);
		tmpLastSize = is.gcount();
		bufferSize += tmpLastSize;
		tmps.push_back(tmpBuf);
	}
	CloseInputStream();
	uint8* buffer = new uint8[bufferSize];
	uint8* lastBufferPos = buffer;
	int32 numBufs = tmps.size();
	for (int32 i=0; i<numBufs; ++i)
	{
		uint32 copyCount = i==numBufs-1 ? tmpLastSize : tmpMaxSize;
		memcpy(lastBufferPos, tmps[i], copyCount);
		lastBufferPos += copyCount;
		delete[] tmps[i];
	}
	BS_ASSERT(buffer+bufferSize == lastBufferPos);
	outData.SetData(buffer, bufferSize);
}
