#ifndef __GUISYSTEM_FOLDERSELECTOR_H__
#define __GUISYSTEM_FOLDERSELECTOR_H__

#include "Base.h"
#include "CEGUIForwards.h"

namespace GUISystem {

	class FolderSelector
	{
	public:

		/// The base class for callback wrappers.
		class CallbackBase
		{
		public:
			/// Executes the callback.
			/// @param path The full path that was selected through the folder selector.
			/// @param canceled Whether the folder selector was canceled.
			/// @param tag The tag of the FolderSelector.
			virtual void execute(const string& path, bool canceled, int tag) = 0;
		};

		/// A callback wrapper that wraps a method of an arbitrary class.
		template<class Class>
		class Callback: public CallbackBase
		{
		public:
			/// The type of callback method.
			typedef void (Class::*Method)(const string&, bool, int);

			/// Constructs a callback to specified method on specified instance.
			Callback(Class* instance, Method method): mInstance(instance), mMethod(method) {}

			/// Executes the callback.
			/// @param path The full path that was selected through the folder selector.
			/// @param canceled Whether the folder selector was canceled.
			/// @param tag The tag of the FolderSelector.
			virtual void execute(const string& path, bool canceled, int tag)
			{
				(mInstance->*mMethod)(path, canceled, tag);
			}

		private:
			Class* mInstance;
			Method mMethod;
		};

		///
		FolderSelector(int tag = 0);

		///
		~FolderSelector();

		/// Shows the folder selector in the specified window.
		void Show();

		/// Hides the folder selector.
		void Hide();

		/// Registers a callback function that will be called when user clicks a button. Only the last registered function
		/// will be called.
		void RegisterCallback(CallbackBase* callback);

		/// Returns the selected file.
		string GetSelectedFile();

		/// Sets the current folder.
		void SetCurrentFolder(string folder);

	private:

		bool OnClicked(const CEGUI::EventArgs&);

		void ChangeFolder(const string& folder);
		
		///
		void UpdateFolderList();

		CEGUI::Window* mWindow;
		CEGUI::Window* mButtonOK;
		CEGUI::Window* mButtonCancel;
		CEGUI::Window* mPathBox;
		CEGUI::Listbox* mFolderList;

		int mTag;
		CallbackBase* mCallback;

		string mCurrentPath;
		vector<string> mFolders;
	};

}

#endif // __GUISYSTEM_FOLDERSELECTOR_H__
