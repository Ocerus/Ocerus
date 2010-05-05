/// @file
/// Notifications of events from input devices.

#ifndef IInputListener_h__
#define IInputListener_h__

#include "InputActions.h"

namespace InputSystem
{
	/// Interface for receiving notifications of events from input devices.
	class IInputListener
	{
	public:

		IInputListener(void) {}
		virtual ~IInputListener(void) {}

		/// Called when a keyboard key is pressed.
		/// If true is returned the even will not be processed further.
		virtual bool KeyPressed(const InputSystem::KeyInfo& ke) { OC_UNUSED(ke); return false; }

		/// Called when a keyboard key is released.
		/// If true is returned the even will not be processed further.
		virtual bool KeyReleased(const InputSystem::KeyInfo& ke) { OC_UNUSED(ke); return false; }

		/// Called when the mouse moves. Cursor position and other info is passed via parameter.
		/// If true is returned the even will not be processed further.
		virtual bool MouseMoved(const InputSystem::MouseInfo& mi) { OC_UNUSED(mi); return false; }

		/// Called when a mouse button is pressed. Cursor position and other info is passed via parameter.
		/// If true is returned the even will not be processed further.
		virtual bool MouseButtonPressed(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn) { OC_UNUSED(mi); OC_UNUSED(btn); return false; }

		/// Called when a mouse button is released. Cursor position and other info is passed via parameter.
		/// If true is returned the even will not be processed further.
		virtual bool MouseButtonReleased(const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn) { OC_UNUSED(mi); OC_UNUSED(btn); return false; }

	};
}

#endif // IInputListener_h__