#include "Common.h"

#include "Editor/EditorGUI.h"
#include "Editor/EditorMgr.h"
#include "Editor/EditorMenu.h"
#include "Editor/EntityWindow.h"
#include "Editor/LayerWindow.h"
#include "Editor/ResourceWindow.h"
#include "Editor/PrototypeWindow.h"
#include "Editor/HierarchyWindow.h"
#include "Editor/ValueEditors/PropertyEditorCreator.h"

#include "Core/Game.h"

#include "EntitySystem/EntityMgr/EntityMgr.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"

#include "GUISystem/CEGUICommon.h"
#include "GUISystem/GUIMgr.h"
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
	mGameViewport = static_cast<GUISystem::ViewportWindow*>(gGUIMgr.GetWindow("EditorRoot/TopViewport"));
	mGameViewport->SetDragAndDropCamera(false);

	// Initialize bottom viewport
	mEditorViewport = static_cast<GUISystem::ViewportWindow*>(gGUIMgr.GetWindow("EditorRoot/BottomViewport"));
	mEditorViewport->SetDragAndDropCamera(true);
	mEditorViewport->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&EditorGUI::OnEditorViewportItemDropped, this));

	DisableViewports();
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
	mLayerWindow->Update(delta);
	mResourceWindow->Update(delta);
}

bool Editor::EditorGUI::OnEditorViewportItemDropped( const CEGUI::EventArgs& e)
{
	const CEGUI::DragDropEventArgs& args = static_cast<const CEGUI::DragDropEventArgs&>(e);
	
	if (args.dragDropItem->getUserString("DragDataType") == "Prototype")
	{
		PrototypeWindow* prototypeWindow = static_cast<PrototypeWindow*>(args.dragDropItem->getUserData());
		if (prototypeWindow == 0) return true;
		EntitySystem::EntityHandle sourcePrototype = prototypeWindow->GetItemAtIndex(args.dragDropItem->getID());

		EntitySystem::EntityHandle entity = mHierarchyWindow->InstantiatePrototype(sourcePrototype, EntitySystem::EntityHandle::Null);
		
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
