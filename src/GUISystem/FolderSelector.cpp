#include "Common.h"
#include "FolderSelector.h"
#include "CEGUITools.h"

using namespace GUISystem;


GUISystem::FolderSelector::FolderSelector(const string& rootPath, int32 tag):
	mWindow(0), mButtonOK(0), mButtonCancel(0), mPathBox(0), mFolderList(0), mEditbox(0), mTag(tag), mCallback(0), mRootPath(rootPath)
{
}

GUISystem::FolderSelector::~FolderSelector()
{
	delete mCallback;
	gGUIMgr.DestroyWindow(mWindow);
}

void GUISystem::FolderSelector::Show(const CEGUI::String& windowTitle, bool showEditbox, const CEGUI::String& editboxLabel)
{
	CEGUI::Window* root = gGUIMgr.GetGUISheet();

	mWindow = gGUIMgr.LoadSystemLayout("FolderSelector.layout", root->getName() + "/");
	mWindow->setAlwaysOnTop(true);
	mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame")->setText(windowTitle);
	root->addChildWindow(mWindow);

	mButtonOK = mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame/Ok");
	mButtonCancel = mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame/Cancel");
	mPathBox = mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame/Path");
	mFolderList = static_cast<CEGUI::Listbox*>(mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame/FolderList"));
	mEditbox = mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame/Editbox");

	mButtonOK->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GUISystem::FolderSelector::OnClicked, this));
	mButtonCancel->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GUISystem::FolderSelector::OnClicked, this));
	mFolderList->subscribeEvent(CEGUI::Listbox::EventMouseDoubleClick, CEGUI::Event::Subscriber(&GUISystem::FolderSelector::OnClicked, this));

	if (!showEditbox)
	{
		mEditbox->hide();
		mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame/EditboxLabel")->hide();
	}
	else
	{
		mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame/EditboxLabel")->setText(editboxLabel);
	}

	UpdateFolderList();
}

void FolderSelector::Hide()
{
	if (mWindow) gGUIMgr.DestroyWindow(mWindow);
	mWindow = 0;
}

void FolderSelector::RegisterCallback(FolderSelector::CallbackBase* callback)
{
	delete mCallback;
	mCallback = callback;
}

bool FolderSelector::OnClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	
	if (args.window == mFolderList)
	{
		CEGUI::ListboxItem* selectedItem = static_cast<CEGUI::ListboxItem*>(mFolderList->getFirstSelectedItem());
		if (selectedItem == 0)
			return false;
		ChangeFolder(mFolders[selectedItem->getID()]);
		UpdateFolderList();
	}
	else if (args.window == mButtonOK)
	{
		if (mCallback)
		{
			CEGUI::ListboxItem* selectedItem = static_cast<CEGUI::ListboxItem*>(mFolderList->getFirstSelectedItem());
			if (selectedItem == 0)
				return false;
			string path = mCurrentPath;//+ "/" + mFolders[selectedItem->getID()];
			string editboxValue = mEditbox->getText().c_str();
			mCallback->execute(GetRelativePath(path), editboxValue, false, mTag);
		}
		delete this;
	}
	else if (args.window == mButtonCancel)
	{
		if (mCallback)
			mCallback->execute("", "", true, mTag);
		delete this;
	}
	else
	{
		OC_NOT_REACHED();
	}
	return true;
}

void FolderSelector::ChangeFolder(const string& folder)
{
	boost::filesystem::path currentPath(mCurrentPath);
	if (folder == "..")
	{
		if (currentPath.has_parent_path() && (currentPath.parent_path().directory_string().length() >= mRootPath.length()))
			currentPath = currentPath.parent_path();
	}
	else
	{
		currentPath = currentPath / folder;
	}
	mCurrentPath = currentPath.directory_string();
}

void GUISystem::FolderSelector::UpdateFolderList()
{
	if (!boost::filesystem::is_directory(mCurrentPath))
	{
		mCurrentPath = boost::filesystem::current_path<boost::filesystem::path>().directory_string();
	}
	if (mCurrentPath.length() < mRootPath.length())
	{
		mCurrentPath = mRootPath;
	}

	mFolders.clear();
	mFolders.push_back("..");
	boost::filesystem::directory_iterator endIt;
	for (boost::filesystem::directory_iterator it(mCurrentPath); it != endIt; ++it)
	{
		if (boost::filesystem::is_directory(it->path()))
		{
			mFolders.push_back(it->filename());
		}
	}
	Containers::sort(mFolders.begin(), mFolders.end());

	mPathBox->setText(GetRelativePath(mCurrentPath));
	mFolderList->resetList();

	for (vector<string>::iterator it = mFolders.begin(); it != mFolders.end(); ++it)
	{
		CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(*it);
		item->setSelectionBrushImage("Vanilla-Images", "GenericBrush");
		item->setID(it - mFolders.begin());
		mFolderList->addItem(item);
	}
}

string GUISystem::FolderSelector::GetRelativePath(const string& absolutePath)
{
	if (absolutePath.length() < mRootPath.size())
		return "";
	else
		return absolutePath.substr(mRootPath.size());
}
