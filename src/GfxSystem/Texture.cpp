#include "Common.h"
#include "Texture.h"
#include "DataContainer.h"

using namespace GfxSystem;

Texture::Texture( void ): mHandle(0) {}

Texture::~Texture( void ) {}

ResourceSystem::ResourcePtr Texture::CreateMe()
{
	return ResourceSystem::ResourcePtr(new Texture());
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

	// load it to OpenGL
	mHandle = gGfxRenderer.LoadTexture((const unsigned char*)dc.GetData(),dc.GetSize(), RGB, 0);

	// we don't need the data buffer anymore
	dc.Release();


	if (mHandle != 0)
		return true;
	else
		return false;
}

bool Texture::UnloadImpl()
{
	// free texture from OpenGL
	gGfxRenderer.DeleteTexture(mHandle);
	mHandle = 0;
	
	return true;
}

uint32 Texture::GetWidth(/*bool bOriginal*/)
{
	EnsureLoaded();
	//TODO
	return 0;//gGfxRenderer.mHGE->Texture_GetWidth(mHandle, false);
}

uint32 Texture::GetHeight(/*bool bOriginal */)
{
	EnsureLoaded();
	//TODO
	return 0;//gGfxRenderer.mHGE->Texture_GetHeight(mHandle, false);
}

uint32 Texture::GetTexture()
{
	EnsureLoaded();
	return mHandle;
}

bool Texture::LoadFromBitmap( const void* pixels, uint32 pixelsLength, uint32 width, uint32 height, ePixelFormat format )
{
	//TODO
	/*SetState(STATE_LOADING);
	
	mFormat = format;
	HTEXTURE tex = gGfxRenderer.mHGE->Texture_Create(width, height);
	mHandle = tex;
	DWORD* texture_data = gGfxRenderer.mHGE->Texture_Lock(tex, false);
	OC_ASSERT(texture_data);
	memcpy(texture_data, pixels, pixelsLength);
	gGfxRenderer.mHGE->Texture_Unlock(tex);
	
	SetState(STATE_LOADED);

	return (tex != 0)?true:false;*/
	return false;
}

