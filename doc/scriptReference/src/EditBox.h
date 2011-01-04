/// This class representes the editbox GUI element. It is registered as a basic
/// reference and its handle can be cast to the Window class.
class Editbox: public Window
{
public:

	/// Returns true if the editbox has input focus.
	bool HasInputFocus() const;

	/// Returns true if the editbox is read-only.
	bool IsReadOnly() const;

	/// Returns true if the text for the editbox will be rendered masked.
	bool IsTextMasked() const;

	/// Returns true if the editbox text is valid given the currently set validation string.
	bool IsTextValid() const;

	/// Returns the currently set validation string.
	const CEGUIString& GetValidationString() const;

	/// Returns the UFT32 code point used when rendering the masked text.
	uint32 GetMaskCodePoint() const;

	/// Returns the maximum text length set for this editbox.
	uint32 GetMaxTextLength() const;

	/// Specifies whether the editbox is read-only.
	void SetReadOnly(bool readonly);

	/// Specifies whether the text for the editbox will be rendered masked or not.
	void SetTextMasked(bool textMasked);

	/// Sets the text validation string.
	void SetValidationString(const CEGUIString& validationString);

	/// Sets the UTF32 code point used when redenring the masked text.
	void SetMaskCodePoint(uint32 codePoint);

	/// Sets the maximum text length.
	void SetMaxTextLength(uint32 maximumLength);

};