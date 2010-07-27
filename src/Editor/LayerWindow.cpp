#include "Common.h"
#include "Editor/LayerWindow.h"
#include "Editor/EditorMgr.h"
#include "Editor/PopupMenu.h"
#include "GUISystem/CEGUITools.h"
#include "GUISystem/ItemListboxProperties.h"
#include "GUISystem/PromptBox.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"

using namespace Editor;

GUISystem::ItemListboxWantsMouseWheel gLayerMouseWheelProperty;

/// Sorting callback function. This function provides an order of items in the list
/// for the sake of simulating tree widget.
bool MySortCallback(const CEGUI::ItemEntry*, const CEGUI::ItemEntry*);

Editor::LayerWindow::LayerWindow():
		mWindow(0), mTree(0), mUpdateTimer(0)
{
}

Editor::LayerWindow::~LayerWindow()
{
}

void Editor::LayerWindow::Init()
{
	CEGUI_EXCEPTION_BEGIN

	mWindow = gGUIMgr.LoadSystemLayout("LayerWindow.layout", "EditorRoot/LayerWindow");
	OC_ASSERT(mWindow);
	gGUIMgr.GetGUISheet()->addChildWindow(mWindow);

	mTree = static_cast<CEGUI::ItemListbox*>(mWindow->getChild(mWindow->getName() + "/List"));
	mTree->setSortMode(CEGUI::ItemListBase::UserSort);
	mTree->setSortCallback(&MySortCallback);
	mTree->addProperty(&gLayerMouseWheelProperty);

	CEGUI_EXCEPTION_END

	OC_ASSERT(mWindow != 0);
	OC_ASSERT(mTree != 0);

	UpdateTree();
}

void Editor::LayerWindow::Update(float32 delta)
{
	// Updates tree every second
	mUpdateTimer += delta;
	if (mUpdateTimer > 1)
	{
		mUpdateTimer = 0;
		UpdateTree();
	}
}

void LayerWindow::NewLayer(EntitySystem::LayerID layerID)
{
	GUISystem::PromptBox* prompt = new GUISystem::PromptBox(layerID);
	prompt->SetText("Please enter new name of the new layer");
	prompt->RegisterCallback(new GUISystem::PromptBox::Callback<Editor::LayerWindow>(this, &LayerWindow::NewLayerPromptCallback));
	prompt->Show();
}

void LayerWindow::MoveLayerUp(EntitySystem::LayerID layerID)
{
	gLayerMgr.MoveLayerUp(layerID);
	UpdateTree();
}

void LayerWindow::MoveLayerDown(EntitySystem::LayerID layerID)
{
	gLayerMgr.MoveLayerDown(layerID);
	UpdateTree();
}

void LayerWindow::RenameLayer(EntitySystem::LayerID layerID)
{
	GUISystem::PromptBox* prompt = new GUISystem::PromptBox(layerID);
	prompt->SetText("Please enter new name of the layer");
	prompt->RegisterCallback(new GUISystem::PromptBox::Callback<Editor::LayerWindow>(this, &LayerWindow::RenameLayerPromptCallback));
	prompt->Show();
}

void LayerWindow::RemoveLayer(EntitySystem::LayerID layerID)
{
	gLayerMgr.DeleteLayer(layerID, false);
	UpdateTree();
}

void LayerWindow::MoveEntityUp(EntitySystem::EntityHandle entity)
{
	gLayerMgr.MoveEntityUp(entity);
	UpdateTree();
}

void LayerWindow::MoveEntityDown(EntitySystem::EntityHandle entity)
{
	gLayerMgr.MoveEntityDown(entity);
	UpdateTree();
}

bool Editor::LayerWindow::OnDragContainerMouseButtonDown(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	CEGUI::ItemEntry* itemEntry = static_cast<CEGUI::ItemEntry*>(args.window->getParent());
	itemEntry->setSelected(true);

	return true;
}

bool LayerWindow::OnDragContainerMouseButtonUp(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	// We only handle right button click.
	if (args.button != CEGUI::RightButton)
		return false;

	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	CEGUI::ItemEntry* item = static_cast<CEGUI::ItemEntry*>(args.window->getParent());
	if (item->getUserData())
	{
		// item is layer
		EntitySystem::LayerID layerID = item->getID();
		if (gLayerMgr.ExistsLayer(layerID))
		{
			PopupMenu* menu = new PopupMenu("EditorRoot/Popup/Layer", true);
			menu->Init<EntitySystem::LayerID>(layerID);
			menu->Open(args.position.d_x, args.position.d_y);
			gEditorMgr.RegisterPopupMenu(menu);
		}
	}
	else
	{
		// item is entity
		EntitySystem::EntityHandle entity = gEntityMgr.GetEntity(item->getID());
		PopupMenu* menu = new PopupMenu("EditorRoot/Popup/LayerEntity", true);
		menu->Init<EntitySystem::EntityHandle>(entity);
		menu->Open(args.position.d_x, args.position.d_y);
		gEditorMgr.RegisterPopupMenu(menu);
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
		UpdateTree();
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
		UpdateTree();
	}
	return true;
}

bool LayerWindow::OnLayerItemDoubleClick(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	if (args.button == CEGUI::LeftButton)
	{
		EntitySystem::LayerID layerID = dragContainer->getID();
		gLayerMgr.SetActiveLayer(layerID);
		UpdateTree();
	}
	return true;
}

bool LayerWindow::OnEntityItemDoubleClick(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	if (args.button == CEGUI::LeftButton)
	{
		EntitySystem::EntityHandle entity = gEntityMgr.GetEntity(dragContainer->getID());
		gEditorMgr.SetCurrentEntity(entity);
	}
	return true;
}


bool LayerWindow::OnDragDropItemDropped(const CEGUI::EventArgs& e)
{
	const CEGUI::DragDropEventArgs& args = static_cast<const CEGUI::DragDropEventArgs&>(e);
	
	ocInfo << "Dragged '" << args.dragDropItem->getUserString("DragDataType") << "' to '" << args.window->getUserString("DragDataType");
	
	if (args.dragDropItem->getUserString("DragDataType") == "Layer")
	{
		if (args.window->getUserString("DragDataType") != "Layer")
		{
			// we can only drag a Layer to Layer
			return true;
		}
		LayerID srcLayerID = args.dragDropItem->getID();
		LayerID destLayerID = args.window->getID();
		if (srcLayerID == destLayerID)
			return true;
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
			if (!gLayerMgr.EntityHasLayer(entity))
				return true;
			destLayerID = gLayerMgr.GetEntityLayerID(entity);
		}
		else
		{
			return true;
		}
		gLayerMgr.SetEntityLayerID(srcEntity, destLayerID);
	}
	UpdateTree();
	return true;
}

void Editor::LayerWindow::UpdateTree()
{
	CEGUI_EXCEPTION_BEGIN
	vector<CEGUI::Window*> itemsToBeDeleted;

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
		CEGUI::ItemEntry* item = static_cast<CEGUI::ItemEntry*>(mTree->getItemFromIndex(i));
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
	for (vector<CEGUI::Window*>::const_iterator it = itemsToBeDeleted.begin(); it != itemsToBeDeleted.end(); ++it)
	{
		gGUIMgr.DestroyWindow(*it);
	}

	// add new layer items
	for (EntitySystem::LayerID layerID = gLayerMgr.GetBottomLayerID(); layerID <= gLayerMgr.GetTopLayerID(); ++layerID)
	{
		if (layerID >= bottomLayerID && layerID <= topLayerID) continue; // Layers in this range are already in the window

		string layerName = gLayerMgr.GetLayerName(layerID);

		CEGUI::ItemEntry* layerItem = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("Editor/ListboxItem"));
		layerItem->setUserData((void*)1);
		layerItem->setID(layerID);
		
		CEGUI::Window* dragContainer = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("DragContainer"));
		dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 16), CEGUI::UDim(0, 0), CEGUI::UDim(1, -16), CEGUI::UDim(1, 0)));
		layerItem->addChildWindow(dragContainer);

		CEGUI::Window* layerItemText = gGUIMgr.CreateWindow("Editor/StaticText");
		layerItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		layerItemText->setProperty("FrameEnabled", "False");
		layerItemText->setProperty("BackgroundEnabled", "False");
		layerItemText->setMousePassThroughEnabled(true);

		CEGUI::Window* layerItemButton = gGUIMgr.CreateWindow("Editor/StaticText");
		layerItemButton->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(0, 10), CEGUI::UDim(1, 0)));
		layerItemButton->setProperty("FrameEnabled", "False");
		layerItemButton->setProperty("BackgroundEnabled", "False");
		layerItemButton->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnLayerItemExpandClick, this));
		layerItem->addChildWindow(layerItemButton);
		
		CEGUI::Window* layerItemVisibilityToggle = gGUIMgr.CreateWindow("Editor/StaticImage");
		layerItemVisibilityToggle->setArea(CEGUI::URect(CEGUI::UDim(1, -16), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		layerItemVisibilityToggle->setProperty("FrameEnabled", "False");
		layerItemVisibilityToggle->setProperty("BackgroundEnabled", "False");
		layerItemVisibilityToggle->setWantsMultiClickEvents(false);
		layerItemVisibilityToggle->setTooltipText(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "layer_visibilitytoggle_hint"));
		layerItemVisibilityToggle->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnLayerItemVisibilityToggleClick, this));
		layerItem->addChildWindow(layerItemVisibilityToggle); 

		dragContainer->addChildWindow(layerItemText);
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnDragContainerMouseButtonDown, this));
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnDragContainerMouseButtonUp, this));
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnLayerItemDoubleClick, this));
		dragContainer->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnDragDropItemDropped, this));

		dragContainer->setID(layerID);
		dragContainer->setUserString("DragDataType", "Layer");

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

		CEGUI::ItemEntry* entityItem = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("Editor/ListboxItem"));
		entityItem->setID(it->GetID());
		entityItem->setUserData(0);
			
		CEGUI::Window* dragContainer = static_cast<CEGUI::ItemEntry*>(gGUIMgr.CreateWindow("DragContainer"));
		dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 20), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		entityItem->addChildWindow(dragContainer);

		CEGUI::Window* entityItemText = gGUIMgr.CreateWindow("Editor/StaticText");
		entityItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		
		entityItemText->setProperty("FrameEnabled", "False");
		entityItemText->setProperty("BackgroundEnabled", "False");
		entityItemText->setMousePassThroughEnabled(true);

		dragContainer->addChildWindow(entityItemText);
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnDragContainerMouseButtonDown, this));
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnDragContainerMouseButtonUp, this));
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnEntityItemDoubleClick, this));
		dragContainer->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnDragDropItemDropped, this));

		dragContainer->setID(it->GetID());
		dragContainer->setUserString("DragDataType", "Entity");
		mTree->addChildWindow(entityItem);
		UpdateEntityItem(entityItem, *it);
	}
	mTree->sortList();
	CEGUI_EXCEPTION_END
}

void LayerWindow::UpdateLayerItem(CEGUI::Window* layerItem, EntitySystem::LayerID layerID)
{
	CEGUI::Window* layerItemText = layerItem->getChildAtIdx(0)->getChildAtIdx(0);
	CEGUI::Window* layerItemEye = layerItem->getChildAtIdx(2);
	
	// update layer name
	if (layerItemText->getText() != gLayerMgr.GetLayerName(layerID))
		layerItemText->setText(gLayerMgr.GetLayerName(layerID));

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

void LayerWindow::UpdateEntityItem(CEGUI::Window* entityItem, EntitySystem::EntityHandle entity)
{
	CEGUI::Window* entityItemText = entityItem->getChildAtIdx(0)->getChildAtIdx(0);
	
	// update entity name
	if (entityItemText->getText() != gEntityMgr.GetEntityName(entity))
		entityItemText->setText(gEntityMgr.GetEntityName(entity));

}

void LayerWindow::NewLayerPromptCallback(bool clickedOK, string text, int32 layerID)
{
	if (!clickedOK)
		return;
	LayerID newLayerID = gLayerMgr.AddTopLayer(text);
	gLayerMgr.MoveLayerBehind(newLayerID, layerID);
	UpdateTree();
}

void LayerWindow::RenameLayerPromptCallback(bool clickedOK, string text, int32 layerID)
{
	if (!clickedOK)
		return;
	gLayerMgr.SetLayerName(layerID, text);
	UpdateTree();
}

bool MySortCallback(const CEGUI::ItemEntry* first , const CEGUI::ItemEntry* second)
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
