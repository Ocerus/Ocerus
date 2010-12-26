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

InputStream* Resource::OpenInputStream(eInputStreamMode mode)
{
	return OpenInputStream(mFilePath, mode);
}

InputStream* Resource::OpenInputStream(const string filePath, eInputStreamMode mode)
{
	OC_ASSERT(mState != STATE_UNINITIALIZED);
	//OC_ASSERT_MSG(boost::filesystem::exists(filePath), "Resource file not found.");

	if (!boost::filesystem::exists(filePath))
		return NULL;

	OC_ASSERT_MSG(!mInputFileStream, "Resource was not closed before reused");
	mInputFileStream = new boost::filesystem::ifstream(filePath, InputStreamMode(mode));
	OC_ASSERT(mInputFileStream);
	return mInputFileStream;
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

	bool miss = (GetState() == STATE_MISSING);

	if ((GetState() != STATE_INITIALIZED) && !miss)
	{
	  ocError << "Resource '" << mName << "' is NOT initialized!";
	  return false;
	}
	if (mIsManual) return false; // manual resources must be loaded by the user

	// make sure the resource mgr is synchronized
	gResourceMgr._NotifyResourceLoadingStarted(this);

	SetState(STATE_LOADING);
	ocTrace << "Loading resource '" << mName << "'";
	RefreshResourceInfo();
	mSizeInBytes = LoadImpl();
	bool loadSuccessful = mSizeInBytes != 0;
	if (loadSuccessful)
	{
		if (!miss)
			SetState(STATE_LOADED);
		else
			SetState(STATE_MISSING_LOADED);
		ocInfo << "Resource '" << mName << "' loaded";
		// make sure the resource mgr is synchronized
		gResourceMgr._NotifyResourceLoaded(this);
	}
	else
	{
		SetState(STATE_FAILED);
		ocError << "Resource '" << mName << "' coult NOT be loaded";
	}
	return loadSuccessful;
}

bool Resource::Unload(bool allowManual)
{
	// wraps around UnloadImpl and does some additional work
	if (mState == STATE_UNINITIALIZED)
	{
	  ocError << "Resource '" << mName << "' is NOT initialized!";
	  return false;
	}
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
	ocTrace << "Unloading resource '" << mName << "'";
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
	// if loading failed, no need to try it again
	if (mState == STATE_FAILED)
		return;

	// mark this resource as being used
	if (mLastUsedTime != sLastUsedTime)
	{
		++sLastUsedTime;
		mLastUsedTime = sLastUsedTime;
	}

	// if the resource is not load it, then load it
	if ((mState != STATE_LOADED) && (mState != STATE_MISSING_LOADED))
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

bool ResourceSystem::Resource::GetRawInputData( DataContainer& outData )
{
	return GetRawInputData(mFilePath, outData);
}

bool ResourceSystem::Resource::GetRawInputData( const string filePath, DataContainer& outData )
{
	// The data is read in small chunks and the resulting buffer is then composed by merging the chunks together.

	outData.Release();
	vector<uint8*> tmps;
	const uint32 tmpMaxSize = 2048; // size of one tmp buffer
	uint32 tmpLastSize = 0; // size of the last tmp buffer in the vector
	uint32 bufferSize = 0; // resulting size
	InputStream* is = OpenInputStream(filePath, ISM_BINARY);

	if (is == NULL)
		return false;

	while (is->good())
	{
		uint8* tmpBuf = new uint8[tmpMaxSize];
		is->read((char*)tmpBuf, tmpMaxSize);
		tmpLastSize = is->gcount();
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
	return true;
}

bool ResourceSystem::Resource::Refresh( void )
{
	if (!boost::filesystem::exists(mFilePath))
	{
		if ((GetState() == STATE_MISSING)||(GetState() == STATE_MISSING_LOADED))
			return true;

		ocInfo << "Resource file " << mFilePath << " went missing. Unloading the resource.";
		if (GetState() == STATE_LOADED)
		{
			Unload();
			SetState(STATE_MISSING);
			return true;
		}
		return false;
	}

	int64 currentWriteTime = 0;
	try { currentWriteTime = boost::filesystem::last_write_time(mFilePath); }
	catch (boost::exception&) 
	{ 
		ocWarning << "Resource file " << mFilePath << " went missing after the resource was loaded";
		if (GetState() == STATE_LOADED)
		{
			Unload();
			SetState(STATE_MISSING);
			mLastWriteTime = 0;
			return true;
		}
		return false;
	}
	if (currentWriteTime > mLastWriteTime)
	{
		ocInfo << "Refreshing resource " << mName << " from " << mFilePath;
		Reload();
		mLastWriteTime = currentWriteTime;
	}

	return true;
}

void ResourceSystem::Resource::RefreshResourceInfo( void )
{
	mLastWriteTime = 0;
	try { mLastWriteTime = boost::filesystem::last_write_time(mFilePath); }
	catch (boost::exception&) { ocWarning << "Resource file " << mFilePath << " went missing after the resource was loaded"; }
}

void ResourceSystem::Resource::Reload( void )
{
	if ((GetState() != STATE_LOADED)&&(GetState() != STATE_MISSING_LOADED))
	{
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

string Resource::GetRelativeFilePath(void ) const
{
	string filePath = GetFilePath();
	const string& basePath = gResourceMgr.GetBasePath(mBasePathType);
	return filePath.size() >= basePath.size() ? filePath.substr(basePath.size()) : filePath;
}

string ResourceSystem::Resource::GetFileDir( void ) const
{
	boost::filesystem::path filePath(GetFilePath());
	string result = filePath.branch_path().native_directory_string();
	return result;
}

string ResourceSystem::Resource::GetRelativeFileDir( void ) const
{
	string fileDir = GetFileDir();
	if (fileDir.at(fileDir.size() - 1) != '/')
		fileDir.append("/");
	const string& basePath = gResourceMgr.GetBasePath(mBasePathType);
	return fileDir.size() >= basePath.size() ? fileDir.substr(basePath.size()) : fileDir;
}
