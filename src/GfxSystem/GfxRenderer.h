/// @file
/// Defines Renderer singleton.

#ifndef _GFXRENDERER_H_
#define _GFXRENDERER_H_

#include "Base.h"
#include "GfxStructures.h"

#define gGfxRenderer GfxSystem::GfxRenderer::GetSingleton()

namespace GfxSystem
{
	
	enum ePixelFormat
	{
		PF_AUTO = 0,			//auto
		PF_L = 1,				//grayscale
		PF_LA = 2,				//grayscale with alpha
		PF_RGB = 3,				//RGB
		PF_RGBA = 4				//RGB with alpha
	};

	class GfxRenderer : public Singleton<GfxRenderer> 
	{
	public:
		/// Intialize renderer
		virtual void Init() const = 0;

		/// Prepares renderer for drawing
		virtual bool BeginRendering() const = 0;
		
		/// Finilize drawing (swap buffers etc ...)
		virtual void EndRendering() const = 0;

		/// Finilize drawing current viewpoint (reset depth buffer ...)
		virtual void FinalizeViewport() const = 0;

		/**
			Loads an image from RAM into an Renderer texture.
			\param buffer the image data in RAM just as if it were still in a file
			\param buffer_length the size of the buffer in bytes
			\param force_channels 0-image format, 1-luminous, 2-luminous/alpha, 3-RGB, 4-RGBA
			\param reuse_texture_ID 0-generate a new texture ID, otherwise reuse the texture ID (overwriting the old texture)
			\param width, height returns size of texture
			\return 0-failed, otherwise returns the OpenGL texture handle
		**/
		virtual uint32 LoadTexture(
			const unsigned char *const buffer,
			const int buffer_length,
			const ePixelFormat force_channels,
			const unsigned int reuse_texture_ID,
			int *width, int *height  ) const = 0;

		/// Deletes texture from renderers memory
		virtual void DeleteTexture(const uint32 &handle) const = 0;

		/// Adds srite to queue for rendering
		void AddSprite(const Sprite spr);

		/// Draws sprites from queue
		void DrawSprites();

		/// Changes renderers texture
		virtual void SetTexture(const uint32 texture) const = 0;
		
		/// Draws quad with currently chosen texture
		virtual void DrawTexturedQuad(	const Vector2& position,
										const Vector2& size,
										const float32 z,
										const float32 transp ) const = 0;

		/// Draws line. Verts must be array of 2 Vector2s.
		virtual void DrawLine(	const Vector2* verts, const Color& color ) const = 0;

		/// Draws polygon. Verts must be array of n Vector2s defining polygon.
		virtual void DrawPolygon(	const Vector2* verts, const int32 n, 
									const Color& color, const bool fill ) const = 0;

		/// Draws circle
		virtual void DrawCircle(	const Vector2& position, const float32 radius,
									const Color& color, const bool fill) const = 0;
		
		/// Draws rectangle. Position is center of rectangle. Rotation in degrees.
		virtual void DrawRect(		const Vector2& position, const Vector2& size, const float32 rotation,
									const Color& color, const bool fill) const = 0;


		/// Resets sprites queue
		inline void ResetSprites() { mSprites.clear(); }

		/// Set current viewport
		virtual void SetViewport(const GfxViewport& viewport) const = 0;

		/// Set current camera
		virtual void SetCamera(const Vector2& position, const float32 zoom, const float32 rotation) const = 0;

	private:
		typedef vector<Sprite> SpriteVector;
		SpriteVector mSprites;

	protected:
		/// Half size of window in world coordination system (regardless of window actual pixel size in OS)
		static const int smOrthoSizeX = 512;
		static const int smOrthoSizeY = 384;
	};
}

#endif