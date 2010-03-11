/// @file
/// Area where some graphics can be rendered by GfxRenderer.

#ifndef GfxViewport_h__
#define GfxViewport_h__

#include "Base.h"

namespace GfxSystem
{
	/// Area where some graphics can be rendered by GfxRenderer.
	class GfxViewport
	{
	public:

		/// Constructs the viewport from a position and a size. Those vectors are from [0,1]x[0,1] and are relative to the window boundaries.
		/// If attribute "relativeScale" is true and application runs in windowed mode, it means that the scale of objects shown
		/// in this viewport is relative to window size. If the aplication runs in fullscreen then the scale is constant to resolution.
		GfxViewport(const Vector2& position, const Vector2& size, const bool relativeScale);

		/// Constructs the viewport which targets a texture. The rendering then results in the texture.
		GfxViewport(ResourceSystem::ResourcePtr texture);

		/// Returns the position of the viewport in the window.
		inline Vector2 GetPosition(void) const { return mPosition; }

		/// Returns the size of the viewport in the window.
		inline Vector2 GetSize(void) const { return mSize; }

		/// Returns the texture this viewport is attached to.
		/// Returns InvalidTextureHandle if there is no such.
		inline TextureHandle GetTexture(void) const { return mTexture; }

		/// Returns true if the viewport belongs to a texture.
		inline bool AssignedToTexture(void) const { return mTexture != InvalidTextureHandle; }

		/// Retrieves the boundaries of the viewport in the world space. This basically maps the screen space viewport to
		/// the world space.
		void CalculateWorldBoundaries( Vector2& topleft, Vector2& bottomright ) const;

		/// Retrieves the boundaries of the viewport in the screen space.
		void CalculateScreenBoundaries( Point& topleft, Point& bottomright ) const;

	private:

		Vector2 mPosition;
		Vector2 mSize;
		TextureHandle mTexture;
		bool mRelativeScale;
	};
}

#endif // GfxViewport_h__