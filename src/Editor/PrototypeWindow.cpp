#include "Common.h"
#include "PrototypeWindow.h"
#include "PopupMenu.h"
#include "EditorMgr.h"
#include "GUISystem/CEGUITools.h"

using namespace Editor;

Editor::PrototypeWindow::PrototypeWindow()
{
}

Editor::PrototypeWindow::~PrototypeWindow()
{
}

void Editor::PrototypeWindow::Init()
{
	mSelectedIndex = -1;

	CEGUI_EXCEPTION_BEGIN
	
	mWindow = gGUIMgr.LoadSystemLayout("PrototypeWindow.layout", "EditorRoot/PrototypeWindow");
	OC_ASSERT(mWindow != 0);
	gGUIMgr.GetGUISheet()->addChildWindow(mWindow);
	mTree = static_cast<CEGUI::ItemListbox*>(mWindow->getChild(mWindow->getName() + "/List"));
	OC_ASSERT(mTree != 0);
	
	mTree->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::PrototypeWindow::OnWindowMouseButtonUp, this));

	CEGUI_EXCEPTION_END

	RebuildTree();
}

void Editor::PrototypeWindow::RebuildTree()
{
	mTree->resetList();

	gEntityMgr.ProcessDestroyQueue();
	gEntityMgr.GetPrototypes(mItems);

	for (EntitySystem::EntityList::iterator it=mItems.begin(); it!=mItems.end(); ++it)
	{
		size_t itemIndex = (size_t)(it - mItems.begin());

		CEGUI::ItemEntry* newItem = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("Editor/ListboxItem", mTree->getName() + "/Item" + StringConverter::ToString(itemIndex)));
		newItem->setID(itemIndex);
		
		CEGUI::Window* dragContainer = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("DragContainer", mTree->getName() + "/ItemDc" + StringConverter::ToString(itemIndex)));
		dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		newItem->addChildWindow(dragContainer);

		CEGUI::Window* newItemText = gCEGUIWM.createWindow("Editor/StaticText", mTree->getName() + "/ItemText" + StringConverter::ToString(itemIndex));
		newItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		newItemText->setText(gEntityMgr.GetEntityName(*it));
		newItemText->setProperty("FrameEnabled", "False");
		newItemText->setProperty("BackgroundEnabled", "False");
		newItemText->setMousePassThroughEnabled(true);

		dragContainer->addChildWindow(newItemText);
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::PrototypeWindow::OnDragContainerMouseButtonUp, this));

		dragContainer->setID(itemIndex);
		dragContainer->setUserString("DragDataType", "Prototype");
		dragContainer->setUserData(this);

		mTree->addChildWindow(newItem);

		if (*it == gEditorMgr.GetCurrentEntity()) newItem->select();
	}
}

bool Editor::PrototypeWindow::OnDragContainerMouseButtonUp(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	CEGUI::ItemEntry* itemEntry = static_cast<CEGUI::ItemEntry*>(args.window->getParent());
	itemEntry->setSelected(true);
	mSelectedIndex = dragContainer->getID();
	
	EntitySystem::EntityHandle item = GetItemAtIndex(mSelectedIndex);
	if (item.IsValid())
	{
		gEditorMgr.SetCurrentEntity(item);
		gEditorMgr.ClearSelection();
	}

	if (args.button == CEGUI::RightButton)
	{
		if (item.IsValid())
		{
			PopupMenu* menu = new PopupMenu("EditorRoot/Popup/PrototypeAboveItem");
			menu->Init<EntitySystem::EntityHandle>(item);
			menu->Open(args.position.d_x, args.position.d_y);
			gEditorMgr.RegisterPopupMenu(menu);
		}
		return true;
	}

	return true;
}

bool Editor::PrototypeWindow::OnWindowMouseButtonUp(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);

	if (args.button == CEGUI::RightButton)
	{
		PopupMenu* menu = new PopupMenu("EditorRoot/Popup/Prototype");
		menu->Init<EntitySystem::EntityHandle>(EntitySystem::EntityHandle::Null);
		menu->Open(args.position.d_x, args.position.d_y);
		gEditorMgr.RegisterPopupMenu(menu);
		return true;
	}
	else if (args.button == CEGUI::LeftButton)
	{
		if (mSelectedIndex != -1)
		{
			gEditorMgr.SetCurrentEntity(EntitySystem::EntityHandle::Null);
		}
	}

	return true;
}

EntitySystem::EntityHandle Editor::PrototypeWindow::GetItemAtIndex( size_t index )
{
	if (index >= mItems.size()) return EntitySystem::EntityHandle::Null;
	return mItems.at(index);
}

EntitySystem::EntityHandle Editor::PrototypeWindow::GetSelectedItem()
{
	if (mTree->getSelectedCount() == 0) return EntitySystem::EntityHandle::Null;
	else return mItems.at(mSelectedIndex);
}

void Editor::PrototypeWindow::SetSelectedEntity( const EntitySystem::EntityHandle entity )
{
	int32 index = 0;
	for (EntitySystem::EntityList::iterator it=mItems.begin(); it!=mItems.end(); ++it, ++index)
	{
		if (*it == entity)
		{
			mSelectedIndex = index;
			mTree->selectRange(mSelectedIndex, mSelectedIndex);
			return;
		}
	}

	mSelectedIndex = -1;
	mTree->clearAllSelections();
}