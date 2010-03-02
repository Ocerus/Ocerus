/// @file
/// Derived class from abstract GfxRenderer using OpenGL.

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
		virtual bool BeginRenderingImpl() const;
		
		/// Finilize drawing whole screen (swap buffers etc ...)
		virtual void EndRenderingImpl() const;
		
		/// Finilize drawing current viewpoint (reset depth buffer ...)
		virtual void FinalizeRenderTargetImpl() const;

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
			const ePixelFormat force_channels,
			const unsigned int reuse_texture_ID,
			int *width, int *height ) const;

		/// Deletes texture from renderers memory
		virtual void DeleteTexture(const uint32 &handle) const;
		
		/// Adds sprite to queue for rendering
		virtual void SetTexture(const uint32 texture) const;
		
		/// Draws sprite
		virtual void DrawSprite(const Sprite& spr) const;

		/// Draws line. Verts must be array of 2 Vector2s.
		virtual void DrawLine(	const Vector2* verts, const Color& color ) const;

		/// Draws polygon. Verts must be array of n Vector2s defining polygon.
		virtual void DrawPolygon(	const Vector2* verts, const int32 n, 
									const Color& color, const bool fill ) const;

		/// Draws circle
		virtual void DrawCircle(	const Vector2& position, const float32 radius,
									const Color& color, const bool fill) const;

		/// Draws rectangle. Position is center of rectangle. Rotation in degrees.
		virtual void DrawRect(		const Vector2& position, const Vector2& size, const float32 rotation,
									const Color& color, const bool fill) const;

		/// Set current viewport
		virtual void SetViewportImpl(const GfxViewport& viewport) const;

		/// Set current camera
		virtual void SetCameraImpl(const Vector2& position, const float32 zoom, const float32 rotation) const;

	};
}

#endif