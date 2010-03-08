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
		virtual bool KeyPressed(const KeyInfo& ke) = 0;

		/// Called when a keyboard key is released.
		/// If true is returned the even will not be processed further.
		virtual bool KeyReleased(const KeyInfo& ke) = 0;

		/// Called when the mouse moves. Cursor position and other info is passed via parameter.
		/// If true is returned the even will not be processed further.
		virtual bool MouseMoved(const MouseInfo& mi) = 0;

		/// Called when a mouse button is pressed. Cursor position and other info is passed via parameter.
		/// If true is returned the even will not be processed further.
		virtual bool MouseButtonPressed(const MouseInfo& mi, const eMouseButton btn) = 0;

		/// Called when a mouse button is released. Cursor position and other info is passed via parameter.
		/// If true is returned the even will not be processed further.
		virtual bool MouseButtonReleased(const MouseInfo& mi, const eMouseButton btn) = 0;

	};
}

#endif // IInputListener_h__