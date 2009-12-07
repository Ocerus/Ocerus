#ifndef _GFXRENDERER_H_
#define _GFXRENDERER_H_

#include "Base.h"
#include "GfxStructures.h"

#define gGfxRenderer GfxSystem::GfxRenderer::GetSingleton()

namespace GfxSystem
{
	class GfxRenderer : public Singleton<GfxRenderer> 
	{
	public:
		/// Intialize renderer
		virtual void Init() const = 0;

		/// Prepares renderer for drawing
		virtual bool BeginRendering() const = 0;
		
		/// Finilize drawing (swap buffers etc ...)
		virtual void EndRendering() const = 0;

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
			const int force_channels,
			const unsigned int reuse_texture_ID,
			int *width, int *height  ) const = 0;

		/// Deletes texture from renderers memory
		virtual void DeleteTexture(const uint32 &handle) const = 0;

		/// Adds srite to queue for rendering
		void AddSprite(const Sprite spr);

		/// Draws sprites from queue
		void DrawSprites() const;

		/// Changes renderers texture
		virtual void SetTexture(const uint32 texture) const = 0;
		
		/// Draws quad with currently chosen texture
		virtual void DrawTexturedQuad(const Vector2& position, const Vector2& size, const float32 z) const = 0;

		/// Resets rendering queue
		inline void Reset() {mSprites.clear();}

		/// Draws simple quad
		virtual void DrawTestQuad() const = 0;

		/// Draws simple textured quad
		virtual void DrawTestTexturedQuad(const uint32 text_handle) const = 0;

	private:
		typedef vector<Sprite> SpriteVector;
		SpriteVector mSprites;
	};
}

#endif