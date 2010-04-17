#include "Common.h"
#include "ResourceWindow.h"

#include "ResourceSystem/ResourceMgr.h"

#include "GUISystem/CEGUITools.h"

using namespace Editor;

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
	CEGUI_EXCEPTION_END
	OC_ASSERT(mWindow != 0);
	OC_ASSERT(mTree != 0);

	BuildResourceTree();
}

void Editor::ResourceWindow::BuildResourceTree()
{
	mTree->resetList();
	mResources.clear();
	gResourceMgr.GetResources(mResources);
	Containers::sort(mResources.begin(), mResources.end(), ResourceComparator);

	uint32 dirItemID = 0;
	vector<string> dirStack;
	for (vector<ResourceSystem::ResourcePtr>::const_iterator it = mResources.begin(); it != mResources.end(); ++it)
	{
		size_t resourceIndex = (size_t)(it - mResources.begin());
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
				dirItem->setID(string::npos);
				mTree->addChildWindow(dirItem);
			}

			++pathDepth;
			indexFrom = indexTo + 1;
		}
		while (indexTo != resourcePath.size());


		CEGUI::ItemEntry* newItem = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("Editor/ListboxItem", mTree->getName() + "/Resource" + StringConverter::ToString(resourceIndex)));
		//newItem->setText(string(pathDepth * 4, ' ') + (*it)->GetName());
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

		dragContainer->setWantsMultiClickEvents(false);

		mTree->addChildWindow(newItem);
	}
}

bool Editor::ResourceWindow::OnDragContainerMouseButtonUp(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	CEGUI::ItemEntry* itemEntry = static_cast<CEGUI::ItemEntry*>(args.window->getParent());
	itemEntry->setSelected(!itemEntry->isSelected());
	return true;
}
