/// Management of game input. It is registered as a no-handle reference. So, the only
/// way you can access it is to use the ::gInputMgr global variable.
class InputMgr
{
public:

	/// Updates the state of the manager and processes all events. This is called automatically by the engine.
	void CaptureInput();

	/// Returns true if the given key is pressed.
	bool IsKeyDown(eKeyCode key) const;

	/// Returns true if the given button of the mouse is pressed.
	bool IsMouseButtonPressed(eMouseButton mouseButton) const;

	/// Returns the current state of the mouse.
	MouseState& GetMouseState() const;
};