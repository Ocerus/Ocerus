#include "Common.h"
#include "CEGUIResource.h"
#include "CEGUIDataContainer.h"
#include "DataContainer.h"

using namespace GUISystem;

ResourceSystem::ResourcePtr CEGUIResource::CreateMe()
{
	return ResourceSystem::ResourcePtr(new CEGUIResource());
}

bool CEGUIResource::LoadImpl()
{
	DataContainer dc;
	GetRawInputData(dc);
	mDataBlock = dc.GetData();
	mDataBlockSize = dc.GetSize();
	return true;
}

bool CEGUIResource::UnloadImpl()
{
	//delete[] mDataBlock;
	return true;
}

void CEGUIResource::GetResource( CEGUI::RawDataContainer& outData )
{
	EnsureLoaded();
	/*
	uint8* mDataBlockCopy = new uint8[mDataBlockSize];
	memcpy(mDataBlockCopy, mDataBlock, mDataBlockSize);
	outData.setData(mDataBlockCopy);
	*/
	outData.setData(mDataBlock);
	outData.setSize(mDataBlockSize);
	Unload(true);
}