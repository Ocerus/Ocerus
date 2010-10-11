#include "Common.h"

#include "CreateProjectDialog.h"
#include "EditorMgr.h"
#include "GUISystem/CEGUICommon.h"
#include "GUISystem/FolderSelector.h"

using namespace Editor;

Editor::CreateProjectDialog::CreateProjectDialog():
	mDialog(0),
	mNameValue(0),
	mLocationValue(0),
	mOKButton(0),
	mCancelButton(0),
	mLocationButton(0)
{
}

Editor::CreateProjectDialog::~CreateProjectDialog()
{
	gGUIMgr.DestroyWindow(mDialog);
}

void CreateProjectDialog::Init()
{
	CEGUI_TRY;
	{
		mDialog = gGUIMgr.LoadSystemLayout("CreateProject.layout", "CreateProjectDialog");
		mNameValue = mDialog->getChildRecursive("CreateProjectDialog/NameValue");
		mLocationValue = mDialog->getChildRecursive("CreateProjectDialog/LocationValue");
		mOKButton = mDialog->getChildRecursive("CreateProjectDialog/OKButton");
		mCancelButton = mDialog->getChildRecursive("CreateProjectDialog/CancelButton");
		mLocationButton = mDialog->getChildRecursive("CreateProjectDialog/LocationButton");

		mNameValue->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber(&Editor::CreateProjectDialog::OnChanged, this));
		mLocationValue->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber(&Editor::CreateProjectDialog::OnChanged, this));
		mOKButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::CreateProjectDialog::OnButtonClicked, this));
		mCancelButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::CreateProjectDialog::OnButtonClicked, this));
		mLocationButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::CreateProjectDialog::OnButtonClicked, this));

		gGUIMgr.GetGUISheet()->addChildWindow(mDialog);
		mDialog->hide();
	}
	CEGUI_CATCH;
}

void CreateProjectDialog::Show()
{
	if (mDialog == 0)
		Init();

	mNameValue->setText("");
	mLocationValue->setText("");
	mOKButton->setEnabled(false);
	mDialog->setModalState(true);
	mDialog->show();
}

void CreateProjectDialog::Cancel()
{
	mDialog->hide();
	mDialog->setModalState(false);
}

void CreateProjectDialog::OpenLocationDialog()
{
	mDialog->setModalState(false);
	GUISystem::FolderSelector* folderSelector = new GUISystem::FolderSelector("");
	folderSelector->RegisterCallback(GUISystem::FolderSelector::Callback(this, &Editor::CreateProjectDialog::OnFolderSelected));
	folderSelector->Show(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "create_project_folder"));
}

void CreateProjectDialog::Confirm()
{
	OC_DASSERT(IsValid());
	string name = mNameValue->getText().c_str();
	string location = mLocationValue->getText().c_str();
	gEditorMgr.Reset();
	gEditorMgr.CreateProject(location + '/' + name);
	mDialog->hide();
	mDialog->setModalState(false);
}

bool CreateProjectDialog::OnChanged(const CEGUI::EventArgs& )
{
	mOKButton->setEnabled(IsValid());
	return true;
}

bool CreateProjectDialog::OnButtonClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	if (args.window == mOKButton)
	{
		Confirm();
	}
	else if (args.window == mCancelButton)
	{
		Cancel();
	}
	else if (args.window == mLocationButton)
	{
		OpenLocationDialog();
	}
	else
	{
		OC_NOT_REACHED();
	}
	return true;
}

void CreateProjectDialog::OnFolderSelected(const string& path, const string& editboxValue, bool canceled, int32 tag)
{
	OC_UNUSED(editboxValue);
	OC_UNUSED(tag);
	mDialog->setModalState(true);
	if (canceled)
		return;
	mLocationValue->setText(path);
}


bool CreateProjectDialog::IsValid()
{
	if (mNameValue->getText().empty())
		return false;

	///@todo Check location exists

	///@todo Check project name does not exist in location

	return true;
}