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

size_t Texture::LoadImpl()
{
	// get texture data
	DataContainer dc;
	GetRawInputData(dc);
	
	// load it to OpenGL
	int width, height;
	mHandle = gGfxRenderer.LoadTexture((const unsigned char*)dc.GetData(),
										dc.GetSize(),
										PF_AUTO, 0,			//pixel format, reuse texture handle (0 = create new)
										&width, &height);

	mWidth = width;
	mHeight = height;

	// we don't need the data buffer anymore
	size_t dataSize = dc.GetSize();
	dc.Release();

	if (!mHandle) return 0;

	return dataSize;
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
	return mWidth;
}

uint32 Texture::GetHeight(/*bool bOriginal */)
{
	EnsureLoaded();
	return mHeight;
}

uint32 Texture::GetTexture()
{
	EnsureLoaded();
	return mHandle;
}