/// @file
/// Viewport window.

#ifndef _VIEWPORT_WINDOW_H_
#define _VIEWPORT_WINDOW_H_

#include "Common.h"
#include <elements/CEGUIFrameWindow.h>

namespace GUISystem
{
	/// The ViewportWindow class represent a viewport window with a frame.
	/// @todo Add MousePicker.
	class ViewportWindow: public CEGUI::FrameWindow
	{
	public:
		/// Constructs a ViewportWindow object. Do not use this constructor directly,
		/// use CEGUI::WindowManager instead.
		ViewportWindow(const CEGUI::String& type, const CEGUI::String& name);

		/// Destroys the ViewportWindow object.
		virtual ~ViewportWindow();

		/// Sets the camera of the viewport to newCamera.
		void SetCamera(const EntitySystem::EntityHandle& newCamera);

		/// Returns the current camera of the viewport.
		EntitySystem::EntityHandle GetCamera() const;

		/// Returns the render target associated with the window.
		inline GfxSystem::RenderTargetID GetRenderTarget() const { return mRenderTarget; }

		/// Sets whether the content can be panned, zoomed, etc.
		/// @see GfxSystem::DragDropCameraMover
		void SetMovableContent(bool isMovableContent);
		
		/// Returns whether the content can be panned, zoomed, etc.
		/// @see GfxSystem::DragDropCameraMover
		inline bool GetMovableContent() const { return mIsMovableContent; }

		/// @name Overridden members from CEGUI::FrameWindow
		//@{
		virtual void initialiseComponents();
		static const CEGUI::String WidgetTypeName;
		//@}

	protected:
		virtual void onSized(CEGUI::WindowEventArgs& e);
		virtual void onMoved(CEGUI::WindowEventArgs& e);

		/// Updates viewport area.
		void UpdateViewportArea();

		/// Sets the size and position parameters with the area of the window.
		void GetArea(Vector2& position, Vector2& size) const;
		
		/// Creates a camera mover on current render target.
		void CreateCameraMover();
		
		/// Destroys the camera mover.
		void DeleteCameraMover();

	private:
		GfxSystem::RenderTargetID mRenderTarget;
		bool mIsMovableContent;
		GfxSystem::DragDropCameraMover* mCameraMover;
	};
}


#endif // _VIEWPORT_WINDOW_H_
