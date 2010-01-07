#include "Common.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "Resource.h"
#include "DataContainer.h"
#include <cstring>

using namespace ResourceSystem;

uint64 Resource::sLastUsedTime;

Resource::Resource():
	mLastUsedTime(0),
	mIsManual(false),
	mState(STATE_UNINITIALIZED),
	mSizeInBytes(0),
	mInputFileStream(0)

{

}

Resource::~Resource()
{
	if (mState >= STATE_LOADING)
		ocWarning << "Memory leak detected - resource '" << mName << "' was not unloaded before destroyed";
	if (mInputFileStream)
	{
		if (mInputFileStream->is_open())
			ocWarning << "Resource '" << mName << "' was not closed before deleting";
		delete mInputFileStream;
		mInputFileStream = 0;
	}
}

InputStream& Resource::OpenInputStream(eInputStreamMode mode)
{
	OC_ASSERT(mState != STATE_UNINITIALIZED);
	OC_ASSERT_MSG(boost::filesystem::exists(mFilePath), "Resource file not found.");
	OC_ASSERT_MSG(!mInputFileStream, "Resource was not closed before reused");
	mInputFileStream = new boost::filesystem::ifstream(mFilePath, InputStreamMode(mode));
	OC_ASSERT(mInputFileStream);
	return *mInputFileStream;
}

void Resource::CloseInputStream()
{
	OC_ASSERT(mState != STATE_UNINITIALIZED);
	OC_ASSERT(mInputFileStream);
	mInputFileStream->close();
	delete mInputFileStream;
	mInputFileStream = 0;
}

bool Resource::Load()
{
	// wraps around LoadImpl and does some additional work
	OC_ASSERT(GetState() == STATE_INITIALIZED);
	if (mIsManual)
		return false; // manual resources must be loaded by the user
	SetState(STATE_LOADING);
	ocInfo << "Loading resource '" << mName << "'";
	RefreshResourceInfo();
	mSizeInBytes = LoadImpl();
	bool loadSuccessful = mSizeInBytes != 0;
	if (loadSuccessful)
	{
		SetState(STATE_LOADED);
		ocInfo << "Resource '" << mName << "' loaded";
		// make sure the resource mgr is synchronized
		gResourceMgr._NotifyResourceLoaded(this);
	}
	else
	{
		SetState(STATE_INITIALIZED);
		ocError << "Resource '" << mName << "' coult NOT be loaded";
	}
	return loadSuccessful;
}

bool Resource::Unload(bool allowManual)
{
	// wraps around UnloadImpl and does some additional work
	OC_ASSERT(mState != STATE_UNINITIALIZED);
	// make sure the resource mgr is synchronized
	gResourceMgr._NotifyResourceUnloaded(this);
	if (GetState() == STATE_INITIALIZED)
		return true; // true as the data are not loaded
	if (mIsManual && !allowManual)
	{
		ocError << "Can't unload manual resource '" << mName << "'";
		return false; // we can't unload manual resources as we won't be able to reload them later
	}
	SetState(STATE_UNLOADING);
	ocInfo << "Unloading resource '" << mName << "'";
	bool result = UnloadImpl();
	SetState(STATE_INITIALIZED);
	if (!result)
	{
		// we have a real problem if we can't dealloc a resource
		ocError << "Resource '" << mName << "' could NOT be unloaded";
	}
	else
	{
		ocInfo << "Resource '" << mName << "' unloaded";
	}
	return result;
}

void ResourceSystem::Resource::EnsureLoaded( void )
{
	// mark this resource as being used
	if (mLastUsedTime != sLastUsedTime)
	{
		++sLastUsedTime;
		mLastUsedTime = sLastUsedTime;
	}

	// if the resource is not load it, then load it
	if (mState != STATE_LOADED)
	{
		if (IsManual())
		{
			ocError << "Access to non-loaded manual resource '" << mName << "'";
		}
		else
		{
			Load();
		}
	}
}

void ResourceSystem::Resource::GetRawInputData( DataContainer& outData )
{
	// The data is read in small chunks and the resulting buffer is then composed by merging the chunks together.

	outData.Release();
	vector<uint8*> tmps;
	const uint32 tmpMaxSize = 2048; // size of one tmp buffer
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
	OC_ASSERT(buffer+bufferSize == lastBufferPos);
	outData.SetData(buffer, bufferSize);
}

void ResourceSystem::Resource::Refresh( void )
{
	if (GetState() == STATE_LOADED)
	{
		int64 currentWriteTime = 0;
		try { currentWriteTime = boost::filesystem::last_write_time(mFilePath); }
		catch (boost::exception&) { ocWarning << "Resource file " << mFilePath << " went missing after the resource was loaded"; }
		if (currentWriteTime > mLastWriteTime)
		{
			ocInfo << "Refreshing resource " << mName << " from " << mFilePath;
			Reload();
		}
	}
}

void ResourceSystem::Resource::RefreshResourceInfo( void )
{
	mLastWriteTime = 0;
	try { mLastWriteTime = boost::filesystem::last_write_time(mFilePath); }
	catch (boost::exception&) { ocWarning << "Resource file " << mFilePath << " went missing after the resource was loaded"; }
}

void ResourceSystem::Resource::Reload( void )
{
	if (GetState() != STATE_LOADED)
	{
		ocError << "Resource " << mName << " cannot be reloaded; it is not loaded";
		return;
	}
	Unload();
	Load();
}

size_t ResourceSystem::Resource::GetSize( void ) const
{
	if (GetState() == STATE_LOADED) return mSizeInBytes;
	else return 0;
}