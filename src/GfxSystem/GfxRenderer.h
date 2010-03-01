/// @file
/// Defines Renderer singleton.

#ifndef _GFXRENDERER_H_
#define _GFXRENDERER_H_

#include "Base.h"
#include "GfxStructures.h"

#define gGfxRenderer GfxSystem::GfxRenderer::GetSingleton()

namespace GfxSystem
{
	
	///@TODO popsat detailneji.
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

		/// Unique identifier of a render target.
		typedef int32 RenderTargetID;

		/// Invalid render target ID.
		static const RenderTargetID InvalidRenderTargetID = -1;

	public:

		/// Default constructor.
		GfxRenderer();

		/// Default destructor.
		virtual ~GfxRenderer();

		/// Intialize the renderer.
		inline void Init() const { InitImpl(); }

		/// Prepares renderer for drawing.
		bool BeginRendering();
		
		/// Finilize drawing (swap buffers etc ...).
		void EndRendering();

		/// Finilize drawing current viewpoint (reset depth buffer ...).
		inline void FinalizeRenderTarget() const { FinalizeRenderTargetImpl(); }

		/// Adds a new render target to the list. Returns the ID of the added target. Mustn't be called while rendering.
		/// Returns InvalidRenderTargetID if something was wrong.
		RenderTargetID AddRenderTarget(const GfxViewport& viewport, const EntitySystem::EntityHandle camera);

		/// Removew the specified render target from the list. Returns false if there was no such.
		bool RemoveRenderTarget(const RenderTargetID toRemove);

		/// Sets the current render target. Returns false if no such exists.
		bool SetCurrentRenderTarget(const RenderTargetID toSet);

		/**
			Loads an image from RAM into an Renderer texture.
			\param buffer the image data in RAM just as if it were still in a file
			\param buffer_length the size of the buffer in bytes
			\param force_channels 0-image format, 1-luminous, 2-luminous/alpha, 3-RGB, 4-RGBA
			\param reuse_texture_ID 0-generate a new texture ID, otherwise reuse the texture ID (overwriting the old texture)
			\param width, height returns size of texture
			\return 0-failed, otherwise returns the OpenGL texture handle
		**/
		virtual uint32 LoadTexture(const unsigned char *const buffer, const int buffer_length, const ePixelFormat force_channels, const unsigned int reuse_texture_ID, int *width, int *height) const = 0;

		/// Deletes texture from renderers memory.
		virtual void DeleteTexture(const uint32 &handle) const = 0;

		/// Adds srite to queue for rendering.
		void AddSprite(const Sprite spr);

		/// Draws sprites from queue.
		void DrawSprites();

		/// Changes renderer's texture.
		virtual void SetTexture(const uint32 texture) const = 0;
		
		/// Draws a quad with currently the chosen texture.
		virtual void DrawSprite(const Sprite& spr) const = 0;

		/// Draws a line. Verts must be an array of 2 Vector2s.
		virtual void DrawLine(const Vector2* verts, const Color& color) const = 0;

		/// Draws a polygon. Verts must be an array of n Vector2s defining polygon.
		virtual void DrawPolygon(const Vector2* verts, const int32 n, const Color& color, const bool fill) const = 0;

		/// Draws a circle.
		virtual void DrawCircle(const Vector2& position, const float32 radius, const Color& color, const bool fill) const = 0;

		/// Draws a rectangle. Position is center of rectangle. Rotation in degrees.
		virtual void DrawRect(const Vector2& position, const Vector2& size, const float32 rotation, const Color& color, const bool fill) const = 0;

	protected:

		/// Called after the renderer is created.
		virtual void InitImpl() const = 0;

		/// Called when the rendering is started.
		virtual bool BeginRenderingImpl() const = 0;

		/// Called when the rendering is finished.
		virtual void EndRenderingImpl() const = 0;

		/// Finilize drawing current viewpoint (reset depth buffer ...).
		virtual void FinalizeRenderTargetImpl() const = 0;

		/// Called when the current viewport is changed.
		virtual void SetCurrentViewportImpl(const GfxViewport& viewport) const = 0;

		/// Called when the current camera is changed.
		virtual void SetCurrentCameraImpl(const Vector2& position, const float32 zoom, const float32 rotation) const = 0;

		/// Resets the sprites queue.
		inline void ResetSprites() { mSprites.clear(); }

	private:

		/// Vector with sprites to be rendered.
		typedef vector<Sprite> SpriteVector;
		SpriteVector mSprites;

		/// Render targets.
		typedef pair<GfxViewport, EntitySystem::EntityHandle> ViewportWithCamera;
		typedef vector<ViewportWithCamera*> RenderTargetsVector;
		RenderTargetsVector mRenderTargets;

	protected:

		/// Half size of window in world coordination system (regardless of window actual pixel size in OS)
		static const int smOrthoSizeX = 512;
		static const int smOrthoSizeY = 384;

		/// True if the rendering began but still didn't finish.
		bool mIsRendering;

		/// Disabled.
		GfxRenderer(const GfxRenderer&);
		GfxRenderer& operator=(const GfxRenderer&);
	};
}

#endif