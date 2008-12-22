#ifndef IInputListener_h__
#define IInputListener_h__

#include "InputActions.h"

namespace InputSystem
{
	/** This class provides callbacks from the input manager.
	*/
	class IInputListener
	{
	public:
		/// @name These do nothing.
		//@{
		IInputListener(void) {}
		virtual ~IInputListener(void) {}
		//@}

		/// @name Called when a keyboard key is pressed/released.
		//@{
		virtual void KeyPressed(const KeyInfo& ke) = 0;
		virtual void KeyReleased(const KeyInfo& ke) = 0;
		//@}

		/// @name Called when the mouse moves. Cursor position and other info is passed via parameter.
		virtual void MouseMoved(const MouseInfo& mi) = 0;
		/// @name Called when a mouse button is pressed.
		virtual void MouseButtonPressed(const MouseInfo& mi, const eMouseButton btn) = 0;
		/// @name Called when a mouse button is released.
		virtual void MouseButtonReleased(const MouseInfo& mi, const eMouseButton btn) = 0;

	};
}

#endif // IInputListener_h__