/// @file
/// Provides a modal message window.
#ifndef _GUISYSTEM_MESSAGEBOX_H_
#define _GUISYSTEM_MESSAGEBOX_H_

#include "Base.h"
#include "CEGUIForwards.h"

namespace GUISystem
{
	/// The MessageBox class provides a modal dialog for informing the user or for asking the user a question
	/// and receiving the answer. To use it, create and setup the MessageBox, register a callback to your callback
	/// function and then call Show(). You can use the ShowMessageBox function that does this in one step.
	class MessageBox
	{
	public:
		/// The type of message box.
		enum eMessageBoxType
		{
			MBT_OK,             ///< Displays message and OK button.
			MBT_OK_CANCEL,      ///< Displays message, OK and Cancel button.
			MBT_YES_NO,         ///< Displays message, Yes and No button.
			MBT_YES_NO_CANCEL	///< Displays message, Yes, No and Cancel button.
		};

		/// The type of button in message box.
		enum eMessageBoxButton
		{
			MBB_OK,      ///< OK button.
			MBB_CANCEL,  ///< Cancel button.
			MBB_YES,     ///< Yes button.
			MBB_NO       ///< No button.
		};

		/// The list of message box button types.
		typedef vector<eMessageBoxButton> MessageBoxButtons;

		/// Callback type that signals that user clicked a button. Parameters are:
		/// @param clickedButton
		/// @param tag
		typedef Utils::Callback2<void, eMessageBoxButton, int32> Callback;

		/// Constructs a MessageBox.
		/// @param type The type of the message box.
		/// @param tag The tag of the message box. This value is purely for client code usage and is passed to callback.
		MessageBox(eMessageBoxType type, int32 tag = 0);

		/// Destroys the MessageBox.
		~MessageBox();

		/// Sets the message box text to be displayed.
		void SetText(const CEGUI::String& text);

		/// Shows the message box.
		void Show();

		/// Registers a callback function that will be called when user clicks a button. Only the last registered function
		/// will be called.
		inline void RegisterCallback(Callback callback) { mCallback = callback; }

	private:
		/// @name CEGUI Callbacks
		//@{
			bool OnButtonClicked(const CEGUI::EventArgs&);
		//@}

		/// The list of pairs that map button type to its widget.
		typedef vector< pair<eMessageBoxButton, CEGUI::Window*> > Buttons;

		/// Sets the MessageBox to display specified button types (in given order).
		void SetButtons(const MessageBoxButtons& buttons);

		/// Returns the button widget for specified button type.
		CEGUI::Window* GetButton(eMessageBoxButton button);

		/// Makes sure the window is wide enough.
		void EnsureWindowIsWideEnough();

		eMessageBoxType mType;
		int32 mTag;
		Callback mCallback;
		CEGUI::Window* mMessageBox;
		Buttons mButtons;
		float32 mMinWidth;
	};

	/// Creates and shows a message box with specified parameters.
	/// @param text The displayed text.
	/// @param type The message box type (which button are visible).
	/// @param callback The function or method which will be called after the user clicks on some button.
	/// @param tag The ID which will be sent as a parameter to the callback method.
	void ShowMessageBox(const CEGUI::String& text, MessageBox::eMessageBoxType type = MessageBox::MBT_OK,
		MessageBox::Callback callback = MessageBox::Callback(), int32 tag = 0);
}

#endif // _GUISYSTEM_MESSAGEBOX_H_
