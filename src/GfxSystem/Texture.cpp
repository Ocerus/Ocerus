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
	
	// load it to low-level renderer
	int32 width, height;
	mHandle = gGfxRenderer.LoadTexture((const unsigned char*)dc.GetData(),
										dc.GetSize(),
										PF_RGBA, 0,			//pixel format, reuse texture handle (0 = create new)
										&width, &height);
	
	// we don't need the data buffer anymore
	size_t dataSize = dc.GetSize();
	dc.Release();

	// if loading texture fails, load NullTexture instead
	if (!mHandle)
	{
		ocWarning << "Texture '" << GetName() << "' cannot be loaded. Loading NullTexture instead!";

		ResourceSystem::ResourcePtr nullTexHandle = gResourceMgr.GetResource("General", "NullTexture");
		string filePath = nullTexHandle->GetFilePath();
		GetRawInputData(filePath, dc);

		mHandle = gGfxRenderer.LoadTexture((const unsigned char*)dc.GetData(),
											dc.GetSize(),
											PF_RGBA, 0,			//pixel format, reuse texture handle (0 = create new)
											&width, &height);

		// we don't need the data buffer anymore
		dataSize = dc.GetSize();
		dc.Release();
	}

	if (!mHandle)
	{
		ocError << "Cannot load NullTexture!";
		mWidth = 0;
		mHeight = 0;
		return 0;
	}
	
	mWidth = width;
	mHeight = height;

	return dataSize;
}

bool Texture::UnloadImpl()
{
	// free texture from low-level renderer
	gGfxRenderer.DeleteTexture(mHandle);
	mHandle = 0;
	
	return true;
}

uint32 Texture::GetWidth()
{
	EnsureLoaded();
	return mWidth;
}

uint32 Texture::GetHeight()
{
	EnsureLoaded();
	return mHeight;
}

TextureHandle Texture::GetTexture()
{
	EnsureLoaded();
	return mHandle;
}