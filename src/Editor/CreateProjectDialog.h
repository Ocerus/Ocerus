/// @file
/// Dialog for creating new project.
#ifndef _EDITOR_CREATEPROJECTDIALOG_H_
#define _EDITOR_CREATEPROJECTDIALOG_H_

#include "Common.h"

namespace Editor {
	/// The CreateProjectDialog class handles the dialog for creating new project.
	class CreateProjectDialog
	{
	public:
		/// Constructs a CreateProjectDialog.
		CreateProjectDialog();

		/// Destroys the CreateProjectDialog.
		~CreateProjectDialog();

		/// Initializes the dialog.
		void Init();

		/// Shows the dialog.
		void Show();

		/// Confirms the dialog. A new project shall be created according to entered values in dialog.
		void Confirm();
		
		/// Cancels the dialog.
		void Cancel();
		
		/// Opens the location dialog.
		void OpenLocationDialog();

	private:
		/// @name CEGUI Callbacks
		//@{
			bool OnChanged(const CEGUI::EventArgs&);
			bool OnButtonClicked(const CEGUI::EventArgs&);
		//@}

		/// FolderSelector callback
		void OnFolderSelected(const string& path, const string& editboxValue, bool canceled, int32 tag);

		/// Returns true, iff entered values are valid.
		bool IsValid();

		CEGUI::Window* mDialog;
		CEGUI::Window* mNameValue;
		CEGUI::Window* mLocationValue;
		CEGUI::Window* mOKButton;
		CEGUI::Window* mCancelButton;
		CEGUI::Window* mLocationButton;
	};

}

#endif // _EDITOR_CREATEPROJECTDIALOG_H_
