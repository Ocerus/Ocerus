/// Base class for all button GUI elements. It is registered as
/// a basic reference and its handle can be cast to all of its descendants and Window.
class ButtonBase: public Window
{
public:

	/// Returns true if the user is hovering over this widget.
	bool IsHovering() const;

	/// Returns true if the button widget is in the pushed state.
	bool IsPushed() const;
};