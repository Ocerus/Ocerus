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
	// needs fixing!!!
	DYN_DELETE_ARRAY mDataBlock;
	return true;
}

void CEGUIResource::GetResource( CEGUI::RawDataContainer& outData )
{
	EnsureLoaded();
	outData.setData(mDataBlock);
	outData.setSize(mDataBlockSize);
}