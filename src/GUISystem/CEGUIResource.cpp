#include "Common.h"
#include "CEGUIResource.h"
#include "CEGUIDataContainer.h"
#include "../Utility/DataContainer.h"

using namespace GUISystem;

ResourceSystem::ResourcePtr CEGUIResource::CreateMe()
{
	return ResourceSystem::ResourcePtr(DYN_NEW CEGUIResource());
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
	//DYN_DELETE_ARRAY mDataBlock;
	return true;
}

void CEGUIResource::GetResource( CEGUI::RawDataContainer& outData )
{
	EnsureLoaded();
	/*
	uint8* mDataBlockCopy = DYN_NEW uint8[mDataBlockSize];
	memcpy(mDataBlockCopy, mDataBlock, mDataBlockSize);
	outData.setData(mDataBlockCopy);
	*/
	outData.setData(mDataBlock);
	outData.setSize(mDataBlockSize);
	Unload(true);
}