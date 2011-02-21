#include "Common.h"
#include "Editor/LayerWindow.h"
#include "Editor/EditorMgr.h"
#include "GUISystem/CEGUICommon.h"
#include "GUISystem/PopupMgr.h"
#include "GUISystem/PromptBox.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"

using namespace Editor;

Editor::LayerWindow::LayerWindow():
		mWindow(0), mTree(0), mLayerPopupMenu(0), mEntityPopupMenu(0)
{
}

Editor::LayerWindow::~LayerWindow()
{
	if (mWindow || mLayerPopupMenu || mEntityPopupMenu)
	{
		ocError << "LayerWindow was destroyed without deinitialization.";		
	}
}

void Editor::LayerWindow::Init()
{
	OC_CEGUI_TRY;
	{
		mWindow = gGUIMgr.GetWindow("Editor/LayerWindow");
		OC_ASSERT(mWindow);
		mTree = static_cast<CEGUI::ItemListbox*>(gGUIMgr.CreateWindow("Editor/ItemListbox", "Editor/LayerWindow/List"));
		mTree->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		mTree->setSortMode(CEGUI::ItemListBase::UserSort);
		mTree->setSortCallback(&Editor::LayerWindow::SortCallback);
		mTree->setUserString("WantsMouseWheel", "True");
		mTree->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnTreeClicked, this));
		mWindow->addChildWindow(mTree);
	}
	OC_CEGUI_CATCH;

	OC_ASSERT(mWindow != 0);
	OC_ASSERT(mTree != 0);

	CreatePopupMenus();
}

void LayerWindow::Deinit()
{
	gGUIMgr.DestroyWindow(mWindow);
	mWindow = 0;
	DestroyPopupMenus();
	DestroyItemCache();
}

void LayerWindow::ShowNewLayerPrompt(EntitySystem::LayerID layerID)
{
	GUISystem::PromptBox* prompt = new GUISystem::PromptBox(layerID);
	prompt->SetLabel(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "layer_new_prompt"));
	prompt->RegisterCallback(GUISystem::PromptBox::Callback(this, &LayerWindow::NewLayerPromptCallback));
	prompt->Show();
}

void LayerWindow::CreateLayer(EntitySystem::LayerID layerID, const string& layerName)
{
	EntitySystem::LayerID newLayerID = gLayerMgr.AddTopLayer(layerName);
	gLayerMgr.MoveLayerBehind(newLayerID, layerID);
	Update();
}

void LayerWindow::MoveLayerUp(EntitySystem::LayerID layerID)
{
	gLayerMgr.MoveLayerUp(layerID);
	Update();
}

void LayerWindow::MoveLayerDown(EntitySystem::LayerID layerID)
{
	gLayerMgr.MoveLayerDown(layerID);
	Update();
}

void LayerWindow::ShowRenameLayerPrompt(EntitySystem::LayerID layerID)
{
	GUISystem::PromptBox* prompt = new GUISystem::PromptBox(layerID);
	prompt->SetLabel(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "layer_rename_prompt"));
	prompt->RegisterCallback(GUISystem::PromptBox::Callback(this, &LayerWindow::RenameLayerPromptCallback));
	prompt->Show();
}

void LayerWindow::RenameLayer(EntitySystem::LayerID layerID, const string& newLayerName)
{
	gLayerMgr.SetLayerName(layerID, newLayerName);
	Update();
}

void LayerWindow::RemoveLayer(EntitySystem::LayerID layerID)
{
	gLayerMgr.DeleteLayer(layerID, false);
	Update();
}

void LayerWindow::MoveEntityUp(EntitySystem::EntityHandle entity)
{
	gLayerMgr.MoveEntityUp(entity);
	Update();
}

void LayerWindow::MoveEntityDown(EntitySystem::EntityHandle entity)
{
	gLayerMgr.MoveEntityDown(entity);
	Update();
}

bool LayerWindow::OnItemClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	CEGUI::ItemEntry* item = static_cast<CEGUI::ItemEntry*>(args.window->getParent());
	if (item->getUserData())
	{
		// item is layer
		if (args.button == CEGUI::RightButton)
		{
			mCurrentPopupLayerID = item->getID();
			if (gLayerMgr.ExistsLayer(mCurrentPopupLayerID))
			{
				// Enable all menu items
				for (size_t idx = 0; idx < mLayerPopupMenu->getChildCount(); ++idx)
					mLayerPopupMenu->getChildAtIdx(idx)->setEnabled(true);

				gPopupMgr->ShowPopup(mLayerPopupMenu, args.position.d_x, args.position.d_y, GUISystem::PopupMgr::Callback(this, &Editor::LayerWindow::OnLayerPopupMenuItemClicked));
			}
		}
	}
	else
	{
		// item is entity
		EntitySystem::EntityHandle clickedEntity = gEntityMgr.GetEntity(item->getID());
		gEditorMgr.SelectEntity(clickedEntity);

		if (args.button == CEGUI::RightButton)
		{
			mCurrentPopupEntity = clickedEntity;
			gPopupMgr->ShowPopup(mEntityPopupMenu, args.position.d_x, args.position.d_y, GUISystem::PopupMgr::Callback(this, &Editor::LayerWindow::OnEntityPopupMenuItemClicked));
		}
	}
	return true;
}

bool LayerWindow::OnItemDoubleClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	CEGUI::ItemEntry* item = static_cast<CEGUI::ItemEntry*>(args.window->getParent());
	if (item->getUserData())
	{
		// item is layer
		if (args.button == CEGUI::LeftButton)
		{
			EntitySystem::LayerID layerID = dragContainer->getID();
			gLayerMgr.SetActiveLayer(layerID);
			Update();
		}
	}
	else
	{
		// item is entity
		if (args.button == CEGUI::LeftButton)
		{
			gEditorMgr.CenterCameraOnSelectedEntity();
		}
	}
	return true;
}

bool LayerWindow::OnTreeClicked(const CEGUI::EventArgs& e)
{
	// User clicked an empty space in tree window
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	if (args.button == CEGUI::RightButton)
	{
		// Disable menu items that require clicking on a layer
		mLayerPopupMenu->getChildAtIdx(LPI_MOVE_UP)->setEnabled(false);
		mLayerPopupMenu->getChildAtIdx(LPI_MOVE_DOWN)->setEnabled(false);
		mLayerPopupMenu->getChildAtIdx(LPI_RENAME)->setEnabled(false);
		mLayerPopupMenu->getChildAtIdx(LPI_REMOVE)->setEnabled(false);

		if (gLayerMgr.ExistsLayer(mCurrentPopupLayerID))
		{
			gPopupMgr->ShowPopup(mLayerPopupMenu, args.position.d_x, args.position.d_y, GUISystem::PopupMgr::Callback(this, &Editor::LayerWindow::OnLayerPopupMenuItemClicked));
		}
	}
	return true;
}

bool LayerWindow::OnLayerItemExpandClick(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::Window* layerItem = args.window->getParent();

	if (args.button == CEGUI::LeftButton)
	{
		EntitySystem::LayerID layerID = layerItem->getID();
		string layerName = gLayerMgr.GetLayerName(layerID);
		set<string>::iterator it = mExpandedLayers.find(layerName); 
		if (it == mExpandedLayers.end())
		{
			mExpandedLayers.insert(layerName);
		}
		else
		{
			mExpandedLayers.erase(it);
		}
		Update();
	}
	return true;
}

bool LayerWindow::OnLayerItemVisibilityToggleClick(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::Window* layerItem = args.window->getParent();

	if (args.button == CEGUI::LeftButton)
	{
		EntitySystem::LayerID layerID = layerItem->getID();
		gLayerMgr.SetLayerVisible(layerID, !gLayerMgr.IsLayerVisible(layerID));
		Update();
	}
	return true;
}

bool LayerWindow::OnDragDropItemDropped(const CEGUI::EventArgs& e)
{
	const CEGUI::DragDropEventArgs& args = static_cast<const CEGUI::DragDropEventArgs&>(e);

	if (args.dragDropItem->getUserString("DragDataType") == "Layer")
	{
		if (args.window->getUserString("DragDataType") != "Layer")
		{
			// we can only drag a Layer to Layer
			return true;
		}
		LayerID srcLayerID = args.dragDropItem->getID();
		LayerID destLayerID = args.window->getID();
		if (srcLayerID == destLayerID) return true;

		if (srcLayerID > destLayerID)
		{
			gLayerMgr.MoveLayerBehind(srcLayerID, destLayerID);
		}
		else
		{
			gLayerMgr.MoveLayerAbove(srcLayerID, destLayerID);
		}
	}
	else if (args.dragDropItem->getUserString("DragDataType") == "Entity")
	{
		EntitySystem::EntityHandle srcEntity = gEntityMgr.GetEntity(args.dragDropItem->getID());
		LayerID destLayerID;
		if (args.window->getUserString("DragDataType") == "Layer")
		{
			destLayerID = args.window->getID();
		}
		else if (args.window->getUserString("DragDataType") == "Entity")
		{
			EntitySystem::EntityHandle entity = gEntityMgr.GetEntity(args.window->getID());
			if (!gLayerMgr.EntityHasLayer(entity)) return true;

			destLayerID = gLayerMgr.GetEntityLayerID(entity);
		}
		else
		{
			return true;
		}
		gLayerMgr.SetEntityLayerID(srcEntity, destLayerID);
	}
	Update();
	return true;
}

void Editor::LayerWindow::Update()
{
	OC_CEGUI_TRY;
	{
		vector<CEGUI::ItemEntry*> itemsToBeDeleted;

		// The range of layers that are already in the window.
		EntitySystem::LayerID topLayerID = -1;
		EntitySystem::LayerID bottomLayerID = 1;

		// The list of entity ids that are already in the window.
		set<EntityID> entities;

		mExpandedLayerIDs.clear();
		for (set<string>::const_iterator it = mExpandedLayers.begin(); it != mExpandedLayers.end(); ++it)
		{
			if (!gLayerMgr.ExistsLayerName(*it))
				continue;

			mExpandedLayerIDs.insert(gLayerMgr.GetLayerID(*it));
		}

		// update existing items in the list
		size_t childCount = mTree->getItemCount();
		for (size_t i = 0; i < childCount; ++i)
		{
			CEGUI::ItemEntry* item = mTree->getItemFromIndex(i);
			if (item->getUserData())
			{
				// item is layer
				EntitySystem::LayerID layerID = item->getID();
				if (!gLayerMgr.ExistsLayer(layerID))
				{
					itemsToBeDeleted.push_back(item);
					continue;
				}

				if (layerID > topLayerID)
					topLayerID = layerID;
				if (layerID < bottomLayerID)
					bottomLayerID = layerID;

				UpdateLayerItem(item, layerID);
			}
			else
			{
				// item is entity
				entities.insert(item->getID());
				EntitySystem::EntityHandle entity = gEntityMgr.GetEntity(item->getID());

				// delete nonexistent entities, entities without layer and entities with collapsed layer
				if (!gEntityMgr.EntityExists(entity) || !gLayerMgr.EntityHasLayer(entity) || mExpandedLayerIDs.find(gLayerMgr.GetEntityLayerID(entity)) == mExpandedLayerIDs.end())
				{
					itemsToBeDeleted.push_back(item);
					continue;
				}

				UpdateEntityItem(item, entity);
			}
		}

		// delete items to be deleted
		for (vector<CEGUI::ItemEntry*>::const_iterator it = itemsToBeDeleted.begin(); it != itemsToBeDeleted.end(); ++it)
		{
			StoreItem(*it);
		}

		// add new layer items
		for (EntitySystem::LayerID layerID = gLayerMgr.GetBottomLayerID(); layerID <= gLayerMgr.GetTopLayerID(); ++layerID)
		{
			if (layerID >= bottomLayerID && layerID <= topLayerID) continue; // Layers in this range are already in the window

			CEGUI::ItemEntry* layerItem = RestoreLayerItem();
			SetupLayerItem(layerItem, layerID);
			mTree->addChildWindow(layerItem);

			UpdateLayerItem(layerItem, layerID);
		}
		
		// add new entity items
		EntitySystem::EntityList entityList;
		gEntityMgr.GetEntities(entityList);
		for (EntitySystem::EntityList::const_iterator it = entityList.begin(); it != entityList.end(); ++it)
		{
			// add only new entities
			if (entities.find(it->GetID()) != entities.end())
				continue;

			// add only entities that have an expanded layer
			if (!gLayerMgr.EntityHasLayer(*it) || mExpandedLayerIDs.find(gLayerMgr.GetEntityLayerID(*it)) == mExpandedLayerIDs.end())
				continue;

			CEGUI::ItemEntry* entityItem = RestoreEntityItem();
			SetupEntityItem(entityItem, *it);
			mTree->addChildWindow(entityItem);

			UpdateEntityItem(entityItem, *it);
		}
		mTree->sortList();
	}
	OC_CEGUI_CATCH;
}

void LayerWindow::SetSelectedEntity(EntitySystem::EntityHandle selectedEntity)
{
	if (!selectedEntity.IsValid())
	{
		mTree->clearAllSelections();
	}
	else
	{
		size_t itemCount = mTree->getItemCount();
		for (size_t idx = 0; idx < itemCount; ++idx)
		{
			CEGUI::ItemEntry* item = mTree->getItemFromIndex(idx);

			if (item->getUserData() == 0 && (EntitySystem::EntityID)item->getID() == selectedEntity.GetID())
			{
				item->setSelected(true);
			}
			else
			{
				item->setSelected(false);
			}
		}
	}
}

void Editor::LayerWindow::Clear()
{
	ocInfo << "Clearing LayerWindow";

	while (mTree->getItemCount() > 0)
	{
		StoreItem(mTree->getItemFromIndex(mTree->getItemCount() - 1));
	}
	OC_ASSERT(mTree->getItemCount() == 0);
}

void LayerWindow::UpdateLayerItem(CEGUI::ItemEntry* layerItem, EntitySystem::LayerID layerID)
{
	CEGUI::Window* layerItemText = layerItem->getChildAtIdx(0)->getChildAtIdx(0);
	CEGUI::Window* layerItemEye = layerItem->getChildAtIdx(2);
	
	// update layer name
	if (layerItemText->getText() != gLayerMgr.GetLayerName(layerID))
		layerItemText->setText(utf8StringToCEGUI(gLayerMgr.GetLayerName(layerID)));

	// update whether layer is active
	if (gLayerMgr.GetActiveLayer() == layerID)
		layerItemText->setFont("Editor-Bold");
	else
		layerItemText->setFont("Editor");

	// update whether layer is visible
	if (gLayerMgr.IsLayerVisible(layerID))
		layerItemEye->setProperty("Image", "set:EditorToolbar image:btnEyeEnabled");
	else
		layerItemEye->setProperty("Image", "set:EditorToolbar image:btnEyeDisabled");

	// update layer expand/collapse icon
	CEGUI::Window* layerItemButton = layerItem->getChildAtIdx(1);
	layerItemButton->setWantsMultiClickEvents(false);
	if (mExpandedLayerIDs.find(layerID) == mExpandedLayerIDs.end())
		layerItemButton->setText("+");
	else
		layerItemButton->setText(CEGUI::String(1, 8722)); // minus sign
}

void LayerWindow::UpdateEntityItem(CEGUI::ItemEntry* entityItem, EntitySystem::EntityHandle entity)
{
	CEGUI::Window* entityItemText = entityItem->getChildAtIdx(0)->getChildAtIdx(0);
	
	// update entity name
	if (entityItemText->getText() != gEntityMgr.GetEntityName(entity))
		entityItemText->setText(utf8StringToCEGUI(gEntityMgr.GetEntityName(entity)));

	if (gEditorMgr.GetSelectedEntity() == entity)
		entityItem->setSelected(true);
}

void LayerWindow::NewLayerPromptCallback(bool clickedOK, const string& text, int32 layerID)
{
	if (!clickedOK)
		return;
	CreateLayer(layerID, text);
	Update();
}

void LayerWindow::RenameLayerPromptCallback(bool clickedOK, const string& text, int32 layerID)
{
	if (!clickedOK)
		return;
	RenameLayer(layerID, text);
	Update();
}

void LayerWindow::CreatePopupMenus()
{
	mLayerPopupMenu = gPopupMgr->CreatePopupMenu("Editor/LayerWindow/LayerPopup");
	mLayerPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("Editor/LayerWindow/LayerPopup/MoveUp", TR("layer_moveup"), TR("layer_moveup_hint"), LPI_MOVE_UP));
	mLayerPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("Editor/LayerWindow/LayerPopup/MoveDown", TR("layer_movedown"), TR("layer_movedown_hint"), LPI_MOVE_DOWN));
	mLayerPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("Editor/LayerWindow/LayerPopup/New", TR("layer_new"), TR("layer_new_hint"), LPI_NEW));
	mLayerPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("Editor/LayerWindow/LayerPopup/Rename", TR("layer_rename"), TR("layer_rename_hint"), LPI_RENAME));
	mLayerPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("Editor/LayerWindow/LayerPopup/Remove", TR("layer_remove"), TR("layer_remove_hint"), LPI_REMOVE));

	mEntityPopupMenu = gPopupMgr->CreatePopupMenu("Editor/LayerWindow/EntityPopup");
	mEntityPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("Editor/LayerWindow/EntityPopup/MoveUp", TR("layer_entitymoveup"), TR("layer_entitymoveup_hint"), EPI_MOVE_UP));
	mEntityPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("Editor/LayerWindow/EntityPopup/MoveDown", TR("layer_entitymovedown"), TR("layer_entitymovedown_hint"), EPI_MOVE_DOWN));
}

void LayerWindow::DestroyPopupMenus()
{
	gPopupMgr->DestroyPopupMenu(mLayerPopupMenu);
	gPopupMgr->DestroyPopupMenu(mEntityPopupMenu);
	mLayerPopupMenu = 0;
	mEntityPopupMenu = 0;
}

void LayerWindow::OnLayerPopupMenuItemClicked(CEGUI::Window* menuItem)
{
	switch(static_cast<eLayerPopupItem>(menuItem->getID()))
	{
	case LPI_MOVE_UP:
		MoveLayerUp(mCurrentPopupLayerID);
		break;
	case LPI_MOVE_DOWN:
		MoveLayerDown(mCurrentPopupLayerID);
		break;
	case LPI_NEW:
		ShowNewLayerPrompt(mCurrentPopupLayerID);
		break;
	case LPI_RENAME:
		ShowRenameLayerPrompt(mCurrentPopupLayerID);
		break;
	case LPI_REMOVE:
		RemoveLayer(mCurrentPopupLayerID);
		break;
	default:
		OC_NOT_REACHED();
	}
}

void LayerWindow::OnEntityPopupMenuItemClicked(CEGUI::Window* menuItem)
{
	switch(static_cast<eEntityPopupItem>(menuItem->getID()))
	{
	case EPI_MOVE_UP:
		MoveEntityUp(mCurrentPopupEntity);
		break;
	case EPI_MOVE_DOWN:
		MoveEntityDown(mCurrentPopupEntity);
		break;
	default:
		OC_NOT_REACHED();
	}
}

CEGUI::ItemEntry* LayerWindow::CreateLayerItem()
{
	static uint32 layerItemCounter = 0;
	CEGUI::String windowName = "Editor/LayerWindow/List/LayerItem" + StringConverter::ToString(layerItemCounter++);

	CEGUI::ItemEntry* layerItem = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("Editor/ListboxItem", windowName));
	layerItem->setUserData((void*)1);

	CEGUI::Window* dragContainer = gGUIMgr.CreateWindow("DragContainer", windowName + "/DC");
	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 16), CEGUI::UDim(0, 0), CEGUI::UDim(1, -16), CEGUI::UDim(1, 0)));
	layerItem->addChildWindow(dragContainer);

	CEGUI::Window* layerItemText = gGUIMgr.CreateWindow("Editor/StaticText", windowName + "/DC/Text");
	layerItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	layerItemText->setProperty("FrameEnabled", "False");
	layerItemText->setProperty("BackgroundEnabled", "False");
	layerItemText->setMousePassThroughEnabled(true);

	CEGUI::Window* layerItemButton = gGUIMgr.CreateWindow("Editor/StaticText", windowName + "/Button");
	layerItemButton->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(0, 10), CEGUI::UDim(1, 0)));
	layerItemButton->setProperty("FrameEnabled", "False");
	layerItemButton->setProperty("BackgroundEnabled", "False");
	layerItemButton->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnLayerItemExpandClick, this));
	layerItem->addChildWindow(layerItemButton);

	CEGUI::Window* layerItemVisibilityToggle = gGUIMgr.CreateWindow("Editor/StaticImage", windowName + "/VisibilityToggle");
	layerItemVisibilityToggle->setArea(CEGUI::URect(CEGUI::UDim(1, -16), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	layerItemVisibilityToggle->setProperty("FrameEnabled", "False");
	layerItemVisibilityToggle->setProperty("BackgroundEnabled", "False");
	layerItemVisibilityToggle->setWantsMultiClickEvents(false);
	layerItemVisibilityToggle->setTooltipText(TR("layer_visibilitytoggle_hint"));
	layerItemVisibilityToggle->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnLayerItemVisibilityToggleClick, this));
	layerItem->addChildWindow(layerItemVisibilityToggle); 

	dragContainer->addChildWindow(layerItemText);
/*
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnDragContainerMouseButtonDown, this));
*/
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnItemClicked, this));
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnItemDoubleClicked, this));
	dragContainer->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnDragDropItemDropped, this));
	dragContainer->setUserString("DragDataType", "Layer");

	return layerItem;
}

CEGUI::ItemEntry* LayerWindow::CreateEntityItem()
{
	static uint32 entityItemCounter = 0;
	CEGUI::String windowName = "Editor/LayerWindow/List/EntityItem" + StringConverter::ToString(entityItemCounter++);

	CEGUI::ItemEntry* entityItem = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("Editor/ListboxItem", windowName));
	entityItem->setUserData(0);

	CEGUI::Window* dragContainer = gGUIMgr.CreateWindow("DragContainer", windowName + "/DC");
	dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 20), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	entityItem->addChildWindow(dragContainer);

	CEGUI::Window* entityItemText = gGUIMgr.CreateWindow("Editor/StaticText", windowName + "/DC/Text");
	entityItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	entityItemText->setProperty("FrameEnabled", "False");
	entityItemText->setProperty("BackgroundEnabled", "False");
	entityItemText->setMousePassThroughEnabled(true);

	dragContainer->addChildWindow(entityItemText);
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnItemClicked, this));
	dragContainer->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnItemDoubleClicked, this));
	dragContainer->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnDragDropItemDropped, this));
	dragContainer->setUserString("DragDataType", "Entity");

	return entityItem;
}

void LayerWindow::SetupLayerItem(CEGUI::ItemEntry* layerItem, EntitySystem::LayerID layerID)
{
	layerItem->setID(layerID);
	layerItem->getChildAtIdx(0)->setID(layerID);
	layerItem->setSelected(false);
}

void LayerWindow::SetupEntityItem(CEGUI::ItemEntry* entityItem, EntitySystem::EntityHandle entityHandle)
{
	entityItem->setID(entityHandle.GetID());
	entityItem->getChildAtIdx(0)->setID(entityHandle.GetID());
	entityItem->setSelected(false);
}

void LayerWindow::StoreItem(CEGUI::ItemEntry* item)
{
	CEGUI::Window* parent = item->getParent();
	if (parent)
		parent->removeChildWindow(item);

	if (item->getUserData() == 0)
	{
		// It is EntityItem
		mEntityItemCache.push_back(item);
	}
	else
	{
		mLayerItemCache.push_back(item);
	}
}

CEGUI::ItemEntry* LayerWindow::RestoreLayerItem()
{
	CEGUI::ItemEntry* item = 0;
	if (mLayerItemCache.empty())
	{
		item = CreateLayerItem();
	}
	else
	{
		item = mLayerItemCache.back();
		mLayerItemCache.pop_back();
	}
	return item;
}

CEGUI::ItemEntry* LayerWindow::RestoreEntityItem()
{
	CEGUI::ItemEntry* item = 0;
	if (mEntityItemCache.empty())
	{
		item = CreateEntityItem();
	}
	else
	{
		item = mEntityItemCache.back();
		mEntityItemCache.pop_back();
	}
	return item;
}

void LayerWindow::DestroyItemCache()
{
	for (ItemCache::iterator it = mLayerItemCache.begin(); it != mLayerItemCache.end(); ++it)
	{
		gGUIMgr.DestroyWindow((*it));
	}
	mLayerItemCache.clear();

	for (ItemCache::iterator it = mEntityItemCache.begin(); it != mEntityItemCache.end(); ++it)
	{
		gGUIMgr.DestroyWindow((*it));
	}
	mEntityItemCache.clear();
}

bool LayerWindow::SortCallback(const CEGUI::ItemEntry* first , const CEGUI::ItemEntry* second)
{
	bool isFirstEntity = (first->getUserData() == 0);
	bool isSecondEntity = (second->getUserData() == 0);

	EntitySystem::LayerID firstLayerID = isFirstEntity ? gLayerMgr.GetEntityLayerID(gEntityMgr.GetEntity(first->getID())) : first->getID();
	EntitySystem::LayerID secondLayerID = isSecondEntity ? gLayerMgr.GetEntityLayerID(gEntityMgr.GetEntity(second->getID())) : second->getID();

	if (firstLayerID != secondLayerID)
		return firstLayerID > secondLayerID;
	if (!isFirstEntity)
		return 1;
	if (!isSecondEntity)
		return 0;
	return first->getID() < second->getID();
}
