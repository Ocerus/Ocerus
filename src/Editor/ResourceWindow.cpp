#include "Common.h"
#include "ResourceWindow.h"
#include "PopupMenu.h"
#include "EditorMgr.h"
#include "ResourceSystem/ResourceMgr.h"
#include "GUISystem/CEGUITools.h"

using namespace Editor;

/// Compares two resources according to their path and returns whether the first is greater than the second.
bool ResourceComparator(const ResourceSystem::ResourcePtr& r1, const ResourceSystem::ResourcePtr& r2)
{
	return (r1->GetFilePath().compare(r2->GetFilePath()) > 0);
}

Editor::ResourceWindow::ResourceWindow()
{

}

Editor::ResourceWindow::~ResourceWindow()
{

}

void Editor::ResourceWindow::Init()
{
	CEGUI_EXCEPTION_BEGIN
	mWindow = GUISystem::LoadWindowLayout("ResourceWindow.layout", "EditorRoot/ResourceWindow");
	gGUIMgr.GetRootLayout()->addChildWindow(mWindow);
	mTree = static_cast<CEGUI::ItemListbox*>(mWindow->getChild(mWindow->getName() + "/List"));
	CEGUI::Window* refreshButton = mWindow->getChildRecursive(mWindow->getName() + "/Toolbar/Refresh");
	refreshButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnRefreshButtonClicked, this));
	CEGUI_EXCEPTION_END
	OC_ASSERT(mWindow != 0);
	OC_ASSERT(mTree != 0);

	RebuildTree();
}

ResourceSystem::ResourcePtr ResourceWindow::GetItemAtIndex(size_t index)
{
	if (index >= mItems.size()) return ResourceSystem::ResourcePtr();
	return mItems.at(index);
}

void Editor::ResourceWindow::RebuildTree()
{
	mTree->resetList();
	mItems.clear();
	gResourceMgr.GetResources(mItems);
	Containers::sort(mItems.begin(), mItems.end(), ResourceComparator);

	uint32 dirItemID = 0;
	vector<string> dirStack;
	for (vector<ResourceSystem::ResourcePtr>::const_iterator it = mItems.begin(); it != mItems.end(); ++it)
	{
		size_t resourceIndex = (size_t)(it - mItems.begin());
		string resourcePath = (*it)->GetFileDir();

		uint32 pathDepth = 0;
		size_t indexFrom = 0;
		size_t indexTo = 0;
		do
		{
			indexTo = resourcePath.find('/', indexFrom);
			if (indexTo == string::npos)
				indexTo = resourcePath.size();
			
			string dirName = resourcePath.substr(indexFrom, indexTo - indexFrom);
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

				CEGUI::ItemEntry* dirItem = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("Editor/ListboxItem", mTree->getName() + "/DirItem" + StringConverter::ToString(dirItemID++)));
				dirItem->setText(string(pathDepth * 4, ' ') + dirName);
				mTree->addChildWindow(dirItem);
			}

			++pathDepth;
			indexFrom = indexTo + 1;
		}
		while (indexTo != resourcePath.size());


		CEGUI::ItemEntry* newItem = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("Editor/ListboxItem", mTree->getName() + "/Resource" + StringConverter::ToString(resourceIndex)));
		newItem->setID(resourceIndex);
		
		CEGUI::Window* dragContainer = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("DragContainer", mTree->getName() + "/DCResource" + StringConverter::ToString(resourceIndex)));
		dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		newItem->addChildWindow(dragContainer);

		CEGUI::Window* newItemText = gCEGUIWM.createWindow("Editor/StaticText", mTree->getName() + "/ResourceText" + StringConverter::ToString(resourceIndex));
		newItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		newItemText->setText(string(pathDepth * 4, ' ') + (*it)->GetName());
		newItemText->setProperty("FrameEnabled", "False");
		newItemText->setProperty("BackgroundEnabled", "False");
		newItemText->setMousePassThroughEnabled(true);

		dragContainer->addChildWindow(newItemText);
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnDragContainerMouseButtonUp, this));

		dragContainer->setID(resourceIndex);
		dragContainer->setUserData(this);

		mTree->addChildWindow(newItem);
	}
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
		ResourceSystem::ResourcePtr resource = GetItemAtIndex(dragContainer->getID());
		if (resource.get())
		{
			PopupMenu* menu = new PopupMenu("EditorRoot/Popup/Resource");
			menu->Init<ResourceSystem::ResourcePtr>(resource);
			menu->Open(args.position.d_x, args.position.d_y);
			gEditorMgr.RegisterPopupMenu(menu);
		}
		return true;
	}

	return true;
}

bool Editor::ResourceWindow::OnRefreshButtonClicked(const CEGUI::EventArgs& e)
{
	OC_UNUSED(e);
	RebuildTree();
	return true;
}
