/// This class representes the radio button GUI element. It is registered as a basic
/// reference and its handle can be cast to the Window and ButtonBase classes.
/// Radio buttons are grouped together using unique group IDs. Only single button in
/// the group can be selected at once.
class RadioButton: public ButtonBase
{
public:

	/// Returns true if the checkbox is selected (has the check mark).
	bool IsSelected() const;

	/// Select or deselect the button.
	void SetSelected(bool selected);

	/// Returns the ID of the group the button belongs to.
	uint32 GetGroupID() const;

	/// Sets the new group for this button.
	void SetGroupID(uint32 groupID);
};