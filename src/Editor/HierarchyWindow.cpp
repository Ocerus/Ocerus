#include "Common.h"
#include "HierarchyWindow.h"
#include "PopupMenu.h"
#include "GUISystem/CEGUITools.h"
#include "Editor/EditorMgr.h"

using namespace Editor;
using namespace EntitySystem;

Editor::HierarchyWindow::HierarchyWindow(): mWindow(0), mTree(0)
{

}

Editor::HierarchyWindow::~HierarchyWindow()
{

}

void Editor::HierarchyWindow::Init()
{
	mSelectedIndex = -1;

	mHierarchy.clear();

	CEGUI_EXCEPTION_BEGIN

	mWindow = GUISystem::LoadWindowLayout("HierarchyWindow.layout", "EditorRoot/HierarchyWindow");
	OC_ASSERT(mWindow != 0);
	gGUIMgr.GetRootLayout()->addChildWindow(mWindow);
	mTree = static_cast<CEGUI::ItemListbox*>(mWindow->getChild(mWindow->getName() + "/List"));
	OC_ASSERT(mTree != 0);

	mTree->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::HierarchyWindow::OnWindowMouseButtonUp, this));

	CEGUI_EXCEPTION_END

	RebuildTree();
}

void Editor::HierarchyWindow::RebuildTree()
{
	mTree->resetList();
	mItems.clear();
	BuildSubtree(mHierarchy.begin(), 0);
}

void Editor::HierarchyWindow::BuildSubtree( const HierarchyTree::iterator_base& parentIter, uint32 depth )
{
	for (HierarchyTree::sibling_iterator siblingIter=mHierarchy.begin(parentIter); siblingIter!=mHierarchy.end(parentIter); ++siblingIter)
	{
		AppendTreeItem(string(depth*4, ' ') + siblingIter->GetName(), *siblingIter);
		if (siblingIter.number_of_children() > 0)
		{
			BuildSubtree(siblingIter, depth + 1);
		}
	}
}

CEGUI::ItemEntry* Editor::HierarchyWindow::AddTreeItem( uint32 index, const string& text, const EntitySystem::EntityHandle data )
{
	static unsigned int itemID = 0;
	++itemID;

	CEGUI::ItemEntry* newItem = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("Editor/ListboxItem", mTree->getName() + "/Item" + StringConverter::ToString(itemID)));
	newItem->setID(itemID);

	CEGUI::Window* dragContainer = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("DragContainer", mTree->getName() + "/ItemContainer" + StringConverter::ToString(itemID)));
	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	newItem->addChildWindow(dragContainer);

	CEGUI::Window* newItemText = gCEGUIWM.createWindow("Editor/StaticText", mTree->getName() + "/ItemText" + StringConverter::ToString(itemID));
	newItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	newItemText->setText(text);
	newItemText->setProperty("FrameEnabled", "False");
	newItemText->setProperty("BackgroundEnabled", "False");
	newItemText->setMousePassThroughEnabled(true);

	dragContainer->addChildWindow(newItemText);
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::HierarchyWindow::OnDragContainerMouseButtonUp, this));
	dragContainer->setID(itemID);
	dragContainer->setUserData(this);


	OC_ASSERT(index <= mTree->getItemCount());
	if (index == mTree->getItemCount())
	{
		mTree->addItem(newItem);
	}
	else
	{
		mTree->insertItem(newItem, mTree->getItemFromIndex(index));
	}
	mItems[itemID] = data;
	OC_ASSERT(mTree->getItemCount() == mItems.size());

	return newItem;
}

CEGUI::ItemEntry* Editor::HierarchyWindow::AppendTreeItem(const string& text, const EntitySystem::EntityHandle data)
{
	return AddTreeItem(mTree->getItemCount(), text, data);
}

void Editor::HierarchyWindow::RemoveTreeItem( uint32 index )
{
	OC_ASSERT(index <= mTree->getItemCount());
	mItems.erase(mTree->getItemFromIndex(index)->getID());
	mTree->removeItem(mTree->getItemFromIndex(index));
	OC_ASSERT(mTree->getItemCount() == mItems.size());
}

void Editor::HierarchyWindow::AddEntityToHierarchy( const EntitySystem::EntityHandle parent, const EntitySystem::EntityHandle toAdd )
{
	OC_FAIL("not implemented");
}

void Editor::HierarchyWindow::RemoveEntityFromHierarchy( const EntitySystem::EntityHandle toRemove )
{
	OC_FAIL("not implemented");
}

void Editor::HierarchyWindow::LoadHierarchy( ResourceSystem::XMLNodeIterator& xml )
{
	mHierarchy.clear();

	EntitySystem::EntityList entities;
	gEntityMgr.GetEntities(entities);
	//uint32 depth = 0;
	for (EntitySystem::EntityList::iterator it=entities.begin(); it!=entities.end(); ++it)
	{
		if (!gEntityMgr.IsEntityTransient(*it)) mHierarchy.append_child(mHierarchy.begin(), *it);
	}

	RebuildTree();

	ocInfo << "Hierarchy loaded";
}

bool Editor::HierarchyWindow::OnDragContainerMouseButtonUp( const CEGUI::EventArgs& e )
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	CEGUI::ItemEntry* itemEntry = static_cast<CEGUI::ItemEntry*>(args.window->getParent());
	itemEntry->setSelected(true);
	EntityMap::iterator entityIter = mItems.find(itemEntry->getID());
	OC_ASSERT(entityIter != mItems.end());
	EntitySystem::EntityHandle entity = entityIter->second;
	gEditorMgr.SetCurrentEntity(entity);

	if (args.button == CEGUI::RightButton)
	{
		if (entity.IsValid())
		{
			PopupMenu* menu = new PopupMenu("EditorRoot/Popup/EntityAboveItem");
			menu->Init<EntitySystem::EntityHandle>(entity);
			menu->Open(args.position.d_x, args.position.d_y);
			gEditorMgr.RegisterPopupMenu(menu);
		}
		return true;
	}

	return true;
}

bool Editor::HierarchyWindow::OnWindowMouseButtonUp( const CEGUI::EventArgs& e )
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);

	if (args.button == CEGUI::RightButton)
	{
		PopupMenu* menu = new PopupMenu("EditorRoot/Popup/Entity");
		menu->Init<EntitySystem::EntityHandle>(EntitySystem::EntityHandle::Null);
		menu->Open(args.position.d_x, args.position.d_y);
		gEditorMgr.RegisterPopupMenu(menu);
		return true;
	}

	return true;
}
