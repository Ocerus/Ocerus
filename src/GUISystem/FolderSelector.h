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
			virtual void execute(const string& path, const string& editboxValue, bool canceled, int32 tag) = 0;
		};

		/// A callback wrapper that wraps a method of an arbitrary class.
		template<class Class>
		class Callback: public CallbackBase
		{
		public:
			/// The type of callback method.
			typedef void (Class::*Method)(const string&, const string&, bool, int32);

			/// Constructs a callback to specified method on specified instance.
			Callback(Class* instance, Method method): mInstance(instance), mMethod(method) {}

			/// Executes the callback.
			/// @param path The full path that was selected through the folder selector.
			/// @param canceled Whether the folder selector was canceled.
			/// @param tag The tag of the FolderSelector.
			virtual void execute(const string& path, const string& editboxValue, bool canceled, int32 tag)
			{
				(mInstance->*mMethod)(path, editboxValue, canceled, tag);
			}

		private:
			Class* mInstance;
			Method mMethod;
		};

		/// Constructs a FolderSelector instance.
		FolderSelector(const string& rootPath = "", int32 tag = 0);

		/// Destroys the FolderSelector instance.
		~FolderSelector();

		/// Shows the folder selector in the specified window.
		/// @param showEditbox Whether the folder selector should contain an editbox.
		void Show(const CEGUI::String& windowTitle, bool showEditbox = false, const CEGUI::String& editboxLabel = "");

		/// Hides the folder selector.
		void Hide();

		/// Registers a callback function that will be called when user clicks a button. Only the last registered function
		/// will be called.
		void RegisterCallback(CallbackBase* callback);

		/// Sets the current folder.
		void SetCurrentFolder(string folder);

	private:

		/// @name CEGUI Callbacks
		//@{
			bool OnFolderListClicked(const CEGUI::EventArgs&);
			bool OnButtonClicked(const CEGUI::EventArgs&);
		//@}

		void ChangeFolder(const string& folder);
		
		void UpdateFolderList();
		
		string GetRelativePath(const string& absolutePath);

		CEGUI::Window* mWindow;
		CEGUI::Window* mButtonOK;
		CEGUI::Window* mButtonCancel;
		CEGUI::Window* mPathBox;
		CEGUI::Listbox* mFolderList;
		CEGUI::Window* mEditbox;

		int mTag;
		CallbackBase* mCallback;

		string mRootPath;
		string mCurrentPath;
		vector<string> mFolders;
	};

}

#endif // __GUISYSTEM_FOLDERSELECTOR_H__
