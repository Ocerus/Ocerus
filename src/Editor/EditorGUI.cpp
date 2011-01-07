#include "Common.h"

#include "Editor/EditorGUI.h"
#include "Editor/EditorMgr.h"
#include "Editor/EditorMenu.h"
#include "Editor/EntityWindow.h"
#include "Editor/LayerWindow.h"
#include "Editor/ResourceWindow.h"
#include "Editor/PrototypeWindow.h"
#include "Editor/HierarchyWindow.h"

#include "Core/Game.h"

#include "EntitySystem/EntityMgr/EntityMgr.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"

#include "GUISystem/CEGUICommon.h"
#include "GUISystem/GUIMgr.h"
#include "GUISystem/PopupMgr.h"
#include "GUISystem/TabNavigation.h"
#include "GUISystem/VerticalLayout.h"
#include "GUISystem/ViewportWindow.h"

namespace Editor
{
	const string EditorGUI::EditorCameraName = "EditorCamera";
}

using namespace Editor;

EditorGUI::EditorGUI():
	mGameViewport(0),
	mEditorViewport(0),
	mPopupMenu(0),
	mNewComponentPopupMenu(0),
	mEditorMenu(0),
	mEntityWindow(0),
	mHierarchyWindow(0),
	mLayerWindow(0),
	mPrototypeWindow(0),
	mResourceWindow(0)
{
}

EditorGUI::~EditorGUI()
{
	delete mEditorMenu;
	delete mEntityWindow;
	delete mHierarchyWindow;
	delete mLayerWindow;
	delete mPrototypeWindow;
	delete mResourceWindow;
}

void EditorGUI::Init()
{
	// Load imagesets for Editor
	gGUIMgr.LoadSystemImageset("EditorToolbar.imageset");

	// Load GUI sheet
	CEGUI::Window* editor = gGUIMgr.LoadSystemLayout("Editor.layout");
	if (editor == 0 || !gGUIMgr.SetGUISheet(editor))
	{
		ocError << "Cannot load editor.";
		return;
	}

	CEGUI::System::getSingleton().setDefaultTooltip("Editor/Tooltip");
	editor->setMousePassThroughEnabled(true);

	// Initialize in-game console
	gGUIMgr.InitConsole();

	// Initialize EditorMenu
	mEditorMenu = new EditorMenu();
	mEditorMenu->Init();

	// Initialize EntityWindow
	mEntityWindow = new EntityWindow();
	mEntityWindow->Init();
	
	// Initialize LayerWindow
	mLayerWindow = new LayerWindow();
	mLayerWindow->Init();

	// Initialize ResourceWindow
	mResourceWindow = new ResourceWindow();
	mResourceWindow->Init();

	// Initialize PrototypeWindow
	mPrototypeWindow = new PrototypeWindow();
	mPrototypeWindow->Init();

	// Initialize HiearchyWindow
	mHierarchyWindow = new HierarchyWindow();
	mHierarchyWindow->Init();

	// Initialize top viewport
	mGameViewport = static_cast<GUISystem::ViewportWindow*>(gGUIMgr.GetWindow("Editor/GameViewport"));
	mGameViewport->SetDragAndDropCamera(false);

	// Initialize bottom viewport
	mEditorViewport = static_cast<GUISystem::ViewportWindow*>(gGUIMgr.GetWindow("Editor/EditorViewport"));
	mEditorViewport->SetDragAndDropCamera(true);
	mEditorViewport->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&EditorGUI::OnEditorViewportItemDropped, this));
	mEditorViewport->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&EditorGUI::OnEditorViewportClicked, this));

	// Initialize popup menu
	CreatePopupMenu();
}

void EditorGUI::Deinit()
{
	if (mResourceWindow)
		mResourceWindow->Deinit();
	
	if (mEntityWindow)
		mEntityWindow->Deinit();
	
	if (mHierarchyWindow)
		mHierarchyWindow->Deinit();
	
	if (mLayerWindow)
		mLayerWindow->Deinit();
	
	if (mPrototypeWindow)
		mPrototypeWindow->Deinit();

	DestroyPopupMenu();
}


void EditorGUI::DisableViewports()
{
	mGameViewport->Disable();
	mEditorViewport->Disable();
}

void EditorGUI::RefreshCameras()
{
	EntitySystem::EntityHandle gameCamera = gEntityMgr.FindFirstEntity(Core::Game::GameCameraName);
	if (!gameCamera.Exists())
	{
		// Create the game camera.
		EntitySystem::EntityDescription desc;
		desc.SetName(Core::Game::GameCameraName);
		desc.AddComponent(EntitySystem::CT_Camera);
		gameCamera = gEntityMgr.CreateEntity(desc);
		gameCamera.FinishInit();
	}
	mGameViewport->SetCamera(gameCamera);
	GlobalProperties::Get<Core::Game>("Game").SetRenderTarget(mGameViewport->GetRenderTarget());

	EntitySystem::EntityHandle editorCamera = gEntityMgr.FindFirstEntity(EditorCameraName);
	if (!editorCamera.Exists())
	{
		// Create the editor camera.
		EntitySystem::EntityDescription desc;
		desc.SetName(EditorCameraName);
		desc.AddComponent(EntitySystem::CT_Camera);
		editorCamera = gEntityMgr.CreateEntity(desc);
		editorCamera.FinishInit();
	}
	mEditorViewport->SetCamera(editorCamera);
	mEditorViewport->Activate();
}

void EditorGUI::UpdateGUIWindows()
{
	mLayerWindow->Update();
	mPrototypeWindow->Update();
	mResourceWindow->Update();
}

void EditorGUI::SetSelectedEntity(EntitySystem::EntityHandle entity)
{
	if (gEntityMgr.IsEntityPrototype(entity))
	{
		GetPrototypeWindow()->SetSelectedEntity(entity);
		GetHierarchyWindow()->SetSelectedEntity(EntitySystem::EntityHandle::Null);
		GetLayerWindow()->SetSelectedEntity(EntitySystem::EntityHandle::Null);
	}
	else
	{
		GetPrototypeWindow()->SetSelectedEntity(EntitySystem::EntityHandle::Null);
		GetHierarchyWindow()->SetSelectedEntity(entity);
		GetLayerWindow()->SetSelectedEntity(entity);
	}
	GetEntityWindow()->Rebuild();
}

void EditorGUI::Update(float32 delta)
{
	// Setting the active window
	if (!gInputMgr.IsMouseButtonPressed(InputSystem::MBTN_LEFT) && !gInputMgr.IsMouseButtonPressed(InputSystem::MBTN_RIGHT))
	{
		CEGUI::Window* activeWindow = CEGUI::System::getSingleton().getGUISheet();
		if (!activeWindow || !activeWindow->getActiveChild())
		{
			if (gEditorMgr.IsLockedToGame()) mGameViewport->Activate();
			else mEditorViewport->Activate();
		}
		else
		{
			bool isEditbox = activeWindow->getActiveChild()->getType().compare("Editor/Editbox") == 0;
			bool isPopupMenu = activeWindow->getActiveChild()->getType().compare("Editor/PopupMenu") == 0;
			bool isButton = activeWindow->getActiveChild()->getType().compare("Editor/Button") == 0;
			if (!isEditbox && !isPopupMenu && !isButton)
			{
				if (gEditorMgr.IsLockedToGame() && !mEditorViewport->isCapturedByThis()) mGameViewport->Activate();
				else mEditorViewport->Activate();
			}
		}
	}

	mEntityWindow->Update(delta);

	//gGUIMgr.GetWindow("UserGUI_GameLayout")->invalidate(true);
}

bool Editor::EditorGUI::OnEditorViewportItemDropped( const CEGUI::EventArgs& e)
{
	const CEGUI::DragDropEventArgs& args = static_cast<const CEGUI::DragDropEventArgs&>(e);
	
	if (args.dragDropItem->getUserString("DragDataType") == "Prototype")
	{
		EntitySystem::EntityHandle sourcePrototype = EntitySystem::EntityHandle(args.dragDropItem->getID());
		EntitySystem::EntityHandle entity = gEditorMgr.InstantiatePrototype(sourcePrototype, EntitySystem::EntityHandle::Null);
		
		// move the new entity to the cursor
		if (entity.HasProperty("Position"))
		{
			InputSystem::MouseState& ms = gInputMgr.GetMouseState();
			Vector2 worldCursorPos;
			gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(ms.x, ms.y), worldCursorPos, GetEditorViewport()->GetRenderTarget());
			entity.GetProperty("Position").SetValue(worldCursorPos);
		}

	} 
	return true;
}

bool EditorGUI::OnEditorViewportClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);
	if (args.button == CEGUI::RightButton)
	{
		EntitySystem::EntityHandle hoveredEntity = gEditorMgr.GetHoveredEntity();
		if (hoveredEntity.IsValid())
		{
			mCurrentPopupEntity = hoveredEntity;
			gPopupMgr->ShowPopup(mPopupMenu, args.position.d_x, args.position.d_y, GUISystem::PopupMgr::Callback(this, &Editor::EditorGUI::OnEditorViewportPopupMenuItemClicked));
		}
	}
	return true;
}


void Editor::EditorGUI::CreatePopupMenu()
{

	mNewComponentPopupMenu = gPopupMgr->CreatePopupMenu("Editor/EditorViewport/Popup/NewComponent/AutoPopup");
	for (size_t i = 0; i < EntitySystem::NUM_COMPONENT_TYPES; ++i)
	{
		const string& componentName = EntitySystem::GetComponentTypeName((EntitySystem::eComponentType)i);
		const CEGUI::String& componentMenuItemName = "Editor/EditorViewport/Popup/NewComponent/Component" + Utils::StringConverter::ToString(i);
		mNewComponentPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem(componentMenuItemName, componentName, "", i));
	}

	mPopupMenu = gPopupMgr->CreatePopupMenu("Editor/EditorViewport/Popup");
	CEGUI::Window* newComponentItem = gPopupMgr->CreateMenuItem("Editor/EditorViewport/Popup/NewComponent", TR("new_component"), TR("new_component_hint"));
	newComponentItem->addChildWindow(mNewComponentPopupMenu);
	mPopupMenu->addChildWindow(newComponentItem);
	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("Editor/EditorViewport/Popup/DuplicateEntity", TR("hierarchy_duplicate"), TR("hierarchy_duplicate_hint"), PI_DUPLICATE_ENTITY));
	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("Editor/EditorViewport/Popup/DeleteEntity", TR("hierarchy_delete"), TR("hierarchy_delete_hint"), PI_DELETE_ENTITY));
	mPopupMenu->addChildWindow(gPopupMgr->CreateMenuItem("Editor/EditorViewport/Popup/CreatePrototype", TR("hierarchy_prototype"), TR("hierarchy_prototype_hint"), PI_CREATE_PROTOTYPE));
}

void Editor::EditorGUI::DestroyPopupMenu()
{
	gPopupMgr->DestroyPopupMenu(mPopupMenu);
	mNewComponentPopupMenu = 0;
	mPopupMenu = 0;
}

void Editor::EditorGUI::OnEditorViewportPopupMenuItemClicked(CEGUI::Window* menuItem)
{
	if (menuItem->getParent() == mNewComponentPopupMenu)
	{
		// New component menu item clicked
		gEditorMgr.AddComponentToSelectedEntity((EntitySystem::eComponentType)menuItem->getID());
	}
	else
	{
		switch (menuItem->getID())
		{
		case PI_DUPLICATE_ENTITY:
			gEditorMgr.DuplicateEntity(mCurrentPopupEntity);
			break;
		case PI_DELETE_ENTITY:
			gEditorMgr.DeleteEntity(mCurrentPopupEntity);
			break;
		case PI_CREATE_PROTOTYPE:
			gEditorMgr.CreatePrototypeFromEntity(mCurrentPopupEntity);
			break;
		default:
			OC_NOT_REACHED();
		}
	}
}
