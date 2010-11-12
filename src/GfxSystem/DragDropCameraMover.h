//// @file
//// Drag'n'Drop camera movement.

#ifndef DragDropCameraMover_h__
#define DragDropCameraMover_h__

#include "Base.h"

namespace GfxSystem 
{
	/// Provides Drag'n'Drop camera movement with given render target.
	/// It receives input through IInputListener as other parts of the system (editor, game).
	class DragDropCameraMover : public InputSystem::IInputListener
	{
	public:

		/// Constructs the Drag'n'Drop camera mover.
		DragDropCameraMover();

		/// Constructs the Drag'n'Drop camera mover.
		DragDropCameraMover( const RenderTargetID renderTarget );

		/// Constructs the Drag'n'Drop camera mover.
		DragDropCameraMover( const RenderTargetID renderTarget,	const float32 zoomSensitivity, const float32 minZoom, const float32 maxZoom	);

		/// Returns the associated render target.
		inline RenderTargetID GetRenderTarget() const { return mRenderTarget; }

		/// Sets a new render target.
		inline void SetRenderTarget(const RenderTargetID newRenderTarget) { mRenderTarget = newRenderTarget; }

		/// Returns the zoom sensitivity.
		inline float32 GetZoomSensitivity() const { return mZoomSensitivity / cmZoomModifier; }

		/// Sets a new zoom sensitivity.
		inline void SetZoomSensitivity(const float32 zoomSensitivity) { mZoomSensitivity = zoomSensitivity * cmZoomModifier; }
		
		/// Returns the minimal zoom.
		inline float32 GetMinZoom() const { return mMinZoom; }

		/// Sets a new minimal zoom.
		inline void SetMinZoom(const float32 minZoom) { mMinZoom = minZoom; }
		
		/// Returns the maximal zoom.
		inline float32 GetMaxZoom() const { return mMaxZoom; }

		/// Sets a new maximal zoom.
		inline void SetMaxZoom(const float32 maxZoom) { mMaxZoom = maxZoom; }
		
		/// Called by the InputSystem when a key is pressed. Not used here.
		virtual bool KeyPressed(const InputSystem::KeyInfo& ke);

		/// Called by the InputSystem when a key is released. Not used here.
		virtual bool KeyReleased(const InputSystem::KeyInfo& ke);

		/// Called when the mouse moves or wheel changes. Used to move or zoom camera.
		virtual bool MouseMoved(const InputSystem::MouseInfo& mi);

		/// Called when a mouse button is pressed. Notices if middle button was pressed.
		virtual bool MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);

		/// Called when a mouse button is released. Notices if middle button was released.
		virtual bool MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn);

	private:

		// Since the InputMgr gives wierd numbers for mouse wheel movement, this constant
		// changes mZoomSensitivity so that it can be set with nice and comprehensible numbers.
		// Actual value is stored in macro ZOOMMOD.
		// Sigle wheel change = 120 so 1.0f/1200.0f means that zoom will change by 10%.
		// If you call method SetZoomSensitivity(1), it will set mZoomSensitivity to this constant.
		const float32 cmZoomModifier;

		RenderTargetID mRenderTarget;

		Vector2 mLastWorldCursorPos;
		bool mLastPosValid;

		float32 mZoomSensitivity;

		float32 mMinZoom;
		float32 mMaxZoom;
	};
}

#endif // DragDropCameraMover_h__