/// This class representes the checkbox GUI element. It is registered as a basic
/// reference and its handle can be cast to the Window and ButtonBase classes.
class Checkbox: public ButtonBase
{
public:

	/// Returns true if the checkbox is selected (has the ckeck mark).
	bool IsSelected() const;

	/// Sets whether the checkbox is selected or not.
	void SetSelected(bool selected);
};