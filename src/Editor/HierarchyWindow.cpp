#include "Common.h"
#include "HierarchyWindow.h"
#include "PopupMenu.h"
#include "PrototypeWindow.h"
#include "Core/Game.h"
#include "GUISystem/CEGUITools.h"
#include "GUISystem/ItemListboxProperties.h"
#include "GUISystem/PromptBox.h"
#include "Editor/EditorMgr.h"
#include "Editor/EditorGUI.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"
#include "ResourceSystem/XMLResource.h"

using namespace Editor;
using namespace EntitySystem;

const uint32 TREE_LEVEL_CHAR_SIZE = 4;

GUISystem::ItemListboxWantsMouseWheel gHierarchyMouseWheelProperty;

Editor::HierarchyWindow::HierarchyWindow(): mWindow(0), mTree(0), mCurrentParent(EntitySystem::EntityHandle::Null), mDontAddEntities(false)
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

	mWindow = gGUIMgr.LoadSystemLayout("HierarchyWindow.layout", "EditorRoot/HierarchyWindow");
	OC_ASSERT(mWindow != 0);
	gGUIMgr.GetGUISheet()->addChildWindow(mWindow);
	mTree = static_cast<CEGUI::ItemListbox*>(mWindow->getChild(mWindow->getName() + "/List"));
	OC_ASSERT(mTree != 0);

	mTree->addProperty(&gHierarchyMouseWheelProperty);

	mTree->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::HierarchyWindow::OnWindowMouseButtonUp, this));
	mTree->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&Editor::HierarchyWindow::OnTreeDragDropItemDropped, this));

	CEGUI_EXCEPTION_END
}

void Editor::HierarchyWindow::RebuildTree()
{
	if (!CheckHierarchy()) return;

	for (int32 i=mTree->getItemCount()-1; i>=0; --i)
	{
		gGUIMgr.DestroyWindow(mTree->getItemFromIndex(i));
	}
	OC_ASSERT(mTree->getItemCount() == 0);
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

	CEGUI::ItemEntry* newItem = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("Editor/ListboxItem"));
	newItem->setID(itemID);

	CEGUI::Window* dragContainer = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("DragContainer"));
	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	newItem->addChildWindow(dragContainer);

	CEGUI::Window* newItemText = gGUIMgr.CreateWindow("Editor/StaticText");
	newItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	newItemText->setText(string(depth*TREE_LEVEL_CHAR_SIZE, ' ') + textName);
	newItemText->setProperty("FrameEnabled", "False");
	newItemText->setProperty("BackgroundEnabled", "False");
	newItemText->setMousePassThroughEnabled(true);

	dragContainer->addChildWindow(newItemText);
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Editor::HierarchyWindow::OnDragContainerMouseButtonDown, this));
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::HierarchyWindow::OnDragContainerMouseButtonUp, this));
	dragContainer->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&Editor::HierarchyWindow::OnTreeItemDragDropItemDropped, this));
	dragContainer->setID(itemID);
	dragContainer->setUserString("DragDataType", "Hierarchy");
	dragContainer->setUserData(this);


	OC_ASSERT(index <= mTree->getItemCount());
	if (index == mTree->getItemCount())
	{
		mTree->addItem(newItem); // this is the most time consuming part of the function!!!
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
	gGUIMgr.DestroyWindow(mTree->getItemFromIndex(index));
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

void Editor::HierarchyWindow::AddEntityToHierarchy( const EntitySystem::EntityHandle toAdd, const EntitySystem::EntityHandle parent )
{
	if (mDontAddEntities) return; // the entity will be added to the hierarchy later on

	if (gEntityMgr.IsEntityTransient(toAdd) || gEntityMgr.IsEntityPrototype(toAdd)
	  || toAdd.GetName() == Core::Game::GameCameraName || toAdd.GetName() == Editor::EditorGUI::EditorCameraName) return;

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
	if (parentIndex == -1) AddTreeItem(0, toAdd.GetName(), 0, toAdd);
	else AddTreeItem(parentIndex+1, toAdd.GetName(), parentDepth+1, toAdd);
}

void Editor::HierarchyWindow::AddEntityToHierarchy( const EntitySystem::EntityHandle toAdd )
{
	AddEntityToHierarchy(toAdd, mCurrentParent);
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
	ocInfo << "Loading hierarchy";

	mHierarchy.clear();

	LoadSubtree(xml, mHierarchy.begin());


	set<EntitySystem::EntityHandle> loadedEntities;
	for (HierarchyTree::iterator it=mHierarchy.begin(); it!=mHierarchy.end(); ++it)
	{
		loadedEntities.insert(*it);
	}

	EntitySystem::EntityList entities;
	gEntityMgr.GetEntities(entities);
	bool reportWarning = true;
	for (EntitySystem::EntityList::iterator it=entities.begin(); it!=entities.end(); ++it)
	{
		if (loadedEntities.find(*it) != loadedEntities.end()) continue;
		if (gEntityMgr.IsEntityTransient(*it)) continue;
		if (it->GetName() == Core::Game::GameCameraName || it->GetName() == Editor::EditorGUI::EditorCameraName) continue;

		if (reportWarning)
		{
			ocWarning << "Not all entities existed in the hierarchy. Loading the rest automatically.";
			reportWarning = false;
		}
		mHierarchy.append_child(mHierarchy.begin(), *it);
	}

	RebuildTree();

	ocInfo << "Hierarchy loaded";
}

void Editor::HierarchyWindow::LoadSubtree( ResourceSystem::XMLNodeIterator& xml, const HierarchyTree::iterator_base& parent )
{
	for (ResourceSystem::XMLNodeIterator iter=xml.IterateChildren(); iter!=xml.EndChildren(); ++iter)
	{
		if ((*iter).compare("Entity") != 0) continue;
		if (!iter.HasAttribute("ID"))
		{
			ocError << "Missing attribute 'ID' in the hierarchy tree description";	
			continue;
		}
		HierarchyTree::iterator childIter = mHierarchy.append_child(parent, EntitySystem::EntityHandle(iter.GetAttribute<EntitySystem::EntityID>("ID")));
		LoadSubtree(iter, childIter);
	}
}

void Editor::HierarchyWindow::SaveHierarchy( ResourceSystem::XMLOutput& storage )
{
	storage.BeginElement("Hierarchy");
	SaveSubtree(storage, mHierarchy.begin());
	storage.EndElement();
}

void Editor::HierarchyWindow::SaveSubtree( ResourceSystem::XMLOutput& storage, const HierarchyTree::iterator_base& parent )
{
	for (HierarchyTree::sibling_iterator iter=mHierarchy.begin(parent); iter!=mHierarchy.end(parent); ++iter)
	{
		storage.BeginElementStart("Entity");
		storage.AddAttribute("ID", StringConverter::ToString(iter->GetID()));
		storage.BeginElementFinish();
		SaveSubtree(storage, iter);
		storage.EndElement();
	}
}

bool Editor::HierarchyWindow::OnDragContainerMouseButtonDown( const CEGUI::EventArgs& e )
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

	return true;
}

bool Editor::HierarchyWindow::OnDragContainerMouseButtonUp( const CEGUI::EventArgs& e )
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	CEGUI::ItemEntry* itemEntry = static_cast<CEGUI::ItemEntry*>(args.window->getParent());
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

	if (mTree->getSelectedCount() > 0)
	{
		gEditorMgr.SetCurrentEntity(EntitySystem::EntityHandle::Null);
		gEditorMgr.ClearSelection();
	}

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

EntitySystem::EntityHandle Editor::HierarchyWindow::InstantiatePrototype(const EntitySystem::EntityHandle prototype, const EntitySystem::EntityHandle parent)
{
	// instantiate
	mCurrentParent = parent;
	EntitySystem::EntityHandle newEntity = gEntityMgr.InstantiatePrototype(prototype);
	if (!newEntity.IsValid()) return newEntity;

	// move the new entity to the middle of the editor window
	EntitySystem::EntityHandle editorCamera = gEntityMgr.FindFirstEntity(EditorGUI::EditorCameraName);
	if (editorCamera.IsValid() && newEntity.HasProperty("Position"))
	{
		newEntity.GetProperty("Position").SetValue(editorCamera.GetProperty("Position").GetValue<Vector2>());
	}
	
	// move the new entity to the active layer
	if (newEntity.HasProperty("Layer"))
	{
	  newEntity.GetProperty("Layer").SetValue(gLayerMgr.GetActiveLayer());
	}

	gEditorMgr.SetCurrentEntity(newEntity);

	return newEntity;
}

bool Editor::HierarchyWindow::OnTreeItemDragDropItemDropped(const CEGUI::EventArgs& e)
{
	const CEGUI::DragDropEventArgs& args = static_cast<const CEGUI::DragDropEventArgs&>(e);
	
	EntitySystem::EntityHandle targetEntity = mItems[args.window->getID()].entity;
	
	if (args.dragDropItem->getUserString("DragDataType") == "Hierarchy")
	{
	  EntitySystem::EntityHandle sourceEntity = mItems[args.dragDropItem->getID()].entity;

	  if (sourceEntity != targetEntity)
	  {
		  HierarchyTree::sibling_iterator sourceIter = std::find(mHierarchy.begin(), mHierarchy.end(), sourceEntity);
		  HierarchyTree::sibling_iterator targetIter = std::find(mHierarchy.begin(), mHierarchy.end(), targetEntity);
		  OC_ASSERT_MSG(sourceIter != mHierarchy.end() && targetIter != mHierarchy.end(), "Invalid drag'n'drop entities");

		  if (mHierarchy.parent(sourceIter) != targetIter && !mHierarchy.is_in_subtree(targetIter, sourceIter))
		  {
			  // the sibling iterator is here because reparent works with sibling iterators
			  HierarchyTree::sibling_iterator sourceIter2 = sourceIter;
			  ++sourceIter2;
			  mHierarchy.reparent(targetIter, sourceIter, sourceIter2);
			  RebuildTree();
			  SetSelectedEntity(sourceEntity);
		  }
	  }
	} 
	else if (args.dragDropItem->getUserString("DragDataType") == "Prototype")
	{
		PrototypeWindow* prototypeWindow = static_cast<PrototypeWindow*>(args.dragDropItem->getUserData());
		if (prototypeWindow == 0) return true;
		EntitySystem::EntityHandle sourcePrototype = prototypeWindow->GetItemAtIndex(args.dragDropItem->getID());

		InstantiatePrototype(sourcePrototype, targetEntity);
	} 
	else
	{
		return false;
	}
	
	return true;
}

bool Editor::HierarchyWindow::OnTreeDragDropItemDropped(const CEGUI::EventArgs& e)
{
	const CEGUI::DragDropEventArgs& args = static_cast<const CEGUI::DragDropEventArgs&>(e);

	if (args.dragDropItem->getUserString("DragDataType") == "Hierarchy")
	{
		EntitySystem::EntityHandle sourceEntity = mItems[args.dragDropItem->getID()].entity;

		HierarchyTree::sibling_iterator sourceIter = std::find(mHierarchy.begin(), mHierarchy.end(), sourceEntity);
		HierarchyTree::iterator targetIter = mHierarchy.begin();
		OC_ASSERT_MSG(sourceIter != mHierarchy.end() && targetIter != mHierarchy.end(), "Invalid drag'n'drop entities");

		if (mHierarchy.parent(sourceIter) != targetIter)
		{
			HierarchyTree::sibling_iterator sourceIter2 = sourceIter;
			++sourceIter2;
			mHierarchy.reparent(targetIter, sourceIter, sourceIter2);
			RebuildTree();
			SetSelectedEntity(sourceEntity);
		}
	} 
	else if (args.dragDropItem->getUserString("DragDataType") == "Prototype")
	{
		PrototypeWindow* prototypeWindow = static_cast<PrototypeWindow*>(args.dragDropItem->getUserData());
		if (prototypeWindow == 0) { return true; }
		EntitySystem::EntityHandle sourcePrototype = prototypeWindow->GetItemAtIndex(args.dragDropItem->getID());

		InstantiatePrototype(sourcePrototype, EntitySystem::EntityHandle::Null);
	} 
	else 
	{ 
		return false; 
	}

	return true;
}

void Editor::HierarchyWindow::SetSelectedEntity( const EntitySystem::EntityHandle entity )
{
	uint32 depth;
	int32 index = FindTreeItem(entity, depth);
	mTree->clearAllSelections();
	if (index != -1)
	{
		mTree->selectRange(index, index);
		gEditorMgr.SelectEntity(entity);
	}
}

EntitySystem::EntityHandle Editor::HierarchyWindow::GetParent( const EntitySystem::EntityHandle entity ) const
{
	HierarchyTree::iterator entityIter = std::find(mHierarchy.begin(), mHierarchy.end(), entity);
	if (entityIter == mHierarchy.end())
	{
		ocError << entity << " is not in the hierarchy";
		return EntitySystem::EntityHandle::Null;
	}
	HierarchyTree::iterator parentIter = mHierarchy.parent(entityIter);
	return *parentIter;
}

void Editor::HierarchyWindow::RefreshEntity( const EntitySystem::EntityHandle entity )
{
	uint32 depth;
	int32 index = FindTreeItem(entity, depth);
	if (index == -1) return;

	CEGUI::ItemEntry* item = mTree->getItemFromIndex(index);
	OC_ASSERT(item);
	CEGUI::Window* itemText = item->getChildAtIdx(0)->getChildAtIdx(0);
	OC_ASSERT(itemText);
	itemText->setText(string(depth*TREE_LEVEL_CHAR_SIZE, ' ') + entity.GetName());
}

void Editor::HierarchyWindow::MoveUp()
{
  EntitySystem::EntityHandle entity = gEditorMgr.GetCurrentEntity();
  if (!entity.Exists()) return;
  
  HierarchyTree::iterator entityIter = std::find(mHierarchy.begin(), mHierarchy.end(), entity);
  if (entityIter == mHierarchy.end()) return;
  
  HierarchyTree::iterator sibling = mHierarchy.previous_sibling(entityIter);
  if (sibling == mHierarchy.end()) return;
  
  mHierarchy.move_before(sibling, entityIter);
  RebuildTree();
	SetSelectedEntity(entity);
}

void Editor::HierarchyWindow::MoveDown()
{
  EntitySystem::EntityHandle entity = gEditorMgr.GetCurrentEntity();
  if (!entity.Exists()) return;
  
  HierarchyTree::iterator entityIter = std::find(mHierarchy.begin(), mHierarchy.end(), entity);
  if (entityIter == mHierarchy.end()) return;
  
  HierarchyTree::iterator sibling = mHierarchy.next_sibling(entityIter);
  if (sibling == mHierarchy.end()) return;
  
  mHierarchy.move_after(sibling, entityIter);
  RebuildTree();
	SetSelectedEntity(entity);
}

void Editor::HierarchyWindow::Clear()
{
	ocInfo << "Clearing HierarchyWindow";

	mHierarchy.clear();
	for (int32 i=mTree->getItemCount()-1; i>=0; --i)
	{
		gGUIMgr.DestroyWindow(mTree->getItemFromIndex(i));
	}
	OC_ASSERT(mTree->getItemCount() == 0);
	mItems.clear();
}

void Editor::HierarchyWindow::CreateEntity(EntitySystem::EntityHandle parent)
{
	GUISystem::PromptBox* prompt = new GUISystem::PromptBox(parent != EntitySystem::EntityHandle::Null ? parent.GetID() : 0);
	prompt->SetText(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "new_entity_prompt"));
	prompt->RegisterCallback(new GUISystem::PromptBox::Callback<Editor::HierarchyWindow>(this, &HierarchyWindow::NewEntityPromptCallback));
	prompt->Show();
}

void Editor::HierarchyWindow::NewEntityPromptCallback(bool clickedOK, string text, int32 tag)
{
	if (!clickedOK) return;
	SetCurrentParent(tag != 0 ? EntitySystem::EntityHandle(tag) : EntitySystem::EntityHandle::Null);
	gEditorMgr.CreateEntity(text);
	SetCurrentParent(EntitySystem::EntityHandle::Null);
}
