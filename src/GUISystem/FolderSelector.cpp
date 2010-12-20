#include "Common.h"
#include "FolderSelector.h"

#include "CEGUICommon.h"

using namespace GUISystem;


GUISystem::FolderSelector::FolderSelector(const string& rootPath, int32 tag):
	mWindow(0), mButtonOK(0), mButtonCancel(0), mPathBox(0), mFolderList(0), mEditbox(0), mTag(tag), mRootPath(rootPath)
{
	mCurrentPath = boost::filesystem::current_path<boost::filesystem::path>().directory_string();
}

GUISystem::FolderSelector::~FolderSelector()
{
	gGUIMgr.DestroyWindow(mWindow);
}

void GUISystem::FolderSelector::Show(const CEGUI::String& windowTitle, bool showEditbox, const CEGUI::String& editboxLabel)
{
	bool success = false;
	OC_CEGUI_TRY;
	{
		CEGUI::Window* root = gGUIMgr.GetGUISheet();

		mWindow = gGUIMgr.LoadSystemLayout("FolderSelector.layout", root->getName() + "/");
		mWindow->setAlwaysOnTop(true);
		mWindow->setModalState(true);
		root->addChildWindow(mWindow);
		
		CEGUI::Window* frame = mWindow->getChild(root->getName() + "/FolderSelector/Frame");
		frame->setText(windowTitle);

		mButtonOK = frame->getChild(root->getName() + "/FolderSelector/ButtonOK");
		mButtonCancel = frame->getChild(root->getName() + "/FolderSelector/ButtonCancel");
		mPathBox = frame->getChild(root->getName() + "/FolderSelector/PathBox");
		mFolderList = static_cast<CEGUI::Listbox*>(frame->getChild(root->getName() + "/FolderSelector/FolderList"));
		mFolderList->setWantsMultiClickEvents(true);
		mEditbox = frame->getChild(root->getName() + "/FolderSelector/Editbox");
		mEditbox->subscribeEvent(CEGUI::Editbox::EventKeyDown,
				CEGUI::Event::Subscriber(&GUISystem::FolderSelector::OnEditboxKeyDown, this));
		mButtonOK->subscribeEvent(CEGUI::PushButton::EventClicked,
				CEGUI::Event::Subscriber(&GUISystem::FolderSelector::OnButtonClicked, this));
		mButtonCancel->subscribeEvent(CEGUI::PushButton::EventClicked,
				CEGUI::Event::Subscriber(&GUISystem::FolderSelector::OnButtonClicked, this));
		mFolderList->subscribeEvent(CEGUI::Listbox::EventMouseDoubleClick,
				CEGUI::Event::Subscriber(&GUISystem::FolderSelector::OnFolderListDoubleClicked, this));

		if (!showEditbox)
		{
			mEditbox->hide();
			frame->getChild(root->getName() + "/FolderSelector/EditboxLabel")->hide();
		}
		else
		{
			frame->getChild(root->getName() + "/FolderSelector/EditboxLabel")->setText(editboxLabel);
			mEditbox->activate();
		}

		UpdateFolderList();
		success = true;
	}
	OC_CEGUI_CATCH;

	if (!success)
	{
		ocError << "Cannot show FolderSelector.";
		delete this;
	}
}

void FolderSelector::Hide()
{
	if (mWindow) gGUIMgr.DestroyWindow(mWindow);
	mWindow = 0;
}

bool FolderSelector::OnFolderListDoubleClicked(const CEGUI::EventArgs&)
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
	if (mCallback.IsSet())
	{
		CEGUI::ListboxItem* selectedItem = static_cast<CEGUI::ListboxItem*>(mFolderList->getFirstSelectedItem());
		if (selectedItem != 0)
		{
			ChangeFolder(mFolders[selectedItem->getID()]);
		}
		const string& path = GetRelativePath(mCurrentPath);
		const string& editboxValue = mEditbox->getText().c_str();
		mCallback.Call(path, editboxValue, false, mTag);
	}
	delete this;
}


void FolderSelector::Cancel()
{
	if (mCallback.IsSet())
		mCallback.Call("", "", true, mTag);
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
			{
				currentPath = currentPath.parent_path();
			}
		}
		else
		{
			currentPath = currentPath / folder;
		}

		if (boost::filesystem::is_directory(currentPath.directory_string()))
		{
			mCurrentPath = currentPath.directory_string();
		}

		if (mCurrentPath[mCurrentPath.length()-1] == ':')
		{
			// this is a E: type of path. We want rather a selection of drives. Let's clear the path altogether to designate that.
			mCurrentPath.clear();
		}
	}
	catch (const boost::system::system_error& e)
	{
		ocWarning << "System exception caught: " << e.what();
	}
}

void GUISystem::FolderSelector::UpdateFolderList()
{
	if (!mCurrentPath.empty() && !boost::filesystem::is_directory(mCurrentPath))
	{
		ocWarning << "Current path '" << mCurrentPath << "' is an incorrect directory; using current directory";
		mCurrentPath = boost::filesystem::current_path<boost::filesystem::path>().directory_string();
	}

	if (mCurrentPath.length() < mRootPath.length())
	{
		mCurrentPath = mRootPath;
	}

	mFolders.clear();
	if (mCurrentPath.empty())
	{
		// if the path is empty it means we're listing drives
		ListDrives(mFolders);
	}
	else
	{
		ListDirectoryContent(mCurrentPath, mFolders);
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

void GUISystem::FolderSelector::ListDirectoryContent( const string& directory, vector<string>& out )
{
	out.push_back("..");
	try
	{
		boost::filesystem::directory_iterator endIt;
		for (boost::filesystem::directory_iterator it(directory); it != endIt; ++it)
		{
			if (boost::filesystem::is_directory(it->path()))
			{
				out.push_back(it->filename());
			}
		}
	}
	catch (const boost::system::system_error& e)
	{
		ocWarning << "System exception caught: " << e.what();
	}
}


#if defined(__WIN__)

#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include <tchar.h>

void GUISystem::FolderSelector::ListDrives( vector<string>& out )
{
	TCHAR szBuffer[1024];
	::GetLogicalDriveStrings(1024, szBuffer);
	TCHAR *pch = szBuffer;
	while (*pch) {
		out.push_back(pch);
		pch = &pch[_tcslen(pch) + 1];
	}
}

#else

void GUISystem::FolderSelector::ListDrives( vector<string>& out )
{
	OC_UNUSED(out);
	OC_FAIL("There are no drives in Linux paths");
}

#endif
