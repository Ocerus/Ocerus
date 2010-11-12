/// @file
/// Key and button codes, data structures for mouse state informations.

#ifndef InputActions_h__
#define InputActions_h__

#include "Base.h"

namespace InputSystem
{
	/// Key code constants.
	/// The codes are copied from OIS, so you can cast the type directly from and to OIS.
	enum eKeyCode
	{
		#define KEY_CODE(x, y) x = y,
		#include "KeyCodes.h"
		#undef KEY_CODE

		NUM_KEY_CODE = 0xFF
	};

	/// Returns the given key code as a string.
	const char* GetKeyCodeString(uint8 code);

	/// Data of an keyboard input event.
	struct KeyInfo
	{
		/// Enum value of the key.
		eKeyCode keyCode;
		/// Character of the key pressed.
		uint32 charCode;
	};

	/// State of the mouse device at a specific point of time.
	struct MouseState
	{
		MouseState(void): x(0), y(0), wheel(0), buttons(0) {}

		/// X position of the cursor.
		int32 x;
		/// Y position of the cursor.
		int32 y;

		/// Wheel position.
		int32 wheel;

		/// Pressed buttons.
		uint8 buttons;
	};

	/// Data of a mouse input event.
	struct MouseInfo
	{
		/// X position of the cursor.
		int32 x;
		/// Y position of the cursor.
		int32 y;

		/// Delta position since last update.
		int32 dx;
		/// Delta position since last update.
		int32 dy;

		/// Wheel position.
		int32 wheel;

		/// Delta position of the wheel since last update.
		int32 wheelDelta;

		/// Default constructor.
		MouseInfo(void) {}

		/// Conversion constructor.
		MouseInfo(const MouseState& ms): 
		x(ms.x),
			y(ms.y),
			dx(0),
			dy(0),
			wheel(ms.wheel),
			wheelDelta(0)
		{

		}
	};

	/// All possible buttons of the mouse device.
	enum eMouseButton 
	{
		/// Left mouse button.
		MBTN_LEFT=1<<1, 
		/// Right mouse button.
		MBTN_RIGHT=1<<2, 
		/// Middle mouse button or the wheel.
		MBTN_MIDDLE=1<<3, 
		/// Unknown button - error.
		MBTN_UNKNOWN=0 
	};

}

#endif // InputActions_h__