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

uint32 Texture::GetWidth(bool bOriginal/* = false*/)
{
	EnsureLoaded();
	return gGfxRenderer.mHGE->Texture_GetWidth(mHandle, bOriginal);
}

uint32 Texture::GetHeight(bool bOriginal/* = false*/)
{
	EnsureLoaded();
	return gGfxRenderer.mHGE->Texture_GetHeight(mHandle, bOriginal);
}

uint64 Texture::GetTexture()
{
	EnsureLoaded();

	return mHandle;
}

bool Texture::LoadFromBitmap( const void* pixels, uint32 pixelsLength, uint32 width, uint32 height, ePixelFormat format )
{
	SetState(STATE_LOADING);
	
	this->mFormat = format;
	HTEXTURE tex = gGfxRenderer.mHGE->Texture_Create(width, height);
	mHandle = tex;
	DWORD* texture_data = gGfxRenderer.mHGE->Texture_Lock(tex, false);

	memcpy(texture_data, pixels, pixelsLength);
	gGfxRenderer.mHGE->Texture_Unlock(tex);
	
	SetState(STATE_LOADED);

	return (tex != 0)?true:false;
}