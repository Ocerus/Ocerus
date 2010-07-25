#include "Common.h"
#include "Editor/LayerWindow.h"
#include "Editor/EditorMgr.h"
#include "GUISystem/CEGUITools.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"

using namespace Editor;

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
	//mTree->addProperty(&gResourceMouseWheelProperty);
	///@todo

	//CEGUI::Window* refreshButton = mWindow->getChildRecursive(mWindow->getName() + "/Toolbar/Refresh");
	//refreshButton->subscribeEvent(CEGUI::PushButton::EventClicked, //CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnRefreshButtonClicked, this));

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
	vector<CEGUI::Window*> itemsToBeDeleted;

	// The range of layers that are already in the window.
	EntitySystem::LayerID topLayerID = -1;
	EntitySystem::LayerID bottomLayerID = 1;

	// The list of entity ids that are already in the window.
	vector<EntityID> entities;

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
			if (item->getText() != gLayerMgr.GetLayerName(layerID))
				item->setText(gLayerMgr.GetLayerName(layerID));
			
			///@todo Update whether layer is active
		}
		else
		{
			// item is entity
			EntitySystem::EntityHandle entity = gEntityMgr.GetEntity(item->getID());
			if (!gEntityMgr.EntityExists(entity) || !gLayerMgr.EntityHasLayer(entity))
			{
				itemsToBeDeleted.push_back(item);
				break;
			}
			entities.push_back(item->getID());

			///@todo Update entity name
		}
	}

	// Delete items to be deleted
	for (vector<CEGUI::Window*>::const_iterator it = itemsToBeDeleted.begin(); it != itemsToBeDeleted.end(); ++it)
	{
		gGUIMgr.DestroyWindow(*it);
	}

	// Add new layers
	for (EntitySystem::LayerID layerID = gLayerMgr.GetBottomLayerID(); layerID <= gLayerMgr.GetTopLayerID(); ++layerID)
	{
		if (layerID >= bottomLayerID && layerID <= topLayerID) continue; // Layers in this range are already in the window

		string layerName = gLayerMgr.GetLayerName(layerID);
		/*
		if (gLayerMgr.GetActiveLayer() == layerID)
			layerName = "* " + layerName;
		else
			layerName = "  " + layerName;
		*/
		//layerName += (gLayerMgr.IsLayerVisible(layerID)) ? "" : " [hidden]"; ///@todo translate

		CEGUI::ItemEntry* layerItem = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("Editor/ListboxItem", mTree->getName() + "/LayerItem" + StringConverter::ToString(layerID)));
		layerItem->setText(layerName);
		layerItem->setUserData((void*)1);
		layerItem->setID(layerID);
		mTree->addChildWindow(layerItem);
	}
	
	// Add new entities
	EntitySystem::EntityList entityList;
	gEntityMgr.GetEntities(entityList);
	for (EntitySystem::EntityList::const_iterator it = entityList.begin(); it != entityList.end(); ++it)
	{
		// Add only new entities
		if (Containers::find(entities.begin(), entities.end(), it->GetID()) != entities.end())
			continue;

		// Add only entities that have layer
		if (!gLayerMgr.EntityHasLayer(*it))
			continue;

		CEGUI::ItemEntry* entityItem = static_cast<CEGUI::ItemEntry*>( gCEGUIWM.createWindow("Editor/ListboxItem", mTree->getName() + "/EntityItem" + StringConverter::ToString(it->GetID())) );
		entityItem->setID(it->GetID());
		entityItem->setUserData(0);
			
		CEGUI::Window* dragContainer = static_cast<CEGUI::ItemEntry*>(gCEGUIWM.createWindow("DragContainer", mTree->getName() + "/DCEntityItem" + StringConverter::ToString(it->GetID())));
		dragContainer->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		entityItem->addChildWindow(dragContainer);

		CEGUI::Window* entityItemText = gCEGUIWM.createWindow("Editor/StaticText", mTree->getName() + "/EntityItemText" + StringConverter::ToString(it->GetID()));
		entityItemText->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
		entityItemText->setText(string(4, ' ') + gEntityMgr.GetEntityName(*it));
		entityItemText->setProperty("FrameEnabled", "False");
		entityItemText->setProperty("BackgroundEnabled", "False");
		entityItemText->setMousePassThroughEnabled(true);

		dragContainer->addChildWindow(entityItemText);
		//dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnDragContainerMouseButtonDown, this));
		//dragContainer->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnDragContainerMouseButtonUp, this));
		//dragContainer->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, CEGUI::Event::Subscriber(&Editor::ResourceWindow::OnDragContainerMouseDoubleClick, this));

		dragContainer->setID(it->GetID());
		//dragContainer->setUserString("DragDataType", "Resource");
		mTree->addChildWindow(entityItem);
	}
	mTree->sortList();
}

bool MySortCallback(const CEGUI::ItemEntry* first , const CEGUI::ItemEntry* second)
{
	bool isFirstEntity = (first->getUserData() == 0);
	bool isSecondEntity = (second->getUserData() == 0);

	EntitySystem::LayerID firstLayerID = isFirstEntity ? gLayerMgr.GetLayerID(gEntityMgr.GetEntity(first->getID())) : first->getID();
	EntitySystem::LayerID secondLayerID = isSecondEntity ? gLayerMgr.GetLayerID(gEntityMgr.GetEntity(second->getID())) : second->getID();

	if (firstLayerID != secondLayerID)
		return firstLayerID < secondLayerID;
	if (!isFirstEntity)
		return 1;
	if (!isSecondEntity)
		return 0;
	return first->getID() < second->getID();
}
