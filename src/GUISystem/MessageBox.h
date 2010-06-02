/// @file
/// Provides a modal message window.
#ifndef __GUISYSTEM_MESSAGEBOX_H__
#define __GUISYSTEM_MESSAGEBOX_H__

#include "Base.h"

namespace CEGUI
{
	class EventArgs;
	class String;
	class Window;
}

namespace GUISystem
{
	/**
	 * The MessageBox class provides a modal dialog for informing the user or for asking the user a question
	 * and receiving the answer.
	 */
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

		/// The base class for callback wrappers.
		class CallbackBase
		{
		public:
			/// Executes the callback.
			/// @param clickedButton The clicked button.
			/// @param tag The tag of the MessageBox.
			virtual void execute(eMessageBoxButton clickedButton, int32 tag) = 0;
		};

		/// A callback wrapper that wraps a method of an arbitrary class.
		template<class Class>
		class Callback: public CallbackBase
		{
		public:
			/// The type of callback method.
			typedef void (Class::*Method)(eMessageBoxButton, int32);

			/// Constructs a callback to specified method on specified instance.
			Callback(Class* instance, Method method): mInstance(instance), mMethod(method) {}

			/// Executes the callback.
			/// @param clickedButton The clicked button.
			/// @param tag The tag of the MessageBox.
			virtual void execute(eMessageBoxButton clickedButton, int32 tag)
			{
				(mInstance->*mMethod)(clickedButton, tag);
			}

		private:
			Class* mInstance;
			Method mMethod;
		};

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
		void RegisterCallback(CallbackBase* callback);

		/// @name CEGUI Callbacks
		//@{
			bool OnButtonClicked(const CEGUI::EventArgs&);
		//@}

	private:
		/// The list of pairs that map button type to its widget.
		typedef vector< pair<eMessageBoxButton, CEGUI::Window*> > Buttons;

		/// Sets the MessageBox to display specified button types (in given order).
		void SetButtons(const MessageBoxButtons& buttons);

		/// Returns the button widget for specified button type.
		CEGUI::Window* GetButton(eMessageBoxButton button);

		eMessageBoxType mType;
		int32 mTag;
		CallbackBase* mCallback;
		CEGUI::Window* mMessageBox;
		Buttons mButtons;
	};

	/// Creates and shows a message box with specified parameters.
	/// @param text The displayed text.
	/// @param type The message box type (which button are visible).
	/// @param callback The function or method which will be called after the user clicks on some button.
	/// @param tag The ID which will be sent as a parameter to the callback method.
	void ShowMessageBox(const CEGUI::String& text, MessageBox::eMessageBoxType type = MessageBox::MBT_OK,
		MessageBox::CallbackBase* callback = 0, int32 tag = 0);
}

#endif // __GUISYSTEM_MESSAGEBOX_H__
