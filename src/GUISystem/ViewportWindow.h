/// @file
/// CEGUI representation of viewport window.

#ifndef _GUISYSTEM_VIEWPORTWINDOW_H_
#define _GUISYSTEM_VIEWPORTWINDOW_H_

#include "Base.h"
#include <elements/CEGUIFrameWindow.h>
#include <CEGUIInputEvent.h>

namespace GUISystem
{
	/// The ViewportWindow class represents a viewport window with a frame.
	class ViewportWindow: public CEGUI::FrameWindow
	{
	public:
		/// Constructs a ViewportWindow object. Do not use this constructor directly,
		/// use CEGUI::WindowManager instead.
		ViewportWindow(const CEGUI::String& type, const CEGUI::String& name);

		/// Destroys the ViewportWindow object.
		virtual ~ViewportWindow();

		/// Disables the viewport. You can enable it again by setting a camera.
		void Disable();

		/// Sets the camera of the viewport to newCamera.
		void SetCamera(const EntitySystem::EntityHandle& newCamera);

		/// Returns the current camera of the viewport.
		EntitySystem::EntityHandle GetCamera() const;

		/// Returns the render target associated with the window.
		inline GfxSystem::RenderTargetID GetRenderTarget() const { return mRenderTarget; }

		/// Sets whether the content can be panned, zoomed, etc.
		/// @see GfxSystem::DragDropCameraMover
		void SetDragAndDropCamera(bool enable);
		
		/// Returns whether the content can be panned, zoomed, etc.
		/// @see GfxSystem::DragDropCameraMover
		inline bool GetMovableContent() const { return mIsMovableContent; }

		/// Registeres an input listener for this window.
		void AddInputListener(InputSystem::IInputListener* listener);

		/// Unregisters an input listener for this window.
		void RemoveInputListener(InputSystem::IInputListener* listener);

		/// Makes the window active for receiving input.
		void Activate();

		/// @name Overridden members from CEGUI::FrameWindow
		//@{
		virtual void InitialiseComponents();
		static const CEGUI::String WidgetTypeName;
		//@}

	protected:
		///@name CEGUI events.
		//@{
			virtual void onSized(CEGUI::WindowEventArgs& e);
			virtual void onMoved(CEGUI::WindowEventArgs& e);
			virtual void onMouseMove(CEGUI::MouseEventArgs& e);
			virtual void onMouseButtonDown(CEGUI::MouseEventArgs& e);
			virtual void onMouseButtonUp(CEGUI::MouseEventArgs& e);
			virtual void onMouseWheel(CEGUI::MouseEventArgs& e);
			virtual void onKeyDown(CEGUI::KeyEventArgs& e);
			virtual void onKeyUp(CEGUI::KeyEventArgs& e);
		//@}

		/// Updates viewport area.
		void UpdateViewportArea();

		/// Sets the size and position parameters with the area of the window.
		void GetArea(Vector2& position, Vector2& size) const;
		
		/// Creates a camera mover on current render target.
		void CreateCameraMover();
		
		/// Destroys the camera mover.
		void DeleteCameraMover();

	private:
		bool mIsEnabled;
		GfxSystem::RenderTargetID mRenderTarget;
		bool mIsMovableContent;
		GfxSystem::DragDropCameraMover* mCameraMover;
		typedef vector<InputSystem::IInputListener*> ListenersList;
		ListenersList mListeners;
	};
}

#endif // _GUISYSTEM_VIEWPORTWINDOW_H_