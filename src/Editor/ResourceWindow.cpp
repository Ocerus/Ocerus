#include "Common.h"
#include "ResourceWindow.h"
#include "EditorMgr.h"
#include "Core/Project.h"
#include "GUISystem/CEGUICommon.h"
#include "GUISystem/PopupMgr.h"
#include "ResourceSystem/ResourceMgr.h"

using namespace Editor;

uint Editor::ResourceWindow::InvalidResourceIndex = (uint)-1;

Editor::ResourceWindow::ResourceWindow(): mUpdateTimer(0)
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
		gGUIMgr.LoadSystemImageset("ResourceWindowIcons.imageset");
		gGUIMgr.GetGUISheet()->addChildWindow(mWindow);

		mTree = static_cast<CEGUI::ItemListbox*>(mWindow->getChild(mWindow->getName() + "/List"));
		mTree->setSortMode(CEGUI::ItemListBase::UserSort);
		mTree->setSortCallback(&ResourceWindow::SortCallback);
		mTree->setUserString("WantsMouseWheel", "True");
	}
	CEGUI_CATCH;

	OC_ASSERT(mWindow != 0);
	OC_ASSERT(mTree != 0);

	CreatePopupMenu();
}

void ResourceWindow::Deinit()
{
	DestroyPopupMenu();
	DestroyItemEntryCache();
}

void ResourceWindow::Clear()
{
	while (mTree->getContentPane()->getChildCount() > 0)
	{
		CEGUI::Window* itemEntry = mTree->getContentPane()->getChildAtIdx(0);
		mTree->getContentPane()->removeChildWindow(itemEntry);
		StoreItemEntry(itemEntry);
	}
}

void ResourceWindow::Update(float32 delta)
{
	// Updates tree every second
	mUpdateTimer += delta;
	if (mUpdateTimer > 2)
	{
		mUpdateTimer = 0;
		if (UpdateResourcePool())
		{
			// Update tree only if resource pool was modified
			UpdateTree();
		}
	}
}

void ResourceWindow::Refresh()
{
	UpdateResourcePool();
	UpdateTree();
}

bool ResourceWindow::UpdateResourcePool()
{
	ResourceList resources;
	gResourceMgr.GetResources(resources, ResourceSystem::BPT_PROJECT);
	bool resourcePoolModified = false;

	// Check resources to be added to our pool
	for (ResourceList::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (Containers::find(mResourcePool.begin(), mResourcePool.end(), (*it)) == mResourcePool.end())
		{
			// Current resource is not present in our resource pool
			AddResourceToTree(*it);
			resourcePoolModified = true;
		}
	}

	// Check resources to be removed from our pool
	for (ResourceList::iterator it = mResourcePool.begin(); it != mResourcePool.end(); ++it)
	{
		if (Containers::find(resources.begin(), resources.end(), (*it)) == resources.end())
		{
			// A resource from our resource pool is not anymore in resource list
			// Set to null resource pointer.
			(*it) = ResourceSystem::ResourcePtr();
			resourcePoolModified = true;
		}
	}
	return resourcePoolModified;
}

void ResourceWindow::AddResourceToTree(const ResourceSystem::ResourcePtr& resource)
{
	mResourcePool.push_back(resource);
	uint resourceIndex = mResourcePool.size() - 1;

	CEGUI::Window* newMenuItem = RestoreResourceItemEntry();
	SetupResourceItemEntry(newMenuItem, resourceIndex);
	mTree->addChildWindow(newMenuItem);
	
	const string& resourcePath = resource->GetRelativeFileDir();
	size_t pathItemStart = 0;
	size_t pathItemEnd = 0;
	pathItemEnd = resourcePath.find('/', pathItemStart);
	while (pathItemEnd != string::npos)
	{
		const string& pathItem = resourcePath.substr(pathItemStart, pathItemEnd - pathItemStart);
		if (pathItem.empty()) continue;
		const string& parentPath = resourcePath.substr(0, pathItemStart);
		const string& fullPath = parentPath + '/' + pathItem;
		
		if (Containers::find(mDirectoryList.begin(), mDirectoryList.end(), fullPath) == mDirectoryList.end())
		{
			mDirectoryList.push_back(fullPath);

			CEGUI::Window* itemEntry = RestoreDirectoryItemEntry();
			SetupDirectoryItemEntry(itemEntry, parentPath, pathItem);
			mTree->addChildWindow(itemEntry);
		}
		pathItemStart = pathItemEnd + 1;
		pathItemEnd = resourcePath.find('/', pathItemStart);
	}
}

void ResourceWindow::UpdateTree()
{
	vector<CEGUI::Window*> menuItemsToBeDeleted;

	size_t childCount = mTree->getItemCount();
	for (size_t i = 0; i < childCount; ++i)
	{
		CEGUI::ItemEntry* menuItem = static_cast<CEGUI::ItemEntry*>(mTree->getItemFromIndex(i));
		if (!UpdateItemEntry(menuItem))
		{
			menuItemsToBeDeleted.push_back(menuItem);
		}
	}

	// delete items to be deleted
	for (vector<CEGUI::Window*>::const_iterator it = menuItemsToBeDeleted.begin(); it != menuItemsToBeDeleted.end(); ++it)
	{
		CEGUI::Window* menuItem = *it;
		menuItem->getParent()->removeChildWindow(menuItem);
		StoreItemEntry(menuItem);
	}

	mTree->sortList();
}

ResourceSystem::ResourcePtr ResourceWindow::MenuItemToResourcePtr(const CEGUI::Window* menuItem)
{
	uint index = menuItem->getID();
	if (index >= mResourcePool.size()) return ResourceSystem::ResourcePtr();
	return mResourcePool.at(index);
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

void ResourceWindow::OpenPopupMenu(ResourceSystem::ResourcePtr resource, float32 posX, float32 posY)
{
	mCurrentPopupResource = resource;
	if (mCurrentPopupResource.get())
	{
		// Enable/disable menu items
		if (gEditorMgr.GetCurrentProject()->IsResourceScene(mCurrentPopupResource))
			mPopupMenu->getChildAtIdx(1)->setEnabled(true);
		else
			mPopupMenu->getChildAtIdx(1)->setEnabled(false);

		// Check current resource type
		for (uint idx = 0; idx < mResourceTypesPopupMenu->getChildCount(); ++idx)
		{
			mResourceTypesPopupMenu->getChildAtIdx(idx)->setText(ResourceSystem::GetResourceTypeName((ResourceSystem::eResourceType)idx));
			if ((ResourceSystem::eResourceType)idx == mCurrentPopupResource->GetType())
				mResourceTypesPopupMenu->getChildAtIdx(idx)->appendText(" *");
		}

		// show popup
		gPopupMgr->ShowPopup(mPopupMenu, posX, posY, GUISystem::PopupMgr::Callback(this, &Editor::ResourceWindow::OnPopupMenuItemClicked));
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
	mCurrentPopupResource.reset();
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
		OpenPopupMenu(MenuItemToResourcePtr(dragContainer), args.position.d_x, args.position.d_y);
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
		ResourceSystem::ResourcePtr resource = MenuItemToResourcePtr(dragContainer);
		if (resource.get() && gEditorMgr.GetCurrentProject()->IsResourceScene(resource))
		{
			gEditorMgr.GetCurrentProject()->OpenScene(resource);
		}
		return true;
	}

	return true;
}

CEGUI::Window* Editor::ResourceWindow::CreateResourceItemEntry()
{
	static uint menuItemCounter = 0;
	const CEGUI::String& name = "ResourceWindow/Tree/MenuItem" + StringConverter::ToString(menuItemCounter++);
	CEGUI::ItemEntry* newItem = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindowDirectly("Editor/ListboxItem", name));
	newItem->setUserData(this);
	CEGUI::Window* dragContainer = gGUIMgr.CreateWindowDirectly("DragContainer", name + "/DC");
	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	newItem->addChildWindow(dragContainer);

	CEGUI::Window* newItemText = gGUIMgr.CreateWindowDirectly("Editor/StaticText", name + "/DC/Text");
	newItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 16), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	newItemText->setProperty("FrameEnabled", "False");
	newItemText->setProperty("BackgroundEnabled", "False");
	newItemText->setMousePassThroughEnabled(true);

	CEGUI::Window* newItemIcon = gGUIMgr.CreateWindowDirectly("Editor/StaticImage", name + "/DC/Icon");
	newItemIcon->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0.5, -6), CEGUI::UDim(0, 12), CEGUI::UDim(0.5, 6)));
	newItemIcon->setProperty("FrameEnabled", "False");
	newItemIcon->setProperty("BackgroundEnabled", "False");
	newItemIcon->setTooltipText("Directory");
	newItemIcon->setMousePassThroughEnabled(true);

	dragContainer->addChildWindow(newItemText);
	dragContainer->addChildWindow(newItemIcon);
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnDragContainerMouseButtonDown, this));
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnDragContainerMouseButtonUp, this));
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnDragContainerMouseDoubleClick, this));
	dragContainer->setUserString("DragDataType", "Resource");
	dragContainer->setUserData(this);
	return newItem;
}

CEGUI::Window* ResourceWindow::CreateDirectoryItemEntry()
{
	static uint dirCounter = 0;
	const CEGUI::String& windowName = "ResourceWindow/Tree/DirItem" + StringConverter::ToString(dirCounter++);
	CEGUI::Window* itemEntry = gGUIMgr.CreateWindowDirectly("Editor/ListboxItem", windowName);
	itemEntry->setID(InvalidResourceIndex);
	CEGUI::Window* dragContainer = gGUIMgr.CreateWindowDirectly("DragContainer", windowName + "/DC");
	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	itemEntry->addChildWindow(dragContainer);

	CEGUI::Window* itemEntryIcon = gGUIMgr.CreateWindowDirectly("Editor/StaticImage", windowName + "/DC/Icon");
	itemEntryIcon->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0.5, -6), CEGUI::UDim(0, 12), CEGUI::UDim(0.5, 6)));
	itemEntryIcon->setProperty("FrameEnabled", "False");
	itemEntryIcon->setProperty("BackgroundEnabled", "False");
	itemEntryIcon->setTooltipText("Directory");
	itemEntryIcon->setProperty("Image", "set:ResourceWindowIcons image:Directory");
	dragContainer->addChildWindow(itemEntryIcon); 

	CEGUI::Window* itemEntryText = gGUIMgr.CreateWindowDirectly("Editor/ListboxItem", windowName + "/DC/Text");	
	itemEntryText->setArea(CEGUI::URect(CEGUI::UDim(0, 16), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	dragContainer->addChildWindow(itemEntryText);
	return itemEntry;
}

void ResourceWindow::SetupResourceItemEntry(CEGUI::Window* menuItem, uint resourceIndex)
{
	OC_DASSERT(resourceIndex < mResourcePool.size());
	ResourceSystem::ResourcePtr res = mResourcePool[resourceIndex];
	OC_DASSERT(res.get() != 0);

	uint indentLevel = GetPathIndentLevel(res->GetRelativeFileDir());

	menuItem->setID(resourceIndex);
	menuItem->setUserString("ResourceDir", res->GetRelativeFileDir());
	menuItem->setUserString("ResourcePath", res->GetRelativeFilePath());

	CEGUI::Window* dragContainer = menuItem->getChildAtIdx(0);
	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, indentLevel * 16), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	dragContainer->setID(resourceIndex);

	CEGUI::Window* menuItemText = dragContainer->getChildAtIdx(0);
	menuItemText->setText(boost::filesystem::path(res->GetName()).filename());
}

void ResourceWindow::SetupDirectoryItemEntry(CEGUI::Window* itemEntry, const string& parentPath, const string& directory)
{
	CEGUI::Window* dragContainer = itemEntry->getChildAtIdx(0);
	CEGUI::Window* itemEntryText = dragContainer->getChildAtIdx(1);
	uint indentLevel = GetPathIndentLevel(parentPath);

	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, indentLevel * 16), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	itemEntryText->setText(directory);
	itemEntry->setUserString("ResourceDir", parentPath);
	itemEntry->setUserString("ResourcePath", parentPath + directory);
}

bool ResourceWindow::UpdateItemEntry(CEGUI::Window* menuItem)
{
	uint resourceIndex = menuItem->getID();

	// Directory ItemEntry does not need updates
	if (resourceIndex == InvalidResourceIndex) return true;

	OC_DASSERT(resourceIndex < mResourcePool.size());
	ResourceSystem::ResourcePtr res = mResourcePool[resourceIndex];
	if (res.get() == 0) return false; // item to be deleted

	CEGUI::Window* menuItemIcon = menuItem->getChildAtIdx(0)->getChildAtIdx(1);
	menuItemIcon->setTooltipText(ResourceSystem::GetResourceTypeName(res->GetType()));
	menuItemIcon->setProperty("Image", "set:ResourceWindowIcons image:Resource" + ResourceSystem::GetResourceTypeName(res->GetType()));
	return true;
}

void Editor::ResourceWindow::StoreItemEntry(CEGUI::Window* itemEntry)
{
	if (itemEntry->getID() == InvalidResourceIndex)
		mDirectoryItemEntryCache.push_back(itemEntry);
	else	
		mResourceItemEntryCache.push_back(itemEntry);
}

CEGUI::Window* Editor::ResourceWindow::RestoreResourceItemEntry()
{
	CEGUI::Window* itemEntry = 0;
	if (mResourceItemEntryCache.empty())
	{
		itemEntry = CreateResourceItemEntry();
	}
	else
	{
		itemEntry = mResourceItemEntryCache.back();
		mResourceItemEntryCache.pop_back();
	}
	return itemEntry;
}

CEGUI::Window* Editor::ResourceWindow::RestoreDirectoryItemEntry()
{
	CEGUI::Window* itemEntry = 0;
	if (mDirectoryItemEntryCache.empty())
	{
		itemEntry = CreateDirectoryItemEntry();
	}
	else
	{
		itemEntry = mDirectoryItemEntryCache.back();
		mDirectoryItemEntryCache.pop_back();
	}
	return itemEntry;
}

void ResourceWindow::DestroyItemEntryCache()
{
	for (ItemEntryCache::iterator it = mResourceItemEntryCache.begin(); it != mResourceItemEntryCache.end(); ++it)
	{
		gGUIMgr.DestroyWindowDirectly((*it));
	}
	mResourceItemEntryCache.clear();

	for (ItemEntryCache::iterator it = mDirectoryItemEntryCache.begin(); it != mDirectoryItemEntryCache.end(); ++it)
	{
		gGUIMgr.DestroyWindowDirectly((*it));
	}
	mDirectoryItemEntryCache.clear();
}

uint ResourceWindow::GetPathIndentLevel(const string& path)
{
	uint indentLevel = 0;
	size_t pos = path.find('/');
	while (pos != string::npos)
	{
		indentLevel++;
		pos = path.find('/', pos + 1);
	}
	return indentLevel;
}

bool ResourceWindow::SortCallback(const CEGUI::ItemEntry* first, const CEGUI::ItemEntry* second)
{
	const CEGUI::String& firstResourcePath = first->getUserString("ResourcePath");
	const CEGUI::String& secondResourcePath = second->getUserString("ResourcePath");
	return firstResourcePath < secondResourcePath;
/*
	ocInfo << "\n\n--Comparement--";
	const CEGUI::String& firstResourceDir = first->getUserString("ResourceDir");
	const CEGUI::String& secondResourceDir = second->getUserString("ResourceDir");
	const CEGUI::String& firstResourcePath = first->getUserString("ResourcePath");
	const CEGUI::String& secondResourcePath = second->getUserString("ResourcePath");
	bool isFirstDirectory = (first->getID() == InvalidResourceIndex);
	bool isSecondDirectory = (second->getID() == InvalidResourceIndex);
	ocInfo << "ResourceDirs: '"  << firstResourceDir << "' vs '" << secondResourceDir << "'";
	ocInfo << "ResourcePaths: '" << firstResourcePath << "' vs '" << secondResourcePath << "'";
	ocInfo << "IsDirectory: " << isFirstDirectory << " vs " << isSecondDirectory;
	if (firstResourceDir == secondResourceDir)
	{
		if (isFirstDirectory && !isSecondDirectory) return true;
		if (isSecondDirectory) return false;

		const CEGUI::String& firstResourcePath = first->getUserString("ResourcePath");
		const CEGUI::String& secondResourcePath = second->getUserString("ResourcePath");
		return firstResourcePath < secondResourcePath;
	}
	else
	{
		return firstResourceDir < secondResourceDir;
	}
*/
}
