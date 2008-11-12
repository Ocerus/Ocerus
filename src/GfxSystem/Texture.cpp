#include "Texture.h"
#include "../Utility/DataContainer.h"
#include "GfxRenderer.h"
#include <hge.h>

using namespace GfxSystem;

ResourceSystem::ResourcePtr Texture::CreateMe()
{
	return ResourceSystem::ResourcePtr(DYN_NEW Texture());
}

void Texture::Init()
{
	mHandle = 0;
}

bool Texture::LoadImpl()
{
	// get texture data
	DataContainer dc;
	GetRawInputData(dc);

	// load it to HGE
	mHandle = gGfxRenderer.mHGE->Texture_Load((const char*)dc.GetData(),dc.GetSize(),false);

	return true;
}

bool Texture::UnloadImpl()
{
	// free texture from HGE
	gGfxRenderer.mHGE->Texture_Free(mHandle);

	// set ini value
	mHandle = 0;
	
	return true;
}

uint32 Texture::GetWidth()
{
	return gGfxRenderer.mHGE->Texture_GetWidth(mHandle,true);
}

uint32 Texture::GetHeight()
{
	return gGfxRenderer.mHGE->Texture_GetHeight(mHandle,true);
}

uint64 Texture::GetTexture()
{
	EnsureLoaded();

	return mHandle;
}

bool Texture::LoadFromBitmap( const void* pixels, uint32 pixelsLength, uint32 width, uint32 height, ePixelFormat format )
{
	SetState(STATE_LOADING);
	//TODO
	SetState(STATE_LOADED);
	return false;
}