#include "Common.h"
#include "CEGUIResource.h"

#include "CEGUIDataContainer.h"
#include "DataContainer.h"

using namespace GUISystem;

ResourceSystem::ResourcePtr CEGUIResource::CreateMe()
{
	return ResourceSystem::ResourcePtr(new CEGUIResource());
}

size_t CEGUIResource::LoadImpl()
{
	GetRawInputData(mData);
	return mData.GetSize();
}

bool CEGUIResource::UnloadImpl()
{
	mData.Release();
	return true;
}

void CEGUIResource::Reload( void )
{
	//should contact CEGUI
}

/// The copy operation is omitted, only pointers are copied instead.
void CEGUIResource::GetResource(CEGUI::RawDataContainer& outData)
{
	EnsureLoaded();
	outData.setData(mData.GetData());
	outData.setSize(mData.GetSize());
	mData.SetData(0, 0);
	Unload(true);
}