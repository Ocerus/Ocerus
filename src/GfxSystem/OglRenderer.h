#ifndef _OGLRENDERER_H_
#define _OGLRENDERER_H_

#include "Base.h"
#include "Singleton.h"

#include "GfxRenderer.h"

#define gOglRenderer GfxSystem::OglRenderer::GetSingleton()

namespace GfxSystem
{
	class OglRenderer : public GfxRenderer, public Singleton<OglRenderer> 
	{
	public:
		/// Intialize OpenGL
		virtual void Init() const;
		
		/// Prepares OpenGL for drawing
		virtual bool BeginRendering() const;
		
		/// Finilize drawing whole screen (swap buffers etc ...)
		virtual void EndRendering() const;
		
		/// Finilize drawing current viewpoint (reset depth buffer ...)
		virtual void FinalizeViewport() const;

/**
	Loads an image from RAM into an OpenGL texture.
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
			int *width, int *height ) const;

		/// Deletes texture from renderers memory
		virtual void DeleteTexture(const uint32 &handle) const;
		
		/// Adds srite to queue for rendering
		virtual void SetTexture(const uint32 texture) const;
		
		/// Draws sprite
		virtual void DrawTexturedQuad(	const Vector2& position,
										const Vector2& size,
										const float32 z,
										const float32 transp) const;

		/// Draws simple quad
		virtual void DrawTestQuad() const;

		/// Draws simple textured quad
		virtual void DrawTestTexturedQuad(const uint32 text_handle) const;


		virtual void SetViewport(const GfxViewport& viewport) const;

		
		virtual void SetCamera(const Vector2& position, const float32 zoom, const float32 rotation) const;
	};
}

#endif