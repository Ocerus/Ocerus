#include "Common.h"
#include "Editor/LayerWindow.h"
#include "Editor/EditorMgr.h"
#include "GUISystem/CEGUITools.h"
#include "GUISystem/ItemListboxProperties.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"

using namespace Editor;

GUISystem::ItemListboxWantsMouseWheel gLayerMouseWheelProperty;

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
	mTree->resetList();
	mTree->addProperty(&gLayerMouseWheelProperty);
	///@todo

	//CEGUI::Window* refreshButton = mWindow->getChildRecursive(mWindow->getName() + "/Toolbar/Refresh");
	//refreshButton->subscribeEvent(CEGUI::PushButton::EventClicked, //CEGUI::Event::Subscriber(&Editor::LayerWindow::OnRefreshButtonClicked, this));

	CEGUI_EXCEPTION_END

	OC_ASSERT(mWindow != 0);
	OC_ASSERT(mTree != 0);

	UpdateTree();
}

void Editor::LayerWindow::Update(float32 delta)
{
	mUpdateTimer += delta;
	if (mUpdateTimer > 1)
	{
		mUpdateTimer = 0;
		UpdateTree();
	}
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

bool LayerWindow::OnLayerExpandClick(const CEGUI::EventArgs& e)
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


bool LayerWindow::OnLayerMouseDoubleClick(const CEGUI::EventArgs& e)
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


/*
bool Editor::LayerWindow::OnDragContainerMouseButtonUp(const CEGUI::EventArgs& e)
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
			PopupMenu* menu;
			if (gEditorMgr.GetCurrentProject()->IsResourceScene(resource))
			{
				menu = new PopupMenu("EditorRoot/Popup/ResourceWithScene");
			}
			else
			{
				menu = new PopupMenu("EditorRoot/Popup/Resource");
			}
			menu->Init<ResourceSystem::ResourcePtr>(resource);
			menu->Open(args.position.d_x, args.position.d_y);
			gEditorMgr.RegisterPopupMenu(menu);
		}
		return true;
	}

	return true;
}

bool Editor::LayerWindow::OnDragContainerMouseDoubleClick(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::DragContainer* dragContainer = static_cast<CEGUI::DragContainer*>(args.window);
	if (dragContainer->isBeingDragged()) return false;

	if (args.button == CEGUI::LeftButton)
	{
		ResourceSystem::ResourcePtr resource = GetItemAtIndex(dragContainer->getID());
		if (resource.get() && gEditorMgr.GetCurrentProject()->IsResourceScene(resource))
		{
			gEditorMgr.GetCurrentProject()->OpenScene(resource);
		}
		return true;
	}

	return true;
}
*/

/*
bool Editor::LayerWindow::OnEditNewLayerKeyDown(const CEGUI::EventArgs& args)
{
	const CEGUI::KeyEventArgs& keyArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
	if (keyArgs.scancode == CEGUI::Key::Return)
		return OnButtonAddLayerClicked(args);
	return false;
}

bool Editor::LayerWindow::OnButtonSetActiveLayerClicked(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	CEGUI::TreeItem* currentItem = mTreeWindow->getFirstSelectedItem();
	if (currentItem == 0 || currentItem->getUserData() != 0) return true;
	EntitySystem::LayerID layerId = currentItem->getID();
	gLayerMgr.SetActiveLayer(layerId);
	UpdateTree();
	return true;
}

bool Editor::LayerWindow::OnButtonAddLayerClicked(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	const CEGUI::String& newLayerName = mNewLayerEditbox->getText();
	gLayerMgr.AddBottomLayer(newLayerName.c_str());
	mNewLayerEditbox->setText("");
	UpdateTree();
	return true;
}

bool Editor::LayerWindow::OnButtonUpDownClicked(const CEGUI::EventArgs& args)
{
	const CEGUI::WindowEventArgs& windowArgs = static_cast<const CEGUI::WindowEventArgs&>(args);
	CEGUI::TreeItem* currentItem = mTreeWindow->getFirstSelectedItem();
	if (currentItem == 0) return true;

	bool movingUp = (windowArgs.window == mUpButton);

	if (currentItem->getUserData() == 0)
	{
		// We are moving layer
		EntitySystem::LayerID layerId = currentItem->getID();
		if (movingUp)
			gLayerMgr.MoveLayerUp(layerId);
		else
			gLayerMgr.MoveLayerDown(layerId);
	}
	else
	{
		// We are moving entity
		EntitySystem::EntityHandle entity = gEntityMgr.GetEntity(currentItem->getID());
		if (movingUp)
			gLayerMgr.MoveEntityUp(entity);
		else
			gLayerMgr.MoveEntityDown(entity);
	}
	UpdateTree();
	return true;
}

bool Editor::LayerWindow::OnButtonEditEntityClicked(const CEGUI::EventArgs&)
{
	CEGUI::TreeItem* currentItem = mTreeWindow->getFirstSelectedItem();
	if (currentItem == 0 || currentItem->getUserData() == 0) return true;
	EntitySystem::EntityHandle entity = gEntityMgr.GetEntity(currentItem->getID());
	gEditorMgr.SetCurrentEntity(entity);
	return true;
}

bool Editor::LayerWindow::OnButtonToggleLayerVisibilityClicked(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	CEGUI::TreeItem* currentItem = mTreeWindow->getFirstSelectedItem();
	if (currentItem == 0 || currentItem->getUserData() != 0) return true;
	EntitySystem::LayerID layerId = currentItem->getID();
	gLayerMgr.ToggleLayerVisible(layerId);
	UpdateTree();
	return true;
}
*/

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
				break;
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

		CEGUI::ItemEntry* layerItem = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("Editor/ListboxItem", mTree->getName() + "/LayerItem" + StringConverter::ToString(layerID)));
		layerItem->setUserData((void*)1);
		layerItem->setID(layerID);
		
		CEGUI::Window* dragContainer = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("DragContainer", mTree->getName() + "/DCLayerItem" + StringConverter::ToString(layerID)));
		dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 16), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		layerItem->addChildWindow(dragContainer);

		CEGUI::Window* layerItemText = gCEGUIWM.createWindow("Editor/StaticText", mTree->getName() + "/LayerItemText" + StringConverter::ToString(layerID));
		layerItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		layerItemText->setProperty("FrameEnabled", "False");
		layerItemText->setProperty("BackgroundEnabled", "False");
		layerItemText->setMousePassThroughEnabled(true);

		CEGUI::Window* layerItemButton = gCEGUIWM.createWindow("Editor/StaticText", mTree->getName() + "/LayerItemButton" + StringConverter::ToString(layerID));
		layerItemButton->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(0, 10), CEGUI::UDim(1, 0)));
		layerItemButton->setProperty("FrameEnabled", "False");
		layerItemButton->setProperty("BackgroundEnabled", "False");
		layerItemButton->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnLayerExpandClick, this));
		layerItem->addChildWindow(layerItemButton);

		dragContainer->addChildWindow(layerItemText);
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnDragContainerMouseButtonDown, this));
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnLayerMouseDoubleClick, this));

		dragContainer->setID(layerID);

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

		CEGUI::ItemEntry* entityItem = static_cast<CEGUI::ItemEntry*>( gCEGUIWM.createWindow("Editor/ListboxItem", mTree->getName() + "/EntityItem" + StringConverter::ToString(it->GetID())) );
		entityItem->setID(it->GetID());
		entityItem->setUserData(0);
			
		CEGUI::Window* dragContainer = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("DragContainer", mTree->getName() + "/DCEntityItem" + StringConverter::ToString(it->GetID())));
		dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 20), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		entityItem->addChildWindow(dragContainer);

		CEGUI::Window* entityItemText = gCEGUIWM.createWindow("Editor/StaticText", mTree->getName() + "/EntityItemText" + StringConverter::ToString(it->GetID()));
		entityItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		
		entityItemText->setProperty("FrameEnabled", "False");
		entityItemText->setProperty("BackgroundEnabled", "False");
		entityItemText->setMousePassThroughEnabled(true);

		dragContainer->addChildWindow(entityItemText);
		dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnDragContainerMouseButtonDown, this));
		//dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnDragContainerMouseButtonUp, this));
		//dragContainer->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, CEGUI::Event::Subscriber(&Editor::LayerWindow::OnDragContainerMouseDoubleClick, this));

		dragContainer->setID(it->GetID());
		//dragContainer->setUserString("DragDataType", "Resource");
		mTree->addChildWindow(entityItem);
		UpdateEntityItem(entityItem, *it);
	}
	mTree->sortList();
	CEGUI_EXCEPTION_END
}

void LayerWindow::UpdateLayerItem(CEGUI::Window* layerItem, EntitySystem::LayerID layerID)
{
	CEGUI::Window* layerItemText = layerItem->getChildAtIdx(0)->getChildAtIdx(0);
	
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
		layerItemText->setProperty("TextColours", "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");
	else
		layerItemText->setProperty("TextColours", "tl:66FFFFFF tr:66FFFFFF bl:66FFFFFF br:66FFFFFF");

	// update layer expand/collapse icon
	CEGUI::Window* layerItemButton = layerItem->getChildAtIdx(1);
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



bool MySortCallback(const CEGUI::ItemEntry* first , const CEGUI::ItemEntry* second)
{
	bool isFirstEntity = (first->getUserData() == 0);
	bool isSecondEntity = (second->getUserData() == 0);

	EntitySystem::LayerID firstLayerID = isFirstEntity ? gLayerMgr.GetEntityLayerID(gEntityMgr.GetEntity(first->getID())) : first->getID();
	EntitySystem::LayerID secondLayerID = isSecondEntity ? gLayerMgr.GetEntityLayerID(gEntityMgr.GetEntity(second->getID())) : second->getID();

	if (firstLayerID != secondLayerID)
		return firstLayerID < secondLayerID;
	if (!isFirstEntity)
		return 1;
	if (!isSecondEntity)
		return 0;
	return first->getID() < second->getID();
}
