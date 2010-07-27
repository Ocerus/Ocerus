#include "Common.h"
#include "Editor/LayerMgrWidget.h"
#include "Editor/EditorMgr.h"
#include "GUISystem/CEGUITools.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"

using namespace Editor;

namespace Editor
{
	const char* const IMAGES_FILE_NAME          = "TaharezLook";
	const char* const TEXT_SELECTION_BRUSH_NAME = "TextSelectionBrush";
}

Editor::LayerMgrWidget::LayerMgrWidget(CEGUI::Window* layerMgrWindow):
		mLayerMgrWindow(layerMgrWindow), mNewLayerEditbox(0), mUpButton(0), mDownButton(0),
		mTreeWindow(0), mUpdateTimer(0)
{
}

void Editor::LayerMgrWidget::Init()
{
	CEGUI::String prefix = mLayerMgrWindow->getName();
	mTreeWindow = static_cast<CEGUI::Tree*>(gGUIMgr.GetWindow(prefix + "/Tree"));

	mNewLayerEditbox = gGUIMgr.GetWindow(prefix + "/EditNewLayer");
	mNewLayerEditbox->subscribeEvent(CEGUI::Editbox::EventKeyDown,
			CEGUI::SubscriberSlot(&Editor::LayerMgrWidget::OnEditNewLayerKeyDown, this));

	CEGUI::Window* btnToggleLayerVisibility = gGUIMgr.GetWindow(prefix + "/ButtonToggleLayerVisibility");
	btnToggleLayerVisibility->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(&Editor::LayerMgrWidget::OnButtonToggleLayerVisibilityClicked, this));

	CEGUI::Window* btnAddLayer = gGUIMgr.GetWindow(prefix + "/ButtonAddLayer");
	btnAddLayer->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(&Editor::LayerMgrWidget::OnButtonAddLayerClicked, this));

	CEGUI::Window* btnEditEntity = gGUIMgr.GetWindow(prefix + "/ButtonEditEntity");
	btnEditEntity->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(&Editor::LayerMgrWidget::OnButtonEditEntityClicked, this));

	CEGUI::Window* btnSetActiveLayer = gGUIMgr.GetWindow(prefix + "/ButtonSetActiveLayer");
	btnSetActiveLayer->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(&Editor::LayerMgrWidget::OnButtonSetActiveLayerClicked, this));

	mUpButton = gGUIMgr.GetWindow(prefix + "/ButtonUp");
	mUpButton->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(&Editor::LayerMgrWidget::OnButtonUpDownClicked, this));

	mDownButton = gGUIMgr.GetWindow(prefix + "/ButtonDown");
	mDownButton->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::SubscriberSlot(&Editor::LayerMgrWidget::OnButtonUpDownClicked, this));
}

void Editor::LayerMgrWidget::Update(float32 delta)
{
	mUpdateTimer += delta;
	if (mUpdateTimer > 1)
	{
		mUpdateTimer = 0;
		UpdateTree();
	}
}

bool Editor::LayerMgrWidget::OnEditNewLayerKeyDown(const CEGUI::EventArgs& args)
{
	const CEGUI::KeyEventArgs& keyArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
	if (keyArgs.scancode == CEGUI::Key::Return)
		return OnButtonAddLayerClicked(args);
	return false;
}

bool Editor::LayerMgrWidget::OnButtonSetActiveLayerClicked(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	CEGUI::TreeItem* currentItem = mTreeWindow->getFirstSelectedItem();
	if (currentItem == 0 || currentItem->getUserData() != 0) return true;
	EntitySystem::LayerID layerId = currentItem->getID();
	gLayerMgr.SetActiveLayer(layerId);
	UpdateTree();
	return true;
}

bool Editor::LayerMgrWidget::OnButtonAddLayerClicked(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	const CEGUI::String& newLayerName = mNewLayerEditbox->getText();
	gLayerMgr.AddBottomLayer(newLayerName.c_str());
	mNewLayerEditbox->setText("");
	UpdateTree();
	return true;
}

bool Editor::LayerMgrWidget::OnButtonUpDownClicked(const CEGUI::EventArgs& args)
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

bool Editor::LayerMgrWidget::OnButtonEditEntityClicked(const CEGUI::EventArgs&)
{
	CEGUI::TreeItem* currentItem = mTreeWindow->getFirstSelectedItem();
	if (currentItem == 0 || currentItem->getUserData() == 0) return true;
	EntitySystem::EntityHandle entity = gEntityMgr.GetEntity(currentItem->getID());
	gEditorMgr.SetCurrentEntity(entity);
	return true;
}

bool Editor::LayerMgrWidget::OnButtonToggleLayerVisibilityClicked(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	CEGUI::TreeItem* currentItem = mTreeWindow->getFirstSelectedItem();
	if (currentItem == 0 || currentItem->getUserData() != 0) return true;
	EntitySystem::LayerID layerId = currentItem->getID();
	gLayerMgr.ToggleLayerVisible(layerId);
	UpdateTree();
	return true;
}


void Editor::LayerMgrWidget::UpdateTree()
{
	// Save information about selected item
	CEGUI::TreeItem* selectedItem = mTreeWindow->getFirstSelectedItem();
	CEGUI::String selectedLayerName;
	EntitySystem::EntityID selectedEntityId = 0;
	bool isLayerSelected = false;

	if (selectedItem)
	{
		selectedLayerName = selectedItem->getText();
		selectedEntityId = selectedItem->getID();
		isLayerSelected = selectedItem->getUserData() == 0;
	}

	// Reset and rebuild tree
	mTreeWindow->resetList(); // this must be rewriten using gGUIMgr.DestroyWindow()
	selectedItem = 0;

	for (EntitySystem::LayerID layerID = gLayerMgr.GetBottomLayerID(); layerID <= gLayerMgr.GetTopLayerID(); ++layerID)
	{
		string layerName = gLayerMgr.GetLayerName(layerID);
		if (gLayerMgr.GetActiveLayer() == layerID)
			layerName = "* " + layerName;

		layerName += (gLayerMgr.IsLayerVisible(layerID)) ? " (+)" : " (-)";

		CEGUI::TreeItem* layerItem = new CEGUI::TreeItem(layerName);
		layerItem->setSelectionBrushImage(IMAGES_FILE_NAME, TEXT_SELECTION_BRUSH_NAME);
		layerItem->setID(layerID);
		layerItem->setUserData(0);

		if (isLayerSelected && selectedLayerName == layerItem->getText())
			selectedItem = layerItem;

		mTreeWindow->addItem(layerItem);

		EntitySystem::EntityList entityList;
		gLayerMgr.GetEntitiesFromLayer(layerID, entityList);
		for (EntitySystem::EntityList::const_iterator it = entityList.begin(); it != entityList.end(); ++it)
		{
			CEGUI::TreeItem* entityItem = new CEGUI::TreeItem(gEntityMgr.GetEntityName(*it));
			entityItem->setSelectionBrushImage(IMAGES_FILE_NAME, TEXT_SELECTION_BRUSH_NAME);
			entityItem->setUserData((void*)1);
			entityItem->setID(it->GetID());
			layerItem->addItem(entityItem);

			if (!isLayerSelected && selectedEntityId == it->GetID())
				selectedItem = entityItem;

		}
		layerItem->toggleIsOpen();
	}

	if (selectedItem)
		mTreeWindow->setItemSelectState(selectedItem, true);
}
