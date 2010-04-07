/// @file
/// Area where some graphics can be rendered by GfxRenderer.

#ifndef GfxViewport_h__
#define GfxViewport_h__

#include "Base.h"
#include "GfxStructures.h"

namespace GfxSystem
{
	/// Area where some graphics can be rendered by GfxRenderer.
	class GfxViewport
	{
	public:

		/// Constructs the viewport from a position and a size. Those vectors are from [0,1]x[0,1] and are relative to the window boundaries.
		/// If attribute "relativeScale" is true and application runs in windowed mode, it means that the scale of objects shown
		/// in this viewport is relative to window size. If the aplication runs in fullscreen then the scale is constant to resolution.
		GfxViewport(const Vector2& position, const Vector2& size, const bool relativeScale, const bool gridEnabled);

		/// Constructs the viewport which targets a texture. The rendering then results in the texture.
		GfxViewport(ResourceSystem::ResourcePtr texture);

		/// Constructs the viewport which targets a texture. The rendering then results in the texture.
		GfxViewport(TextureHandle texture, const uint32 width, const uint32 height);

		/// Returns the position of the viewport in the window.
		inline Vector2 GetPosition(void) const { return mPosition; }

		/// Returns the size of the viewport in the window.
		inline Vector2 GetSize(void) const { return mSize; }

		/// Sets the position of the viewport in the window.
		inline void SetPosition(const Vector2& newPosition) { mPosition = newPosition; }

		/// Sets the size of the viewport in the window.
		inline void SetSize(const Vector2& newSize) { mSize = newSize; }

		/// Returns grid attributes.
		inline GridInfo GetGridInfo(void) const { return mGridInfo; }
		
		/// Sets grid attributes.
		inline void SetGridInfo(const GridInfo& newGridInfo) { mGridInfo = newGridInfo; }
		
		/// Returns true if drawing grid is enabled.
		inline bool GetGridEnabled(void) const { return mGridEnabled; }
		
		/// Enables or disables drawing grid.
		inline void SetGridEnabled(const bool newGridEnabled) { mGridEnabled = newGridEnabled; }

		/// Returns the texture this viewport is attached to.
		/// Returns InvalidTextureHandle if there is no such.
		inline TextureHandle GetRenderTexture(void) const { return mTexture; }

		/// Returns true if the viewport belongs to a texture.
		inline bool AttachedToTexture(void) const { return mTexture != InvalidTextureHandle; }

		/// Retrieves the boundaries of the viewport in the world space. This basically maps the screen space viewport to
		/// the world space. It is without camera transormation.
		void CalculateWorldBoundaries( Vector2& topleft, Vector2& bottomright ) const;

		/// Retrieves the boundaries of the viewport in the screen space.
		void CalculateScreenBoundaries( Point& topleft, Point& bottomright ) const;

	private:

		Vector2 mPosition;
		Vector2 mSize;
		TextureHandle mTexture;
		bool mRelativeScale;

		bool mGridEnabled;
		GridInfo mGridInfo;
	};
}

#endif // GfxViewport_h__