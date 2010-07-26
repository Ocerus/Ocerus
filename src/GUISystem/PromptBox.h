/// @file
/// Provides a modal prompt window.
#ifndef __GUISYSTEM_PROMPTBOX_H__
#define __GUISYSTEM_PROMPTBOX_H__

#include "Base.h"

namespace CEGUI
{
	class EventArgs;
	class String;
	class Window;
}

namespace GUISystem
{
	/// The PromptBox class provides a modal dialog for letting user enter a string.
	class PromptBox
	{
	public:

		/// The base class for callback wrappers.
		class CallbackBase
		{
		public:
			/// Executes the callback.
			/// @param clickedOK Whether the prompt was clickedOK
			/// @param text Text entered.
			/// @param tag The tag of the PromptBox.
			virtual void execute(bool clickedOK, string text, int32 tag) = 0;
		};

		/// A callback wrapper that wraps a method of an arbitrary class.
		template<class Class>
		class Callback: public CallbackBase
		{
		public:
			/// The type of callback method.
			typedef void (Class::*Method)(bool, string, int32);

			/// Constructs a callback to specified method on specified instance.
			Callback(Class* instance, Method method): mInstance(instance), mMethod(method) {}

			/// Executes the callback.
			/// @param clickedOK Whether the prompt was clickedOK
			/// @param text Text entered.
			/// @param tag The tag of the PromptBox.
			virtual void execute(bool clickedOK, string text, int32 tag)
			{
				(mInstance->*mMethod)(clickedOK, text, tag);
			}

		private:
			Class* mInstance;
			Method mMethod;
		};

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
		void RegisterCallback(CallbackBase* callback);

	private:

		/// @name CEGUI Callbacks
		//@{
			bool OnButtonClicked(const CEGUI::EventArgs&);
			bool OnEditboxKeyDown(const CEGUI::EventArgs&);
		//@}

		void SendPrompt(bool clickedOK);

		/// Makes sure the window is wide enough.
		void EnsureWindowIsWideEnough();

		int32 mTag;
		CallbackBase* mCallback;
		CEGUI::Window* mPromptBox;
		float32 mMinWidth;
	};

	/// Creates and shows a prompt box with specified parameters.
	/// @param text The displayed text.
	/// @param callback The function or method which will be called after the user clicks on some button.
	/// @param tag The ID which will be sent as a parameter to the callback method.
	void ShowPromptBox(const CEGUI::String& text, PromptBox::CallbackBase* callback = 0, int32 tag = 0);
}

#endif // __GUISYSTEM_PROMPTBOX_H__
