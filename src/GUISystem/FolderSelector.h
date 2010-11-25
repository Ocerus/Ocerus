/// @file
/// Provides a widget for selecting a folder and optionally entering a string value.
#ifndef __GUISYSTEM_FOLDERSELECTOR_H__
#define __GUISYSTEM_FOLDERSELECTOR_H__

#include "Base.h"
#include "CEGUIForwards.h"
#include "Utils/Callback.h"

namespace GUISystem {

	/// The FolderSelector class provides a modal dialog for selecting a folder.
	class FolderSelector
	{
	public:

		/// Callback type that signals that user confirmed/cancelled the dialog. Parameters are:
		/// @param path
		/// @param editboxValue
		/// @param wasCancelled
		/// @param tag
		typedef Utils::Callback4<void, const string&, const string&, bool, int32> Callback;

		/// Constructs a FolderSelector instance.
		/// @param rootPath The path that will be considered as root, i.e user cannot get above this path.
		/// @param tag Tag value that will be passed to callback.
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
		inline void RegisterCallback(Callback callback) { mCallback = callback; }

		/// Sets the current folder.
		void SetCurrentFolder(string folder);

	private:

		/// @name CEGUI Callbacks
		//@{
			bool OnFolderListClicked(const CEGUI::EventArgs&);
			bool OnButtonClicked(const CEGUI::EventArgs&);
			bool OnEditboxKeyDown(const CEGUI::EventArgs&);
		//@}

		/// Changes the current directory to another.
		void ChangeFolder(const string& folder);

		/// Updates the GUI list of folders.
		void UpdateFolderList();

		/// Fills the out vector with folders inside the given directory.
		void ListDirectoryContent(const string& directory, vector<string>& out);

		/// Fills the out vector with logical drives.
		void ListDrives(vector<string>& out);

		/// Confirms the form.
		void Submit();

		/// Cancels the form.
		void Cancel();

		/// Returns the relative path to the current directory.
		string GetRelativePath(const string& absolutePath);

		CEGUI::Window* mWindow;
		CEGUI::Window* mButtonOK;
		CEGUI::Window* mButtonCancel;
		CEGUI::Window* mPathBox;
		CEGUI::Listbox* mFolderList;
		CEGUI::Window* mEditbox;

		int mTag;
		Callback mCallback;

		string mRootPath;
		string mCurrentPath;
		vector<string> mFolders;
	};
}

#endif // __GUISYSTEM_FOLDERSELECTOR_H__
