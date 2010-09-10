#include "Common.h"
#include "Editor/HierarchyWindow.h"
#include "Editor/EditorMgr.h"
#include "Editor/EditorGUI.h"
#include "Core/Game.h"
#include "GUISystem/CEGUICommon.h"
#include "GUISystem/PromptBox.h"
#include "GUISystem/PopupMgr.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"
#include "ResourceSystem/XMLResource.h"

using namespace Editor;
using namespace EntitySystem;

Editor::HierarchyWindow::HierarchyWindow(): mWindow(0), mTree(0), mCurrentParent(EntitySystem::EntityHandle::Null), mDontAddEntities(false), mIsRemovingChildEntities(false)
{
}

Editor::HierarchyWindow::~HierarchyWindow()
{
	if (mWindow || mTree || mPopupMenu)
	{
		ocError << "HierarchyWindow was destroyed without deinitialization.";
	}
}

void Editor::HierarchyWindow::Init()
{
	ClearHierarchy();
	CEGUI_TRY;
	{
		mWindow = gGUIMgr.GetWindow("Editor/HierarchyWindow");
		OC_ASSERT(mWindow);
		mTree = static_cast<CEGUI::ItemListbox*>(gGUIMgr.CreateWindowDirectly("Editor/ItemListbox", "Editor/HierarchyWindow/Tree"));
		mTree->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		mTree->setUserString("WantsMouseWheel", "True");

		mTree->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Editor::HierarchyWindow::OnTreeClicked, this));
		mTree->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&Editor::HierarchyWindow::OnItemDroppedOnTree, this));
		mWindow->addChildWindow(mTree);
	}
	CEGUI_CATCH;

	CreatePopupMenu();
}

void HierarchyWindow::Deinit()
{
	gGUIMgr.DestroyWindowDirectly(mWindow);
	mWindow = 0;
	mTree = 0;
	DestroyPopupMenu();
}

void Editor::HierarchyWindow::Update()
{
	if (!CheckHierarchy()) return;

	size_t desiredItemCount = GetHierarchySize();
	for (size_t i = mTree->getItemCount(); i < desiredItemCount; ++i)
	{
		CEGUI::ItemEntry* newItem = RestoreTreeItem();
		mTree->addItem(newItem);
	}

	for (size_t i = mTree->getItemCount(); i > desiredItemCount; --i)
	{
		StoreTreeItem(mTree->getItemFromIndex(i - 1));
	}

	OC_ASSERT(mTree->getItemCount() == desiredItemCount);

	size_t itemIndex = 0;
	SetupSubtree(mHierarchy.begin(), "", 0, itemIndex);
	OC_ASSERT(itemIndex == mTree->getItemCount());
}

void Editor::HierarchyWindow::SetupSubtree(const HierarchyTree::iterator_base& parentIter, const string& hierarchyPath, uint32 depth, size_t& itemIndex)
{
	for (HierarchyTree::sibling_iterator siblingIter=mHierarchy.begin(parentIter); siblingIter!=mHierarchy.end(parentIter); ++siblingIter)
	{
		string itemHierarchyPath = hierarchyPath + '/' + siblingIter->GetName();
		CEGUI::ItemEntry* treeItem = mTree->getItemFromIndex(itemIndex++);
		SetupTreeItem(treeItem, *siblingIter, depth, hierarchyPath);

		if (siblingIter.number_of_children() > 0)
		{
			SetupSubtree(siblingIter, itemHierarchyPath, depth + 1, itemIndex);
		}
	}
}

void Editor::HierarchyWindow::AddEntityToHierarchy(const EntitySystem::EntityHandle toAdd, const EntitySystem::EntityHandle parent)
{
	if (mDontAddEntities) return; // the entity will be added to the hierarchy later on

	if (gEntityMgr.IsEntityTransient(toAdd) || gEntityMgr.IsEntityPrototype(toAdd)
	  || toAdd.GetName() == Core::Game::GameCameraName || toAdd.GetName() == Editor::EditorGUI::EditorCameraName) return;

	HierarchyTree::iterator parentIter = mHierarchy.begin();
	if (parent.IsValid())
	{
		parentIter = Containers::find(mHierarchy.begin(), mHierarchy.end(), parent);
		if (parentIter == mHierarchy.end())
		{
			ocError << "Parent is not in the hierarchy tree";
			return;
		}
	}
	mHierarchy.append_child(parentIter, toAdd);
	Update();
}

void Editor::HierarchyWindow::AddEntityToHierarchy(const EntitySystem::EntityHandle toAdd)
{
	AddEntityToHierarchy(toAdd, mCurrentParent);
}

void Editor::HierarchyWindow::RemoveEntityFromHierarchy(const EntitySystem::EntityHandle toRemove)
{
	if (mIsRemovingChildEntities) return; // We are already inside RemoveEntityFromHierarchy few levels higher in the call stack

	if (gEntityMgr.IsEntityTransient(toRemove) || gEntityMgr.IsEntityPrototype(toRemove)) return;

	HierarchyTree::iterator iter = Containers::find(mHierarchy.begin(), mHierarchy.end(), toRemove);
	if (iter == mHierarchy.end())
	{
		ocWarning << "Attempting to remove non-existent hierarchy item";
		return;
	}

	mIsRemovingChildEntities = true;

	HierarchyTree::iterator subtreeIter = iter;
	++subtreeIter;
	HierarchyTree::iterator subtreeIterEnd = iter;
	subtreeIterEnd.skip_children();
	++subtreeIterEnd;
	
	while (subtreeIter != subtreeIterEnd)
	{
		ocInfo << "Destroying " << *subtreeIter << " because it is descendant of " << *iter;
		gEntityMgr.DestroyEntity(*subtreeIter);
		++subtreeIter;
	}
	mHierarchy.erase(iter);
	mIsRemovingChildEntities = false;

	Update();
}

void HierarchyWindow::ClearHierarchy()
{
	// clear the hierarchy
	mHierarchy.clear();

	// insert the top element
	mHierarchy.insert(mHierarchy.begin(), EntitySystem::EntityHandle::Null);
}

size_t HierarchyWindow::GetHierarchySize()
{
	// subtract the top element
	size_t hierarchySize = mHierarchy.size();
	if (hierarchySize != 0)
		--hierarchySize;
	return hierarchySize;
}

bool Editor::HierarchyWindow::CheckHierarchy()
{
	HierarchyTree::pre_order_iterator endIter = mHierarchy.end(mHierarchy.begin());
	for (HierarchyTree::iterator it = mHierarchy.begin(mHierarchy.begin()); it!=endIter; ++it)
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

void Editor::HierarchyWindow::LoadHierarchy( ResourceSystem::XMLNodeIterator& xml )
{
	ocInfo << "Loading hierarchy";

	ClearHierarchy();
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

	Update();

	ocInfo << "Hierarchy loaded";
}

void Editor::HierarchyWindow::LoadSubtree(ResourceSystem::XMLNodeIterator& xml, const HierarchyTree::iterator_base& parent)
{
	for (ResourceSystem::XMLNodeIterator iter=xml.IterateChildren(); iter!=xml.EndChildren(); ++iter)
	{
		if ((*iter).compare("Entity") != 0) continue;
		if (!iter.HasAttribute("ID"))
		{
			ocError << "Missing attribute 'ID' in the hierarchy tree description";	
			continue;
		}
		EntitySystem::EntityHandle entity = EntitySystem::EntityHandle(iter.GetAttribute<EntitySystem::EntityID>("ID"));
		HierarchyTree::iterator childIter = mHierarchy.append_child(parent, entity);
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

bool Editor::HierarchyWindow::OnTreeItemClicked( const CEGUI::EventArgs& e )
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	EntityHandle clickedEntity = EntityHandle(dragContainer->getID());
	gEditorMgr.SelectEntity(clickedEntity);

	if (args.button == CEGUI::RightButton)
	{
		OpenPopupMenu(clickedEntity, args.position.d_x, args.position.d_y);
	}
	return true;
}

bool Editor::HierarchyWindow::OnTreeClicked( const CEGUI::EventArgs& e )
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);

	gEditorMgr.SelectEntity(EntitySystem::EntityHandle::Null);
	gEditorMgr.ClearSelection();

	if (args.button == CEGUI::RightButton)
	{
		OpenPopupMenu(EntitySystem::EntityHandle::Null, args.position.d_x, args.position.d_y);
	}

	return true;
}

bool Editor::HierarchyWindow::OnItemDroppedOnTreeItem(const CEGUI::EventArgs& e)
{
	const CEGUI::DragDropEventArgs& args = static_cast<const CEGUI::DragDropEventArgs&>(e);
	
	EntitySystem::EntityHandle targetEntity = EntityHandle(args.window->getID());
	
	if (args.dragDropItem->getUserString("DragDataType") == "Hierarchy")
	{
		EntitySystem::EntityHandle sourceEntity = EntityHandle(args.dragDropItem->getID());
		
		if (sourceEntity == targetEntity)
			return true;
		
		HierarchyTree::iterator sourceIterBegin = Containers::find(mHierarchy.begin(), mHierarchy.end(), sourceEntity);
		HierarchyTree::iterator targetIter = Containers::find(mHierarchy.begin(), mHierarchy.end(), targetEntity);

		if (sourceIterBegin == mHierarchy.end() || targetIter == mHierarchy.end())
			return true;

		HierarchyTree::iterator sourceIterEnd = sourceIterBegin;
		sourceIterEnd.skip_children();
		++sourceIterEnd;

		// check whether item was dropped on its parent
		if (mHierarchy.parent(sourceIterBegin) == targetIter)
			return true;

		if (mHierarchy.is_in_subtree(targetIter, sourceIterBegin, sourceIterEnd))
		{
			// item was dragged to its subtree:
			HierarchyTree::sibling_iterator childIter = mHierarchy.begin(sourceIterBegin);
			while (childIter != mHierarchy.end(sourceIterBegin))
			{
				HierarchyTree::sibling_iterator iter = childIter;
				++childIter;
				mHierarchy.move_before(sourceIterBegin, iter);
			}
			sourceIterEnd = mHierarchy.next_sibling(sourceIterBegin);
			mHierarchy.reparent(targetIter, sourceIterBegin, sourceIterEnd);
		}
		else
		{
			sourceIterEnd = mHierarchy.next_sibling(sourceIterBegin);
			mHierarchy.reparent(targetIter, sourceIterBegin, sourceIterEnd);
		}
		Update();
	} 
	else if (args.dragDropItem->getUserString("DragDataType") == "Prototype")
	{
		EntitySystem::EntityHandle sourcePrototype = EntitySystem::EntityHandle(args.dragDropItem->getID());
		gEditorMgr.InstantiatePrototype(sourcePrototype, targetEntity);
	} 
	else
	{
		return false;
	}
	return true;
}

bool Editor::HierarchyWindow::OnItemDroppedOnTree(const CEGUI::EventArgs& e)
{
	const CEGUI::DragDropEventArgs& args = static_cast<const CEGUI::DragDropEventArgs&>(e);

	if (args.dragDropItem->getUserString("DragDataType") == "Hierarchy")
	{
		// Move the dragged entity before the first item in the hierarchy
		EntitySystem::EntityHandle sourceEntity = EntityHandle(args.dragDropItem->getID());
		if (!sourceEntity.IsValid())
			return true;

		HierarchyTree::iterator sourceIter = Containers::find(mHierarchy.begin(), mHierarchy.end(), sourceEntity);
		HierarchyTree::sibling_iterator lastTopElementIter = --mHierarchy.end(mHierarchy.begin());
		if (sourceIter == false || sourceIter == lastTopElementIter)
			return true;

		mHierarchy.move_after((HierarchyTree::iterator)lastTopElementIter, sourceIter);
		Update();
	} 
	else if (args.dragDropItem->getUserString("DragDataType") == "Prototype")
	{
		EntitySystem::EntityHandle sourcePrototype = EntitySystem::EntityHandle(args.dragDropItem->getID());
		gEditorMgr.InstantiatePrototype(sourcePrototype, EntitySystem::EntityHandle::Null);
	} 
	return true;
}

void Editor::HierarchyWindow::SetSelectedEntity(EntitySystem::EntityHandle entity)
{
	if (!entity.IsValid())
	{
		mTree->clearAllSelections();
		return;
	}

	for (size_t idx = 0; idx < mTree->getItemCount(); ++idx)
	{
		CEGUI::ItemEntry* treeItem = mTree->getItemFromIndex(idx);
		treeItem->setSelected((EntitySystem::EntityID)treeItem->getID() == entity.GetID());
	}
}

EntitySystem::EntityHandle Editor::HierarchyWindow::GetParent(EntityHandle entity) const
{
	HierarchyTree::iterator entityIter = Containers::find(mHierarchy.begin(), mHierarchy.end(), entity);
	if (entityIter == mHierarchy.end())
	{
		ocError << entity << " is not in the hierarchy";
		return EntitySystem::EntityHandle::Null;
	}
	HierarchyTree::iterator parentIter = mHierarchy.parent(entityIter);
	return *parentIter;
}

void Editor::HierarchyWindow::MoveUp(EntitySystem::EntityHandle entity)
{
	if (!entity.Exists()) return;
  
	HierarchyTree::iterator entityIter = Containers::find(mHierarchy.begin(), mHierarchy.end(), entity);
	if (entityIter == mHierarchy.end()) return;
  
	HierarchyTree::iterator sibling = mHierarchy.previous_sibling(entityIter);
	if (sibling == false) return;
  
	mHierarchy.move_before(sibling, entityIter);
	Update();
}

void Editor::HierarchyWindow::MoveDown(EntitySystem::EntityHandle entity)
{
	if (!entity.Exists()) return;
  
	HierarchyTree::iterator entityIter = Containers::find(mHierarchy.begin(), mHierarchy.end(), entity);
	if (entityIter == mHierarchy.end()) return;
  
	HierarchyTree::iterator sibling = mHierarchy.next_sibling(entityIter);
	if (sibling == false) return;
  
	mHierarchy.move_after(sibling, entityIter);
	Update();
}

void HierarchyWindow::ReparentUp(EntityHandle entity)
{
	if (!entity.Exists()) return;
  
	HierarchyTree::iterator entityIter = Containers::find(mHierarchy.begin(), mHierarchy.end(), entity);
	if (entityIter == mHierarchy.end()) return;
  
	HierarchyTree::iterator parent = mHierarchy.parent(entityIter);
	if (parent == mHierarchy.begin()) return;
  
	mHierarchy.move_before(parent, entityIter);
	Update();
}

void Editor::HierarchyWindow::Clear()
{
	ocInfo << "Clearing HierarchyWindow";

	ClearHierarchy();
	for (int32 i=mTree->getItemCount()-1; i>=0; --i)
	{
		StoreTreeItem(mTree->getItemFromIndex(i));
	}
	OC_ASSERT(mTree->getItemCount() == 0);
}

void HierarchyWindow::CreatePopupMenu()
{
	mPopupMenu = gPopupMgr->CreatePopupMenu("HierarchyWindow/Popup");
	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("HierarchyWindow/Popup/MoveUp", TR("hierarchy_moveup"), TR("hierarchy_moveup_hint"), PI_MOVE_UP));
	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("HierarchyWindow/Popup/MoveDown", TR("hierarchy_movedown"), TR("hierarchy_movedown_hint"), PI_MOVE_DOWN));
	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("HierarchyWindow/Popup/ReparentUp", TR("hierarchy_reparentup"), TR("hierarchy_reparentup_hint"), PI_REPARENT_UP));

	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("HierarchyWindow/Popup/AddEntity", TR("hierarchy_add"), TR("hierarchy_add_hint"), PI_ADD_ENTITY));
	CEGUI::Window* newComponentMenuItem = gPopupMgr->CreateMenuItem("HierarchyWindow/Popup/NewComponent", TR("new_component"), TR("new_component_hint"), PI_NEW_COMPONENT);
	mPopupMenu->addChildWindow(newComponentMenuItem);
	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("HierarchyWindow/Popup/DuplicateEntity", TR("hierarchy_duplicate"), TR("hierarchy_duplicate_hint"), PI_DUPLICATE_ENTITY));
	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("HierarchyWindow/Popup/DeleteEntity", TR("hierarchy_delete"), TR("hierarchy_delete_hint"), PI_DELETE_ENTITY));
	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("HierarchyWindow/Popup/CreatePrototype", TR("hierarchy_prototype"), TR("hierarchy_prototype_hint"), PI_CREATE_PROTOTYPE));

	mComponentPopupMenu = gPopupMgr->CreatePopupMenu("HierarchyWindow/Popup/NewComponent/AutoPopup");
	for (size_t i = 0; i < EntitySystem::NUM_COMPONENT_TYPES; ++i)
	{
		const string& componentName = EntitySystem::GetComponentTypeName((EntitySystem::eComponentType)i);
		const CEGUI::String& componentMenuItemName = "HierarchyWindow/Popup/NewComponent/Component" + Utils::StringConverter::ToString(i);
		mComponentPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem(componentMenuItemName, componentName, "", i));
	}
	newComponentMenuItem->addChildWindow(mComponentPopupMenu);
}

void HierarchyWindow::DestroyPopupMenu()
{
	gPopupMgr->DestroyPopupMenu(mPopupMenu);
}

void HierarchyWindow::OpenPopupMenu(EntityHandle clickedEntity, float32 mouseX, float32 mouseY)
{
	mCurrentPopupEntity = clickedEntity;
	if (mCurrentPopupEntity.IsValid())
	{
		// Enable all menu items
		for (size_t idx = 0; idx < mPopupMenu->getChildCount(); ++idx)
			mPopupMenu->getChildAtIdx(idx)->setEnabled(true);
	}
	else
	{
		// Disable menu items that require entity
		mPopupMenu->getChildAtIdx(PI_MOVE_UP)->setEnabled(false);
		mPopupMenu->getChildAtIdx(PI_MOVE_DOWN)->setEnabled(false);
		mPopupMenu->getChildAtIdx(PI_REPARENT_UP)->setEnabled(false);
		mPopupMenu->getChildAtIdx(PI_NEW_COMPONENT)->setEnabled(false);
		mPopupMenu->getChildAtIdx(PI_DUPLICATE_ENTITY)->setEnabled(false);
		mPopupMenu->getChildAtIdx(PI_DELETE_ENTITY)->setEnabled(false);
		mPopupMenu->getChildAtIdx(PI_CREATE_PROTOTYPE)->setEnabled(false);
	}
	gPopupMgr->ShowPopup(mPopupMenu, mouseX, mouseY, GUISystem::PopupMgr::Callback(this, &Editor::HierarchyWindow::OnPopupMenuItemClicked));
}


void HierarchyWindow::OnPopupMenuItemClicked(CEGUI::Window* menuItem)
{
	if (menuItem->getParent() == mComponentPopupMenu)
	{
		// New component menu item clicked
		gEditorMgr.AddComponentToSelectedEntity((EntitySystem::eComponentType)menuItem->getID());
	}
	else
	{
		switch (menuItem->getID())
		{
		case PI_MOVE_UP:	
			MoveUp(mCurrentPopupEntity);
			break;
		case PI_MOVE_DOWN:
			MoveDown(mCurrentPopupEntity);
			break;
		case PI_REPARENT_UP:
			ReparentUp(mCurrentPopupEntity);
			break;
		case PI_ADD_ENTITY:
			gEditorMgr.ShowCreateEntityPrompt(mCurrentPopupEntity);
			break;
		case PI_DUPLICATE_ENTITY:
			gEditorMgr.DuplicateEntity(mCurrentPopupEntity);
			break;
		case PI_DELETE_ENTITY:
			gEditorMgr.DeleteSelectedEntity();
			break;
		case PI_CREATE_PROTOTYPE:
			gEditorMgr.CreatePrototypeFromSelectedEntity();
			break;
		default:
			OC_NOT_REACHED();
		}
	}
}

CEGUI::ItemEntry* Editor::HierarchyWindow::CreateTreeItem()
{
	static uint32 itemCounter = 0;
	CEGUI::String windowName = "Editor/Hierarchy/Tree/EntityItem" + StringConverter::ToString(itemCounter++);

	CEGUI::ItemEntry* newItem = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindowDirectly("Editor/ListboxItem", windowName));
	CEGUI::Window* dragContainer = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindowDirectly("DragContainer", windowName + "/DC"));
	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	newItem->addChildWindow(dragContainer);

	CEGUI::Window* newItemText = gGUIMgr.CreateWindowDirectly("Editor/StaticText", windowName + "/DC/Text");
	newItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	newItemText->setProperty("FrameEnabled", "False");
	newItemText->setProperty("BackgroundEnabled", "False");
	newItemText->setMousePassThroughEnabled(true);

	dragContainer->addChildWindow(newItemText);
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Editor::HierarchyWindow::OnTreeItemClicked, this));
	dragContainer->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&Editor::HierarchyWindow::OnItemDroppedOnTreeItem, this));
	dragContainer->setUserString("DragDataType", "Hierarchy");

	return newItem;
}

void Editor::HierarchyWindow::SetupTreeItem(CEGUI::ItemEntry* treeItem, EntityHandle entity, uint32 hierarchyLevel, const string& hierarchyPath)
{
	static const uint32 paddingPerLevel = 16;

	treeItem->setID(entity.GetID());
	treeItem->setUserString("HierarchyPath", hierarchyPath);
	treeItem->setSelected(gEditorMgr.GetSelectedEntity() == entity);
	
	CEGUI::Window* dragContainer = treeItem->getChildAtIdx(0);
	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, (float32)(hierarchyLevel * paddingPerLevel)), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	dragContainer->setID(entity.GetID());

	CEGUI::Window* treeItemText = dragContainer->getChildAtIdx(0);
	treeItemText->setText(entity.GetName());
}

void Editor::HierarchyWindow::StoreTreeItem(CEGUI::ItemEntry* treeItem)
{
	CEGUI::Window* parent = treeItem->getParent();
	if (parent)
		parent->removeChildWindow(treeItem);

	mTreeItemCache.push_back(treeItem);
}

CEGUI::ItemEntry* Editor::HierarchyWindow::RestoreTreeItem()
{
	CEGUI::ItemEntry* treeItem = 0;
	if (mTreeItemCache.empty())
	{
		treeItem = CreateTreeItem();
	}
	else
	{
		treeItem = mTreeItemCache.back();
		mTreeItemCache.pop_back();
	}
	return treeItem;
}

void Editor::HierarchyWindow::DestroyTreeItemCache()
{
	for (TreeItemCache::iterator it = mTreeItemCache.begin(); it != mTreeItemCache.end(); ++it)
	{
		gGUIMgr.DestroyWindowDirectly(*it);
	}
	mTreeItemCache.clear();
}