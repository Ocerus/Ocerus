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
		
		/// Finilize drawing (swap buffers etc ...)
		virtual void EndRendering() const;

/**
	Loads an image from RAM into an OpenGL texture.
	\param buffer the image data in RAM just as if it were still in a file
	\param buffer_length the size of the buffer in bytes
	\param force_channels 0-image format, 1-luminous, 2-luminous/alpha, 3-RGB, 4-RGBA
	\param reuse_texture_ID 0-generate a new texture ID, otherwise reuse the texture ID (overwriting the old texture)
	\return 0-failed, otherwise returns the OpenGL texture handle
**/
		virtual uint32 LoadTexture(
			const unsigned char *const buffer,
			int buffer_length,
			int force_channels,
			unsigned int reuse_texture_ID ) const;

		/// Deletes texture from renderers memory
		virtual void DeleteTexture(const uint32 &handle) const;

		/// Draws simple quad
		virtual void DrawTestQuad() const;

		/// Draws simple textured quad
		virtual void DrawTestTexturedQuad(const uint32 text_handle) const;
	};
}

#endif