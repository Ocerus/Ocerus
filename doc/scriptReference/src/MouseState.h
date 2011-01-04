/// State of the mouse device at a specific point of time. This class is registered as a simple value type.
struct MouseState
{
	/// X coordinate in the screen.
	int32 x;

	/// Y coordinate in the screen.
	int32 y;

	/// Wheel position.
	int32 wheel;

	/// Pressed buttons.
	UINT8 buttons;
};