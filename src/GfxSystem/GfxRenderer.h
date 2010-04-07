/// @file
/// Entry point to the rendering subsystem.

#ifndef _GFXRENDERER_H_
#define _GFXRENDERER_H_

#include "Base.h"
#include "GfxStructures.h"
#include "GfxViewport.h"
#include "RenderTarget.h"

/// Macro for easier use.
#define gGfxRenderer GfxSystem::GfxRenderer::GetSingleton()

namespace GfxSystem
{
	/// Rendering of everything except GUI. The rendering is done via rendering targets which define a viewport in the output
	/// device and a camera to use while rendering.
	class GfxRenderer : public Singleton<GfxRenderer> 
	{
	public:

		/// Default constructor.
		GfxRenderer();

		/// Default destructor.
		virtual ~GfxRenderer();

		/// Intializes the renderer.
		virtual void Init() = 0;

		/// Prepares renderer for drawing.
		bool BeginRendering();
		
		/// Finilizes drawing of everything this frame (swap buffers etc ...).
		void EndRendering();

		/// Finalizes drawing of the current viewpoint
		inline void FinalizeRenderTarget() const;

		/// Adds a new render target to the list. Returns the ID of the added target. Mustn't be called while rendering.
		/// Returns InvalidRenderTargetID if something was wrong.
		RenderTargetID AddRenderTarget(const GfxViewport& viewport, const EntitySystem::EntityHandle camera);

		/// Removes the specified render target from the list. Returns false if there was no such.
		bool RemoveRenderTarget(const RenderTargetID toRemove);

		/// Sets the current render target. Returns false if no such exists.
		bool SetCurrentRenderTarget(const RenderTargetID toSet);
		
		/// Returns true if the render target is valid.
		bool IsRenderTargetValid(const RenderTargetID toCheck);
		
		/// Returns the current scene manager.
		inline GfxSceneMgr* GetSceneManager() const { return mSceneMgr; }


	public:

		/// Loads an image from RAM into the platform specific texture.
		///	\param buffer the image data in RAM just as if it were still in a file
		///	\param buffer_length the size of the buffer in bytes
		///	\param force_channels 0-image format, 1-luminous, 2-luminous/alpha, 3-RGB, 4-RGBA
		///	\param reuse_texture_ID 0-generate a new texture ID, otherwise reuse the texture ID (overwriting the old texture)
		///	\param width, height returns size of texture
		///	\return 0-failed, otherwise returns the OpenGL texture handle
		virtual TextureHandle LoadTexture(const uint8* const buffer, const int32 buffer_length, const ePixelFormat force_channels, 
			const uint32 reuse_texture_ID, int32* width, int32* height) const = 0;

		/// Creates a texture into which it can be rendered.
		virtual TextureHandle CreateRenderTexture(const uint32 width, const uint32 height) const = 0;

		/// Deletes the texture from the memory.
		virtual void DeleteTexture(const TextureHandle& handle) const = 0;

		/// Adds a textured quad to the queue for rendering.
		void QueueTexturedQuad(const TexturedQuad spr);

		/// Draws all quads from the queue.
		void ProcessTexturedQuads();

		/// Draws all visible entities.
		void DrawEntities();

		/// Draws a quad with its texture.
		virtual void DrawTexturedQuad(const TexturedQuad& quad) const = 0;

		/// Draws a 3d model.
		virtual void DrawTexturedMesh(const TexturedMesh& mesh) const = 0;

		/// Draws a sprite component.
		void DrawSprite(const EntitySystem::Component* sprite, const EntitySystem::Component* transform) const;

		/// Draws a single entity.
		void DrawEntity(const EntitySystem::EntityHandle entity) const;

		/// Draws a line. Verts must be an array of 2 Vector2s.
		virtual void DrawLine(const Vector2* verts, const Color& color, const float32 width = 1.0f) const = 0;

		/// Draws a polygon. Verts must be an array of n Vector2s defining the polygon.
		virtual void DrawPolygon(const Vector2* verts, const int32 n, const Color& color, const bool fill) const = 0;

		/// Draws a circle.
		virtual void DrawCircle(const Vector2& position, const float32 radius, const Color& color, const bool fill) const = 0;

		/// Draws a rectangle. Rotation is given in radians.
		virtual void DrawRect(const Vector2& topleft, const Vector2& bottomright, const float32 rotation, const Color& color, const bool fill) const = 0;

		/// Clears the screen with the given color.
		virtual void ClearScreen(const Color& color) const = 0;

		/// Clears the designated render target with the given color.
		void ClearRenderTarget(const RenderTargetID target, const Color& color) const;

		/// Clears the current render target with the given color.
		void ClearCurrentRenderTarget(const Color& color) const;

		/// Clears the given viewport with the given color.
		virtual void ClearViewport(const GfxViewport& viewport, const Color& color) const = 0;


	public:

		/// Converts coordinates from the screen space to the world space.
		/// Returns false if the conversion failed (for example, the screen position was not in any of the viewports).
		/// The result is returned in the second parameter.
		/// As a last parameter the desired render target can be specified.
		bool ConvertScreenToWorldCoords(const Point& screenCoords, Vector2& worldCoords, const RenderTargetID renderTarget = InvalidRenderTargetID) const;

		/// Returns a pointer to the viewport associated with a render target, or NULL if render target does not exist.
		GfxViewport* GetRenderTargetViewport(const RenderTargetID renderTarget) const;

		/// Returns the camera associated with a render target.
		EntitySystem::EntityHandle GetRenderTargetCamera(const RenderTargetID renderTarget) const;

		/// Returns the camera zoom of a render target.
		float32 GetRenderTargetCameraZoom(const RenderTargetID renderTarget) const;

		/// Sets position of camera associated with render target. Returns false if render target does not exist.
		bool SetRenderTargetCameraZoom(const RenderTargetID renderTarget, float32 newZoom) const;

		/// Returns the camera rotation of a render target.
		float32 GetRenderTargetCameraRotation(const RenderTargetID renderTarget) const;

		/// Returns the camera position of a render target.
		Vector2 GetRenderTargetCameraPosition(const RenderTargetID renderTarget) const;

		/// Sets position of camera associated with render target. Returns false if render target does not exist.
		bool SetRenderTargetCameraPosition(const RenderTargetID renderTarget, Vector2 newPosition) const;

		/// Retrieves render targets camera view boundaries in world space.
		void CalculateRenderTargetWorldBoundaries( const RenderTargetID renderTarget, Vector2& topleft, Vector2& bottomright ) const;

	protected:

		/// Called when the rendering is started.
		virtual bool BeginRenderingImpl() const = 0;

		/// Called when the rendering is finished.
		virtual void EndRenderingImpl() const = 0;

		/// Finish rendering of the current viewpoint.
		virtual void FinalizeRenderTargetImpl() const = 0;

		/// Called when the current viewport is changed.
		virtual void SetViewportImpl(const GfxViewport* viewport) = 0;

		/// Called when the current camera is changed.
		virtual void SetCameraImpl(const Vector2& position, const float32 zoom, const float32 rotation) const = 0;

	private:

		// Vector with sprites to be rendered.
		typedef vector<TexturedQuad> QuadVector;
		QuadVector mQuads;

		// Render targets.
		typedef vector<RenderTarget*> RenderTargetsVector;
		RenderTargetsVector mRenderTargets;
		RenderTargetID mCurrentRenderTargetID;

	private:

		/// Converts coordinates from the screen space to the world space.
		/// Returns false if the conversion failed (for example, the screen position was not in any of the viewports).
		/// The result is returned in the second parameter.
		/// As a last parameter the desired render target must be specified.
		bool ConvertScreenToWorldCoords( const Point& screenCoords, Vector2& worldCoords, const RenderTarget& renderTarget ) const;

		/// Does inverse camera transformation on the given vector and returns result.
		Vector2 GetInverseCameraTranform( const EntitySystem::EntityHandle& camera, const Vector2& vec ) const;

		/// Draws grid (defined in viewport) ontop of everything.
		void DrawGrid( const RenderTargetID renderTargetID ) const;

	public:

		/// Constant which binds pixel and world units together. And image of this size will be 1.0 units big in the world.
		static const float32 PIXELS_PER_WORLD_UNIT;

	protected:

		/// True if the rendering began but still didn't finish.
		bool mIsRendering;
		GfxSceneMgr* mSceneMgr;

		/// Disabled.
		GfxRenderer(const GfxRenderer&);
		GfxRenderer& operator=(const GfxRenderer&);
	};
}

//-----------------------------------------------------------------------------
// Implementation



void GfxSystem::GfxRenderer::FinalizeRenderTarget() const
{
	DrawGrid( mCurrentRenderTargetID );
	FinalizeRenderTargetImpl();
}

#endif