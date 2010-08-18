/// @file
/// Provides a modal prompt window.
#ifndef _GUISYSTEM_PROMPTBOX_H_
#define _GUISYSTEM_PROMPTBOX_H_

#include "Base.h"
#include "CEGUIForwards.h"

namespace GUISystem
{
	/// The PromptBox class provides a modal dialog for allowing user enter a string.
	class PromptBox
	{
	public:
		/// Callback type that signals that user clicked a button. Parameters are:
		/// @param clickedOK Whether user confirmed the dialog.
		/// @param text The entered text.
		/// @param tag Tag that was used to create the PromptBox.
		typedef Utils::Callback3<void, bool, const string&, int32> Callback;

		/// Constructs a PromptBox.
		/// @param tag The tag of the prompt box. This value is purely for client code usage and is passed to callback.
		PromptBox(int32 tag = 0);

		/// Destroys the PromptBox.
		~PromptBox();

		/// Sets the prompt box text to be displayed.
		void SetText(const CEGUI::String& text);

		/// Shows the prompt box.
		void Show();

		/// Registers a callback function that will be called when user sends the prompt box. Only the
		/// last registered function will be called.
		inline void RegisterCallback(Callback callback) { mCallback = callback; }

	private:
		/// @name CEGUI Callbacks
		//@{
			bool OnButtonClicked(const CEGUI::EventArgs&);
			bool OnEditboxKeyDown(const CEGUI::EventArgs&);
		//@}

		/// Calls the registered callback with information from the dialog and destroys the instance.
		void SendPrompt(bool clickedOK);

		/// Makes sure the window is wide enough.
		void EnsureWindowIsWideEnough();

		int32 mTag;
		Callback mCallback;
		CEGUI::Window* mPromptBox;
		float32 mMinWidth;
	};

	/// Creates and shows a prompt box with specified parameters.
	/// @param text The displayed text.
	/// @param callback The function or method which will be called after the user clicks on some button.
	/// @param tag The ID which will be sent as a parameter to the callback method.
	void ShowPromptBox(const CEGUI::String& text, PromptBox::Callback callback = PromptBox::Callback(), int32 tag = 0);
}

#endif // _GUISYSTEM_PROMPTBOX_H_
