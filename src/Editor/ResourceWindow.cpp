#include "Common.h"
#include "ResourceWindow.h"
#include "PopupMenu.h"
#include "EditorMgr.h"
#include "ResourceSystem/ResourceMgr.h"
#include "Core/Project.h"
#include "GUISystem/CEGUICommon.h"
#include "GUISystem/PopupMgr.h"

using namespace Editor;

/// Compares two resources according to their path and returns whether the first is greater than the second.
bool ResourceComparator(const ResourceSystem::ResourcePtr& r1, const ResourceSystem::ResourcePtr& r2)
{
	int compareDirs = r1->GetFileDir().compare(r2->GetFileDir());
	return compareDirs == 0 ? (r1->GetFilePath().compare(r2->GetFilePath()) > 0) : (compareDirs > 0);
}

Editor::ResourceWindow::ResourceWindow()
{

}

Editor::ResourceWindow::~ResourceWindow()
{

}

void Editor::ResourceWindow::Init()
{
	CEGUI_TRY;
	{
		mWindow = gGUIMgr.LoadSystemLayout("ResourceWindow.layout", "EditorRoot/ResourceWindow");
		OC_ASSERT(mWindow);
		gGUIMgr.GetGUISheet()->addChildWindow(mWindow);

		mTree = static_cast<CEGUI::ItemListbox*>(mWindow->getChild(mWindow->getName() + "/List"));
		mTree->setUserString("WantsMouseWheel", "True");

		CEGUI::Window* refreshButton = mWindow->getChildRecursive(mWindow->getName() + "/Toolbar/Refresh");
		refreshButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnRefreshButtonClicked, this));
	}
	CEGUI_CATCH;

	OC_ASSERT(mWindow != 0);
	OC_ASSERT(mTree != 0);

	CreatePopupMenu();
	RebuildTree();
}

ResourceSystem::ResourcePtr ResourceWindow::GetItemAtIndex(size_t index)
{
	if (index >= mItems.size()) return ResourceSystem::ResourcePtr();
	return mItems.at(index);
}

void Editor::ResourceWindow::RebuildTree()
{
	for (int32 i=mTree->getItemCount()-1; i>=0; --i)
	{
		gGUIMgr.DestroyWindow(mTree->getItemFromIndex(i));
	}
	OC_ASSERT(mTree->getItemCount() == 0);
	mItems.clear();

	gResourceMgr.GetResources(mItems, ResourceSystem::BPT_PROJECT);
	Containers::sort(mItems.begin(), mItems.end(), ResourceComparator);

	vector<string> dirStack;
	for (vector<ResourceSystem::ResourcePtr>::const_iterator it = mItems.begin(); it != mItems.end(); ++it)
	{
		size_t resourceIndex = (size_t)(it - mItems.begin());
		string resourcePath = (*it)->GetRelativeFileDir();

		uint32 pathDepth = 0;
		size_t indexFrom = 0;
		size_t indexTo = 0;
		do
		{
			indexTo = resourcePath.find('/', indexFrom);
			if (indexTo == string::npos)
				indexTo = resourcePath.size();
			
			string dirName = resourcePath.substr(indexFrom, indexTo - indexFrom);
			if (dirName.empty()) continue;

			if ((pathDepth == dirStack.size()) || (dirName != dirStack[pathDepth]))
			{
				if (pathDepth == dirStack.size())
				{
					dirStack.push_back(dirName);
				}
				else
				{
					dirStack.resize(pathDepth + 1);
					dirStack[pathDepth] = dirName;
				}

				CEGUI::ItemEntry* dirItem = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("Editor/ListboxItem"));
				dirItem->setText(string(pathDepth * 4, ' ') + dirName);
				mTree->addChildWindow(dirItem);
			}

			++pathDepth;
			indexFrom = indexTo + 1;
		}
		while (indexTo != resourcePath.size());


		CEGUI::ItemEntry* newItem = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("Editor/ListboxItem"));
		newItem->setID(resourceIndex);
		
		CEGUI::Window* dragContainer = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("DragContainer"));
		dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		newItem->addChildWindow(dragContainer);

		CEGUI::Window* newItemText = gGUIMgr.CreateWindow("Editor/StaticText");
		newItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		newItemText->setText(string(pathDepth * 4, ' ') + boost::filesystem::path((*it)->GetName()).filename());
		newItemText->setProperty("FrameEnabled", "False");
		newItemText->setProperty("BackgroundEnabled", "False");
		newItemText->setMousePassThroughEnabled(true);

		dragContainer->addChildWindow(newItemText);
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnDragContainerMouseButtonDown, this));
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnDragContainerMouseButtonUp, this));
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnDragContainerMouseDoubleClick, this));

		dragContainer->setID(resourceIndex);
		dragContainer->setUserString("DragDataType", "Resource");
		dragContainer->setUserData(this);

		mTree->addChildWindow(newItem);
	}
}

void ResourceWindow::CreatePopupMenu()
{
	mPopupMenu = gPopupMgr->CreatePopupMenu("ResourceWindow/Popup");

	CEGUI::Window* changeTypeMenuItem = gPopupMgr->CreateMenuItem("ResourceWindow/Popup/ChangeType", TR("resource_change_type"), TR("resource_change_type_hint"), PI_INVALID);
	mPopupMenu->addChildWindow(changeTypeMenuItem);
	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("ResourceWindow/Popup/OpenScene", TR("open_scene"), TR("open_scene_hint"), PI_OPEN_SCENE));

	mResourceTypesPopupMenu = gPopupMgr->CreatePopupMenu("ResourceWindow/Popup/ChangeType/Popup");
	for (uint resType = 0; resType < ResourceSystem::NUM_RESTYPES; ++resType)
	{
		string resName = ResourceSystem::GetResourceTypeName((ResourceSystem::eResourceType)resType);
		CEGUI::Window* menuItem = gPopupMgr->CreateMenuItem("ResourceWindow/Popup/ChangeType/Resource" + Utils::StringConverter::ToString(resType), resName, "", resType);
		mResourceTypesPopupMenu->addChildWindow(menuItem);
	}
	changeTypeMenuItem->addChildWindow(mResourceTypesPopupMenu);
}

void ResourceWindow::DestroyPopupMenu()
{
	gPopupMgr->DestroyPopupMenu(mPopupMenu);
}

void ResourceWindow::SetCurrentResourceType(ResourceSystem::eResourceType resType)
{
	for (uint idx = 0; idx < mResourceTypesPopupMenu->getChildCount(); ++idx)
	{
		mResourceTypesPopupMenu->getChildAtIdx(idx)->setText(ResourceSystem::GetResourceTypeName((ResourceSystem::eResourceType)idx));
		if ((ResourceSystem::eResourceType)idx == resType)
			mResourceTypesPopupMenu->getChildAtIdx(idx)->appendText(" *");
	}
}

void ResourceWindow::OnPopupMenuItemClicked(CEGUI::Window* menuItem)
{
	if (menuItem->getParent() == mResourceTypesPopupMenu)
	{
		// Change type
		ResourceSystem::eResourceType newResType = (ResourceSystem::eResourceType)menuItem->getID();
		gResourceMgr.ChangeResourceType(mCurrentPopupResource, newResType);
		Refresh();
	}
	else
	{
		switch ((ePopupItem)menuItem->getID())
		{
		case PI_OPEN_SCENE:
			gEditorMgr.GetCurrentProject()->OpenScene(mCurrentPopupResource);
			break;
		default:
			OC_NOT_REACHED();
		}
	}
}

bool Editor::ResourceWindow::OnDragContainerMouseButtonDown(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	CEGUI::ItemEntry* itemEntry = static_cast<CEGUI::ItemEntry*>(args.window->getParent());
	itemEntry->setSelected(true);

	return true;
}

bool Editor::ResourceWindow::OnDragContainerMouseButtonUp(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	CEGUI::ItemEntry* itemEntry = static_cast<CEGUI::ItemEntry*>(args.window->getParent());
	itemEntry->setSelected(true);

	if (args.button == CEGUI::RightButton)
	{
		mCurrentPopupResource = GetItemAtIndex(dragContainer->getID());
		if (mCurrentPopupResource.get())
		{
			if (gEditorMgr.GetCurrentProject()->IsResourceScene(mCurrentPopupResource))
			{
				mPopupMenu->getChildAtIdx(1)->setEnabled(true);
			}
			else
			{
				mPopupMenu->getChildAtIdx(1)->setEnabled(false);
			}

			SetCurrentResourceType(mCurrentPopupResource->GetType());
			gPopupMgr->ShowPopup(mPopupMenu, args.position.d_x, args.position.d_y, GUISystem::PopupMgr::Callback(this, &Editor::ResourceWindow::OnPopupMenuItemClicked));
		}
		return true;
	}

	return true;
}

bool Editor::ResourceWindow::OnDragContainerMouseDoubleClick(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	if (args.button == CEGUI::LeftButton)
	{
		ResourceSystem::ResourcePtr resource = GetItemAtIndex(dragContainer->getID());
		if (resource.get() && gEditorMgr.GetCurrentProject()->IsResourceScene(resource))
		{
			gEditorMgr.GetCurrentProject()->OpenScene(resource);
		}
		return true;
	}

	return true;
}

bool Editor::ResourceWindow::OnRefreshButtonClicked(const CEGUI::EventArgs& e)
{
	OC_UNUSED(e);
	gResourceMgr.RefreshBasePathToGroup(ResourceSystem::BPT_PROJECT, "Project");
	RebuildTree();
	return true;
}
