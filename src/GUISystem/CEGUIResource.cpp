#include "CEGUIResource.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

using namespace GUISystem;
namespace fs = boost::filesystem;

ResourceSystem::ResourcePtr CEGUIResource::CreateMe()
{
	return ResourceSystem::ResourcePtr(DYN_NEW CEGUIResource());
}

void CEGUIResource::_GetCEGUIResource()
{
	EnsureLoaded();
}

//returns new datablock reference
DataBlock * CEGUIResource::AddNewDataBlock(void) {
	return AddNewDataBlock(INT_MAX);
}

DataBlock * CEGUIResource::AddNewDataBlock(int size) {
	if (mDataBlocks == 0) {		
		mLastDataBlock = mDataBlocks = DYN_NEW DataBlock;
		mDataBlocks->payload = DYN_NEW uint8[size];
		mDataBlocks->prev = 0;
	} else {
		mLastDataBlock->next = DYN_NEW DataBlock;
		mLastDataBlock->next->prev = mLastDataBlock;
		mLastDataBlock = mLastDataBlock->next;
		mLastDataBlock->payload = DYN_NEW uint8[size];
	}
	mDataBlocks->next = 0;
	return mLastDataBlock;
}

bool CEGUIResource::LoadImpl()
{
	boost::uintmax_t filesize;
	OpenInputStream(ISM_BINARY);

	
	fs::path p(mFilePath, fs::native);

	assert(fs::exists(p) && fs::is_regular (p));

	filesize = fs::file_size( p );	
	mDataBlocks = 0;
	while (filesize > 0) {
		if (filesize > INT_MAX) {
			mLastDataBlock = AddNewDataBlock();
			mInputStream->read((char*)(mLastDataBlock->payload), INT_MAX);
		} else {
			mLastDataBlock = AddNewDataBlock((int)filesize);
			mInputStream->read((char*)(mLastDataBlock->payload), (int)filesize);
		}
		filesize -= INT_MAX;
	}

	CloseInputStream();
	return true;
}

bool CEGUIResource::UnloadImpl()
{
	while (mDataBlocks) {
		DataBlock * last;
		DYN_DELETE_ARRAY mDataBlocks->payload;
		last = mDataBlocks;
		mDataBlocks = mDataBlocks->next;
		DYN_DELETE last;
	}
	return true;
}
