#ifndef _EDITOR_CREATEPROJECTDIALOG_H_
#define _EDITOR_CREATEPROJECTDIALOG_H_

#include "Common.h"

namespace Editor {

	class CreateProjectDialog
	{
	public:
		///
		CreateProjectDialog();

		///
		~CreateProjectDialog();

		///
		void Init();

		///
		void OpenDialog();

		///
		void CreateProject();
		
		///
		void CancelDialog();
		
		void OpenLocationDialog();

	private:
		/// @name CEGUI Callbacks
		//@{
			bool OnChanged(const CEGUI::EventArgs&);
			bool OnButtonClicked(const CEGUI::EventArgs&);
		//@}

		/// FolderSelector callback
		void OnFolderSelected(const string& path, const string& editboxValue, bool canceled, int32 tag);

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
