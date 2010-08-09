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
	gGUIMgr.DestroyWindowDirectly(mWindow);
}

void GUISystem::FolderSelector::Show(const CEGUI::String& windowTitle, bool showEditbox, const CEGUI::String& editboxLabel)
{
	CEGUI::Window* root = gGUIMgr.GetGUISheet();

	mWindow = gGUIMgr.LoadSystemLayout("FolderSelector.layout", root->getName() + "/");
	mWindow->setAlwaysOnTop(true);
	mWindow->setModalState(true);
	mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame")->setText(windowTitle);
	root->addChildWindow(mWindow);

	mButtonOK = mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame/Ok");
	mButtonCancel = mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame/Cancel");
	mPathBox = mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame/Path");
	mFolderList = static_cast<CEGUI::Listbox*>(mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame/FolderList"));
	mFolderList->setWantsMultiClickEvents(false);
	mEditbox = mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame/Editbox");
	mEditbox->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&GUISystem::FolderSelector::OnEditboxKeyDown, this));

	mButtonOK->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GUISystem::FolderSelector::OnButtonClicked, this));
	mButtonCancel->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GUISystem::FolderSelector::OnButtonClicked, this));
	mFolderList->subscribeEvent(CEGUI::Listbox::EventMouseClick, CEGUI::Event::Subscriber(&GUISystem::FolderSelector::OnFolderListClicked, this));

	if (!showEditbox)
	{
		mEditbox->hide();
		mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame/EditboxLabel")->hide();
	}
	else
	{
		mWindow->getChildRecursive(root->getName() + "/FolderSelector/Frame/EditboxLabel")->setText(editboxLabel);
		mEditbox->activate();
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

bool FolderSelector::OnFolderListClicked(const CEGUI::EventArgs&)
{
	CEGUI::ListboxItem* selectedItem = static_cast<CEGUI::ListboxItem*>(mFolderList->getFirstSelectedItem());
	if (selectedItem == 0)
		return false;
	ChangeFolder(mFolders[selectedItem->getID()]);
	UpdateFolderList();
	return true;
}

bool FolderSelector::OnButtonClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	if (args.window == mButtonOK)
	{
		Submit();
	}
	else
	{
		Cancel();
	}
	return true;
}

bool FolderSelector::OnEditboxKeyDown(const CEGUI::EventArgs& args)
{
	const CEGUI::KeyEventArgs& keyArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
	switch (keyArgs.scancode)
	{
		case CEGUI::Key::Return:
		case CEGUI::Key::NumpadEnter:
			Submit();
			return true;
		case CEGUI::Key::Escape:
			Cancel();
		default:
			break;
	}
	return false;
}

void FolderSelector::Submit()
{
	if (mCallback)
	{
		const string& path = GetRelativePath(mCurrentPath);
		const string& editboxValue = mEditbox->getText().c_str();
		mCallback->execute(path, editboxValue, false, mTag);
	}
	delete this;
}


void FolderSelector::Cancel()
{
	if (mCallback)
		mCallback->execute("", "", true, mTag);
	delete this;
}



void FolderSelector::ChangeFolder(const string& folder)
{
	try
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
		if (boost::filesystem::is_directory(currentPath.directory_string()))
			mCurrentPath = currentPath.directory_string();
	}
	catch (const boost::system::system_error& e)
	{
		ocWarning << "System exception caught: " << e.what();
	}
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
	try
	{
		boost::filesystem::directory_iterator endIt;
		for (boost::filesystem::directory_iterator it(mCurrentPath); it != endIt; ++it)
		{
			if (boost::filesystem::is_directory(it->path()))
			{
				mFolders.push_back(it->filename());
			}
		}
	}
	catch (const boost::system::system_error& e)
	{
		ocWarning << "System exception caught: " << e.what();
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
