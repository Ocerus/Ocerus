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
}

void Editor::HierarchyWindow::RebuildTree()
{
	if (!CheckHierarchy()) return;
	mTree->resetList();
	mItems.clear();
	BuildSubtree(mHierarchy.begin(), 0);
}

void Editor::HierarchyWindow::BuildSubtree( const HierarchyTree::iterator_base& parentIter, uint32 depth )
{
	for (HierarchyTree::sibling_iterator siblingIter=mHierarchy.begin(parentIter); siblingIter!=mHierarchy.end(parentIter); ++siblingIter)
	{
		AppendTreeItem(siblingIter->GetName(), depth, *siblingIter);
		if (siblingIter.number_of_children() > 0)
		{
			BuildSubtree(siblingIter, depth + 1);
		}
	}
}

CEGUI::ItemEntry* Editor::HierarchyWindow::AddTreeItem( uint32 index, const string& textName, uint32 depth, const EntitySystem::EntityHandle data )
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
	newItemText->setText(string(depth*4, ' ') + textName);
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
	HierarchyInfo info;
	info.depth = depth;
	info.entity = data;
	mItems[itemID] = info;
	OC_ASSERT(mTree->getItemCount() == mItems.size());

	return newItem;
}

CEGUI::ItemEntry* Editor::HierarchyWindow::AppendTreeItem(const string& text, uint32 depth, const EntitySystem::EntityHandle data)
{
	return AddTreeItem(mTree->getItemCount(), text, depth, data);
}

void Editor::HierarchyWindow::RemoveTreeItem( uint32 index )
{
	OC_ASSERT(index <= mTree->getItemCount());
	mItems.erase(mTree->getItemFromIndex(index)->getID());
	mTree->removeItem(mTree->getItemFromIndex(index));
	OC_ASSERT(mTree->getItemCount() == mItems.size());
}

int32 Editor::HierarchyWindow::FindTreeItem( const EntitySystem::EntityHandle data, uint32& depth )
{
	for (size_t i=0; i<mTree->getItemCount(); ++i)
	{
		CEGUI::ItemEntry* item = mTree->getItemFromIndex(i);
		if (mItems[item->getID()].entity == data)
		{
			depth = mItems[item->getID()].depth;
			return (uint32)i;
		}
	}

	return -1;
}

void Editor::HierarchyWindow::AddEntityToHierarchy( const EntitySystem::EntityHandle parent, const EntitySystem::EntityHandle toAdd )
{
	if (gEntityMgr.IsEntityTransient(toAdd) || gEntityMgr.IsEntityPrototype(toAdd)) return;

	HierarchyTree::iterator parentIter = mHierarchy.begin();
	if (parent.IsValid()) parentIter = std::find(mHierarchy.begin(), mHierarchy.end(), parent);
	if (parentIter == mHierarchy.end())
	{
		ocError << "Parent is not in the hierarchy tree";
		return;
	}
	mHierarchy.append_child(parentIter, toAdd);
	uint32 parentDepth;
	int32 parentIndex = FindTreeItem(parent, parentDepth);
	if (parentIndex == -1) AppendTreeItem(toAdd.GetName(), 0, toAdd);
	else AddTreeItem(parentIndex, toAdd.GetName(), parentDepth+1, toAdd);
}

void Editor::HierarchyWindow::RemoveEntityFromHierarchy( const EntitySystem::EntityHandle toRemove )
{
	if (gEntityMgr.IsEntityTransient(toRemove) || gEntityMgr.IsEntityPrototype(toRemove)) return;

	uint32 depth;
	int32 index = FindTreeItem(toRemove, depth);
	if (index == -1)
	{
		ocWarning << "Attempting to remove non-existent tree item";
		return;
	}

	HierarchyTree::iterator iter = std::find(mHierarchy.begin(), mHierarchy.end(), toRemove);
	if (iter == mHierarchy.end())
	{
		ocWarning << "Attempting to remove non-existent hierarchy item";
		return;
	}

	// delete all child items in the tree
	uint32 childItemIndex = index + 1;
	while (childItemIndex < mTree->getItemCount() && mItems[mTree->getItemFromIndex(childItemIndex)->getID()].depth > depth)
	{
		EntitySystem::EntityHandle childEntity = mItems[mTree->getItemFromIndex(childItemIndex)->getID()].entity;
		gEntityMgr.DestroyEntity(childEntity);
	}

	mHierarchy.erase(iter);

	RemoveTreeItem(index);
}

void Editor::HierarchyWindow::LoadHierarchy( ResourceSystem::XMLNodeIterator& xml )
{
	mHierarchy.clear();

	EntitySystem::EntityList entities;
	gEntityMgr.GetEntities(entities);
	//uint32 depth = 0;
	HierarchyTree::iterator lastIter = mHierarchy.begin();
	for (EntitySystem::EntityList::iterator it=entities.begin(); it!=entities.end(); ++it)
	{
		if (gEntityMgr.IsEntityTransient(*it)) continue;

		lastIter = mHierarchy.append_child(lastIter, *it);
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
	EntitySystem::EntityHandle entity = entityIter->second.entity;
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

bool Editor::HierarchyWindow::CheckHierarchy()
{
	HierarchyTree::pre_order_iterator endIter = mHierarchy.end(mHierarchy.begin());
	for (HierarchyTree::pre_order_iterator it=mHierarchy.begin(mHierarchy.begin()); it!=endIter; ++it)
	{
		if (!it->IsValid())
		{
			ocError << "Invalid EntityHandle in the hierarchy tree";
			return false;
		}
		if (!it->Exists())
		{
			ocError << "Non-existent entity in the hierarchy tree";
			return false;
		}
	}

	return true;
}
