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
		if (mInputFileStream->is_open())
			gLogMgr.LogMessage("Resource '" + mName + "' was not closed", LOG_ERROR);
		DYN_DELETE mInputFileStream;
		mInputFileStream = 0;
	}
}

InputStream& Resource::OpenInputStream(eInputStreamMode mode)
{
	//TODO predelat tak, aby vyuzivalo GetRawInputData a ne naopak
	ASSERT(mState != STATE_UNINITIALIZED);
	ASSERT_MSG(boost::filesystem::exists(mFilePath), "Resource file not found.");
	ASSERT_MSG(!mInputFileStream, "Resource was not closed before reused");
	mInputFileStream = DYN_NEW boost::filesystem::ifstream(mFilePath, InputStreamMode(mode));
	ASSERT(mInputFileStream);
	return *mInputFileStream;
}

void Resource::CloseInputStream()
{
	ASSERT(mState != STATE_UNINITIALIZED);
	ASSERT(mInputFileStream);
	mInputFileStream->close();
	DYN_DELETE mInputFileStream;
	mInputFileStream = 0;
}

bool Resource::Load()
{
	// wraps around LoadImpl and does some additional work
	ASSERT(mState == STATE_INITIALIZED);
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
	ASSERT(mState != STATE_UNINITIALIZED);
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
		ASSERT_MSG(result, "Resource could NOT be unloaded");
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
			ASSERT(!"Access to non-loaded manual resource");
		}
		Load();
	}
}

void ResourceSystem::Resource::GetRawInputData( DataContainer& outData )
{
	//TODO predelat tak, aby vyuzilo toho, ze nekdy je znat pocet ctenych dat dopredu -> a pridat prislusnou metodu na zjisteni toho poctu
	outData.Release();
	std::vector<uint8*> tmps;
	const uint32 tmpMaxSize = 1024; // size of one tmp buffer
	uint32 tmpLastSize; // size of the last tmp buffer in the vector
	uint32 bufferSize = 0; // resulting size
	InputStream& is = OpenInputStream(ISM_BINARY);
	while (is.good())
	{
		uint8* tmpBuf = DYN_NEW uint8[tmpMaxSize];
		is.read((char*)tmpBuf, tmpMaxSize);
		tmpLastSize = is.gcount();
		bufferSize += tmpLastSize;
		tmps.push_back(tmpBuf);
	}
	CloseInputStream();
	uint8* buffer = DYN_NEW uint8[bufferSize];
	uint8* lastBufferPos = buffer;
	int32 numBufs = tmps.size();
	for (int32 i=0; i<numBufs; ++i)
	{
		uint32 copyCount = i==numBufs-1 ? tmpLastSize : tmpMaxSize;
		memcpy(lastBufferPos, tmps[i], copyCount);
		lastBufferPos += copyCount;
		DYN_DELETE_ARRAY tmps[i];
	}
	ASSERT(buffer+bufferSize == lastBufferPos);
	outData.SetData(buffer, bufferSize);
}
