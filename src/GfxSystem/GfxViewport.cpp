#include "Common.h"
#include "GfxViewport.h"
#include "Texture.h"


// Half the size of the window in the world coordination system (regardless of the actual pixel size).
// The value was determined from the 1024x768 resolution and serves as a constat to which we can related the zoom.
const int ORTHO_SIZE_X = 512;
const int ORTHO_SIZE_Y = 384;

GfxSystem::GfxViewport::GfxViewport( const Vector2& position, const Vector2& size, const bool relativeScale ):
	mPosition(position),
	mSize(size),
	mTexture(InvalidTextureHandle),
	mRelativeScale(relativeScale)
{

}

GfxSystem::GfxViewport::GfxViewport( ResourceSystem::ResourcePtr texture ):
	mPosition(Vector2_Zero),
	mSize(Vector2_Zero),
	mTexture(InvalidTextureHandle),
	mRelativeScale(false)
{
	OC_ASSERT(texture->GetType() == ResourceSystem::RESTYPE_TEXTURE);
	TexturePtr textureResource = (TexturePtr)texture; 
	mTexture = textureResource->GetTexture();
	mSize.x = (float32)textureResource->GetWidth();
	mSize.y = (float32)textureResource->GetHeight();
}

GfxSystem::GfxViewport::GfxViewport( TextureHandle texture, const uint32 width, const uint32 height ):
	mPosition(Vector2_Zero),
	mSize(Vector2_Zero),
	mTexture(texture),
	mRelativeScale(false)
{
	mSize.x = (float32)width;
	mSize.y = (float32)height;
}

void GfxSystem::GfxViewport::CalculateWorldBoundaries( Vector2& topleft, Vector2& bottomright ) const
{
	if (AttachedToTexture())
	{
		topleft.x = -ORTHO_SIZE_X;
		bottomright.x = ORTHO_SIZE_X;
		topleft.y = -ORTHO_SIZE_Y;
		bottomright.y = ORTHO_SIZE_Y;
	}
	else if (mRelativeScale)
	{
		// size of objects depends on resolution
		topleft.x = -ORTHO_SIZE_X * mSize.x;
		bottomright.x = ORTHO_SIZE_X * mSize.x;
		topleft.y = -ORTHO_SIZE_Y * mSize.y;
		bottomright.y = ORTHO_SIZE_Y * mSize.y;
	}
	else
	{
		// size of objects doesn't depend on resolution
		int32 resx = gGfxWindow.GetResolutionWidth();
		int32 resy = gGfxWindow.GetResolutionHeight();
		topleft.x = -resx * mSize.x / 2;
		bottomright.x = resx * mSize.x / 2;
		topleft.y = -resy * mSize.y / 2;
		bottomright.y = resy * mSize.y / 2;
	}
}

void GfxSystem::GfxViewport::CalculateScreenBoundaries( Point& topleft, Point& bottomright ) const
{
	if (AttachedToTexture())
	{
		topleft.x = 0;
		topleft.y = 0;
		bottomright.x = (int32)mSize.x;
		bottomright.y = (int32)mSize.y;
	}
	else
	{
		int32 resx = gGfxWindow.GetResolutionWidth();
		int32 resy = gGfxWindow.GetResolutionHeight();

		topleft.x = (int32)(mPosition.x * resx);
		topleft.y = (int32)(mPosition.y * resy);
		bottomright.x = topleft.x + (int32)(mSize.x * resx);
		bottomright.y = topleft.y + (int32)(mSize.y * resy);
	}
}