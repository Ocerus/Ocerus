#include "Common.h"
#include "ResourceWindow.h"
#include "EditorMgr.h"
#include "Core/Project.h"
#include "GUISystem/CEGUICommon.h"
#include "GUISystem/PopupMgr.h"
#include "ResourceSystem/ResourceMgr.h"
#include "Core/Application.h"

using namespace Editor;

uint32 Editor::ResourceWindow::InvalidResourceIndex = (uint32)-1;

Editor::ResourceWindow::ResourceWindow():
	mPopupMenu(0),
	mResourceTypesPopupMenu(0),
	mWindow(0),
	mTree(0)
{
}

Editor::ResourceWindow::~ResourceWindow()
{
	if (mWindow || mPopupMenu)
	{
		ocError << "ResourceWindow was destroyed without deinitialization.";
	}
}

void Editor::ResourceWindow::Init()
{
	OC_CEGUI_TRY;
	{
		mWindow = gGUIMgr.GetWindow("Editor/ResourceWindow");
		OC_ASSERT(mWindow);
		gGUIMgr.LoadSystemImageset("ResourceWindowIcons.imageset");
		mTree = static_cast<CEGUI::ItemListbox*>(gGUIMgr.CreateWindow("Editor/ItemListbox", "Editor/ResourceWindow/Tree"));
		mTree->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		mTree->setSortMode(CEGUI::ItemListBase::UserSort);
		mTree->setSortCallback(&ResourceWindow::SortCallback);
		mTree->setUserString("WantsMouseWheel", "True");
		mWindow->addChildWindow(mTree);
	}
	OC_CEGUI_CATCH;

	OC_ASSERT(mWindow != 0);
	OC_ASSERT(mTree != 0);

	CreatePopupMenu();
}

void ResourceWindow::Deinit()
{
	gGUIMgr.DestroyWindow(mWindow);
	mWindow = 0;
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
	mDirectoryList.clear();
}

void ResourceWindow::Update()
{
	if (UpdateResourcePool())
	{
		// Update tree only if resource pool was modified
		UpdateTree();
	}
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
	size_t resourceIndex = mResourcePool.size() - 1;

	CEGUI::Window* newItemEntry = RestoreResourceItemEntry();
	SetupResourceItemEntry(newItemEntry, resourceIndex);
	mTree->addChildWindow(newItemEntry);
	
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
	vector<CEGUI::Window*> itemEntriesToBeDeleted;

	size_t childCount = mTree->getItemCount();
	for (size_t i = 0; i < childCount; ++i)
	{
		CEGUI::ItemEntry* itemEntry = static_cast<CEGUI::ItemEntry*>(mTree->getItemFromIndex(i));
		if (!UpdateItemEntry(itemEntry))
		{
			itemEntriesToBeDeleted.push_back(itemEntry);
		}
	}

	// delete items to be deleted
	for (vector<CEGUI::Window*>::const_iterator it = itemEntriesToBeDeleted.begin(); it != itemEntriesToBeDeleted.end(); ++it)
	{
		CEGUI::Window* itemEntry = *it;
		itemEntry->getParent()->removeChildWindow(itemEntry);
		StoreItemEntry(itemEntry);
	}

	mTree->sortList();
}

ResourceSystem::ResourcePtr ResourceWindow::ItemEntryToResourcePtr(const CEGUI::Window* itemEntry)
{
	uint32 index = itemEntry->getID();
	if (index >= mResourcePool.size()) return ResourceSystem::ResourcePtr();
	return mResourcePool.at(index);
}

void ResourceWindow::CreatePopupMenu()
{
	mPopupMenu = gPopupMgr->CreatePopupMenu("Editor/ResourceWindow/Popup");

	CEGUI::Window* changeTypeMenuItem = gPopupMgr->CreateMenuItem("Editor/ResourceWindow/Popup/ChangeType", TR("resource_change_type"), TR("resource_change_type_hint"), PI_INVALID);
	mPopupMenu->addChildWindow(changeTypeMenuItem);
	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("Editor/ResourceWindow/Popup/OpenScene", TR("open_scene"), TR("open_scene_hint"), PI_OPEN_SCENE));

	mResourceTypesPopupMenu = gPopupMgr->CreatePopupMenu("Editor/ResourceWindow/Popup/ChangeType/Popup");
	for (size_t resType = 0; resType < ResourceSystem::NUM_RESTYPES; ++resType)
	{
		string resName = ResourceSystem::GetResourceTypeName((ResourceSystem::eResourceType)resType);
		CEGUI::Window* menuItem = gPopupMgr->CreateMenuItem("Editor/ResourceWindow/Popup/ChangeType/Resource" + Utils::StringConverter::ToString(resType), resName, "", resType);
		mResourceTypesPopupMenu->addChildWindow(menuItem);
	}
	changeTypeMenuItem->addChildWindow(mResourceTypesPopupMenu);
}

void ResourceWindow::DestroyPopupMenu()
{
	gPopupMgr->DestroyPopupMenu(mPopupMenu);
	mPopupMenu = 0;
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
		for (size_t idx = 0; idx < mResourceTypesPopupMenu->getChildCount(); ++idx)
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
		ResourceSystem::eResourceType newType = (ResourceSystem::eResourceType)menuItem->getID();
		gEditorMgr.ChangeResourceType(mCurrentPopupResource, newType);
		Update();
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

bool Editor::ResourceWindow::OnTreeItemClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	CEGUI::ItemEntry* itemEntry = static_cast<CEGUI::ItemEntry*>(args.window->getParent());
	itemEntry->setSelected(true);

	if (args.button == CEGUI::RightButton)
	{
		OpenPopupMenu(ItemEntryToResourcePtr(dragContainer), args.position.d_x, args.position.d_y);
		return true;
	}

	return true;
}

bool Editor::ResourceWindow::OnTreeItemDoubleClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	if (args.button == CEGUI::LeftButton)
	{
		ResourceSystem::ResourcePtr resource = ItemEntryToResourcePtr(dragContainer);
		if (resource.get() && gEditorMgr.GetCurrentProject()->IsResourceScene(resource))
		{
			gEditorMgr.GetCurrentProject()->OpenScene(resource);
		}
		else
		{
			Core::Application::GetSingleton().OpenFileInExternalApp(resource->GetFilePath());
		}
		return true;
	}

	return true;
}

CEGUI::Window* Editor::ResourceWindow::CreateResourceItemEntry()
{
	static uint32 resourceItemCounter = 0;
	const CEGUI::String& name = "Editor/ResourceWindow/Tree/ResourceItem" + StringConverter::ToString(resourceItemCounter++);
	CEGUI::ItemEntry* newItem = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("Editor/ListboxItem", name));
	newItem->setUserData(this);
	CEGUI::Window* dragContainer = gGUIMgr.CreateWindow("DragContainer", name + "/DC");
	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	newItem->addChildWindow(dragContainer);

	CEGUI::Window* newItemText = gGUIMgr.CreateWindow("Editor/StaticText", name + "/DC/Text");
	newItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 16), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	newItemText->setProperty("FrameEnabled", "False");
	newItemText->setProperty("BackgroundEnabled", "False");
	newItemText->setMousePassThroughEnabled(true);

	CEGUI::Window* newItemIcon = gGUIMgr.CreateWindow("Editor/StaticImage", name + "/DC/Icon");
	newItemIcon->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0.5, -6), CEGUI::UDim(0, 12), CEGUI::UDim(0.5, 6)));
	newItemIcon->setProperty("FrameEnabled", "False");
	newItemIcon->setProperty("BackgroundEnabled", "False");
	newItemIcon->setTooltipText("Directory");
	newItemIcon->setMousePassThroughEnabled(true);

	dragContainer->addChildWindow(newItemText);
	dragContainer->addChildWindow(newItemIcon);
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnTreeItemClicked, this));
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnTreeItemDoubleClicked, this));
	dragContainer->setUserString("DragDataType", "Resource");
	dragContainer->setUserData(this);
	return newItem;
}

CEGUI::Window* ResourceWindow::CreateDirectoryItemEntry()
{
	static uint32 dirCounter = 0;
	const CEGUI::String& windowName = "Editor/ResourceWindow/Tree/DirItem" + StringConverter::ToString(dirCounter++);
	CEGUI::Window* itemEntry = gGUIMgr.CreateWindow("Editor/ListboxItem", windowName);
	itemEntry->setID(InvalidResourceIndex);
	CEGUI::Window* dragContainer = gGUIMgr.CreateWindow("DragContainer", windowName + "/DC");
	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	itemEntry->addChildWindow(dragContainer);

	CEGUI::Window* itemEntryIcon = gGUIMgr.CreateWindow("Editor/StaticImage", windowName + "/DC/Icon");
	itemEntryIcon->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0.5, -6), CEGUI::UDim(0, 12), CEGUI::UDim(0.5, 6)));
	itemEntryIcon->setProperty("FrameEnabled", "False");
	itemEntryIcon->setProperty("BackgroundEnabled", "False");
	itemEntryIcon->setTooltipText("Directory");
	itemEntryIcon->setProperty("Image", "set:ResourceWindowIcons image:Directory");
	itemEntryIcon->setMousePassThroughEnabled(false);
	dragContainer->addChildWindow(itemEntryIcon); 

	CEGUI::Window* itemEntryText = gGUIMgr.CreateWindow("Editor/ListboxItem", windowName + "/DC/Text");	
	itemEntryText->setArea(CEGUI::URect(CEGUI::UDim(0, 16), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	itemEntryText->setMousePassThroughEnabled(false);
	dragContainer->addChildWindow(itemEntryText);
	return itemEntry;
}

void ResourceWindow::SetupResourceItemEntry(CEGUI::Window* itemEntry, uint32 resourceIndex)
{
	OC_DASSERT(resourceIndex < mResourcePool.size());
	ResourceSystem::ResourcePtr res = mResourcePool[resourceIndex];
	OC_DASSERT(res.get() != 0);

	uint32 indentLevel = GetPathIndentLevel(res->GetRelativeFileDir());

	itemEntry->setID(resourceIndex);
	itemEntry->setUserString("ResourceDir", res->GetRelativeFileDir());
	itemEntry->setUserString("ResourcePath", res->GetRelativeFilePath());

	CEGUI::Window* dragContainer = itemEntry->getChildAtIdx(0);
	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, (float32)(indentLevel * 16)), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	dragContainer->setID(resourceIndex);

	CEGUI::Window* itemEntryText = dragContainer->getChildAtIdx(0);
	itemEntryText->setText(boost::filesystem::path(res->GetName()).filename());
}

void ResourceWindow::SetupDirectoryItemEntry(CEGUI::Window* itemEntry, const string& parentPath, const string& directory)
{
	CEGUI::Window* dragContainer = itemEntry->getChildAtIdx(0);
	CEGUI::Window* itemEntryText = dragContainer->getChildAtIdx(1);
	uint32 indentLevel = GetPathIndentLevel(parentPath);

	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, (float32)(indentLevel * 16)), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	itemEntryText->setText(directory);
	itemEntry->setUserString("ResourceDir", parentPath);
	itemEntry->setUserString("ResourcePath", parentPath + directory);
}

bool ResourceWindow::UpdateItemEntry(CEGUI::Window* itemEntry)
{
	uint32 resourceIndex = itemEntry->getID();

	// Directory ItemEntry does not need updates
	if (resourceIndex == InvalidResourceIndex) return true;

	OC_DASSERT(resourceIndex < mResourcePool.size());
	ResourceSystem::ResourcePtr res = mResourcePool[resourceIndex];
	if (res.get() == 0) return false; // item to be deleted

	CEGUI::Window* itemEntryIcon = itemEntry->getChildAtIdx(0)->getChildAtIdx(1);
	itemEntryIcon->setTooltipText(ResourceSystem::GetResourceTypeName(res->GetType()));
	itemEntryIcon->setProperty("Image", "set:ResourceWindowIcons image:Resource" + ResourceSystem::GetResourceTypeName(res->GetType()));
	return true;
}

void Editor::ResourceWindow::StoreItemEntry(CEGUI::Window* itemEntry)
{
	CEGUI::Window* parent = itemEntry->getParent();
	if (parent)
		parent->removeChildWindow(itemEntry);

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
		gGUIMgr.DestroyWindow((*it));
	}
	mResourceItemEntryCache.clear();

	for (ItemEntryCache::iterator it = mDirectoryItemEntryCache.begin(); it != mDirectoryItemEntryCache.end(); ++it)
	{
		gGUIMgr.DestroyWindow((*it));
	}
	mDirectoryItemEntryCache.clear();
}

uint32 ResourceWindow::GetPathIndentLevel(const string& path)
{
	uint32 indentLevel = 0;
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
	static const CEGUI::utf32 delim = (CEGUI::utf32)('/');

	if (first == second) return false;

	const CEGUI::String& firstResourcePath = first->getUserString("ResourcePath");
	const CEGUI::String& secondResourcePath = second->getUserString("ResourcePath");

	size_t firstItemIdxStart = 0;
	size_t firstItemIdxEnd = firstResourcePath.find(delim);
	size_t secondItemIdxStart = 0;
	size_t secondItemIdxEnd = secondResourcePath.find(delim);

	bool firstItemIsDirectory, secondItemIsDirectory;

	do
	{
		firstItemIsDirectory = (firstItemIdxEnd != CEGUI::String::npos);
		secondItemIsDirectory = (secondItemIdxEnd != CEGUI::String::npos);
		
		if (!firstItemIsDirectory && first->getID()== InvalidResourceIndex)
			firstItemIsDirectory = true;

		if (!secondItemIsDirectory && second->getID()== InvalidResourceIndex)
			secondItemIsDirectory = true;

		if (firstItemIsDirectory && !secondItemIsDirectory)
			return true;

		if (!firstItemIsDirectory && secondItemIsDirectory)
			return false;

		int cmp = firstResourcePath.compare(firstItemIdxStart, firstItemIdxEnd - firstItemIdxStart, secondResourcePath, secondItemIdxStart, secondItemIdxEnd - secondItemIdxStart);

		if (cmp != 0)
		{
			return cmp < 0;
		}

		firstItemIdxStart = firstItemIdxEnd;
		if (firstItemIdxStart == CEGUI::String::npos)
			firstItemIdxStart = firstResourcePath.size();
		else
			firstItemIdxStart++;
		firstItemIdxEnd = firstResourcePath.find(delim, firstItemIdxStart);

		secondItemIdxStart = secondItemIdxEnd;
		if (secondItemIdxStart == CEGUI::String::npos)
			secondItemIdxStart = secondResourcePath.size();
		else
			secondItemIdxStart++;
		secondItemIdxEnd = secondResourcePath.find(delim, secondItemIdxStart);
	} while (firstItemIdxStart != firstResourcePath.size() || secondItemIdxStart != secondResourcePath.size());
	return false;
}
