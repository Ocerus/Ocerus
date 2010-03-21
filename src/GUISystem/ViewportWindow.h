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

	private:
		GfxSystem::RenderTargetID mRenderTarget;
	};
}


#endif // _VIEWPORT_WINDOW_H_
