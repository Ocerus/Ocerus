#include "CEGUIResource.h"

using namespace GUISystem;

CEGUIResource::CEGUIResource(): mState(STATE_UNINITIALIZED), mInputStream(0) {}

CEGUIResource::~CEGUIResource()
{
	if (mInputStream)
	{
		DYN_DELETE mInputStream;
		mInputStream = 0;
	}
}

InputStream& CEGUIResource::OpenInputStream()
{
	assert(mState != STATE_UNINITIALIZED);
	assert(boost::filesystem::exists(mFilePath) && "Resource file not found.");
	mInputStream = DYN_NEW boost::filesystem::ifstream(mFilePath);
	assert(mInputStream);
	return *mInputStream;
}

void CEGUIResource::CloseInputStream()
{
	assert(mState != STATE_UNINITIALIZED);
	assert(mInputStream);
	mInputStream->close();
	DYN_DELETE mInputStream;
	mInputStream = 0;
}

bool CEGUIResource::Load()
{
	assert(mState == STATE_INITIALIZED);
	mState = STATE_LOADING;
	bool result = LoadImpl();
	mState = STATE_LOADED;
	return result;
}

bool CEGUIResource::Unload()
{
	assert(mState != STATE_UNINITIALIZED);
	if (mState == STATE_INITIALIZED)
		return true; // true as the data are not loaded
	mState = STATE_UNLOADING;
	bool result = UnloadImpl();
	mState = STATE_INITIALIZED;
	return result;
}

void CEGUIResource::EnsureLoaded( void )
{
	if (mState != STATE_LOADED)
		Load();
}
