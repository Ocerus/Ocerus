/// @file
/// Specialized OpenGL renderer.

#ifndef _OGLRENDERER_H_
#define _OGLRENDERER_H_

#include "Base.h"
#include "Singleton.h"
#include "GfxRenderer.h"

namespace GfxSystem
{
	/// Specialized OpenGL renderer.
	class OglRenderer : public GfxRenderer
	{
	public:

		virtual void Init();

		virtual bool BeginRenderingImpl() const;

		virtual void EndRenderingImpl() const;

		virtual void SetViewportImpl(const GfxViewport* viewport);

		virtual void SetCameraImpl(const Vector2& position, const float32 zoom, const float32 rotation) const;

		virtual void FinalizeRenderTargetImpl() const;

		virtual TextureHandle LoadTexture(const uint8* const buffer, const int32 buffer_length, const ePixelFormat force_channels, 
			const uint32 reuse_texture_ID, int32* width, int32* height) const;

		virtual TextureHandle CreateRenderTexture(const uint32 width, const uint32 height) const;

		virtual void DeleteTexture(const TextureHandle& handle) const;

		virtual void DrawTexturedQuad(const TexturedQuad& quad) const;

		virtual void DrawLine(const Vector2* verts, const Color& color) const;

		virtual void DrawPolygon(const Vector2* verts, const int32 n, const Color& color, const bool fill ) const;

		virtual void DrawCircle(const Vector2& position, const float32 radius, const Color& color, const bool fill) const;

		virtual void DrawRect(const Vector2& topleft, const Vector2& bottomright, const float32 rotation, const Color& color, const bool fill) const;

		virtual void ClearScreen(const Color& color) const;

		virtual void ClearViewport(const GfxViewport& viewport, const Color& color) const;


	private:

		typedef unsigned int OpenGLHandle;

		const GfxViewport* mCurrentViewport;
		OpenGLHandle mFrameBuffer;
	};
}

#endif