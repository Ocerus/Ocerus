#include "Common.h"

#include "Editor/EditorGUI.h"
#include "Editor/EditorMgr.h"
#include "Editor/EditorMenu.h"
#include "Editor/LayerWindow.h"
#include "Editor/ResourceWindow.h"
#include "Editor/PrototypeWindow.h"
#include "Editor/HierarchyWindow.h"
#include "Editor/ValueEditors/PropertyEditorCreator.h"

#include "Core/Game.h"

#include "EntitySystem/EntityMgr/EntityMgr.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"

#include "GUISystem/GUIMgr.h"
#include "GUISystem/CEGUITools.h"
#include "GUISystem/ItemListboxProperties.h"

#include "GUISystem/VerticalLayout.h"
#include "GUISystem/ViewportWindow.h"


namespace Editor
{
	const string ENTITY_EDITOR_NAME = "EditorRoot/EntityEditor/Scrollable";
	const string EditorGUI::EditorCameraName = "EditorCamera";
}

GUISystem::ItemListboxWantsMouseWheel gEditorMouseWheelProperty;


using namespace Editor;

EditorGUI::EditorGUI():
	mPropertyItemHeight(0),
	mComponentGroupHeight(0),
	mPropertyUpdateTimer(0),
	mNeedEntityEditorUpdate(false),
	mEntityEditorLayout(0),
	mGameViewport(0),
	mEditorViewport(0),
	mLayerWindow(0),
	mResourceWindow(0),
	mHierarchyWindow(0)
{
}

EditorGUI::~EditorGUI()
{
	delete mLayerWindow;
	delete mEditorMenu;
	delete mResourceWindow;
	delete mPrototypeWindow;
	delete mHierarchyWindow;
}

void EditorGUI::LoadGUI()
{
	CEGUI_EXCEPTION_BEGIN

	// Load Imagesets for Editor.
	CEGUI::ImagesetManager::getSingleton().create("EditorToolbar.imageset");

	CEGUI_EXCEPTION_END
	
	if (!gGUIMgr.SetGUISheet(gGUIMgr.LoadSystemLayout("Editor.layout")))
	{
		ocError << "Can't load editor.";
		return;
	}
	gGUIMgr.InitConsole();
	CEGUI::System::getSingleton().setDefaultTooltip("Editor/Tooltip");
	mPropertyItemHeight = (int32)gCEGUIWM.getWindow("EditorRoot")->getFont(true)->getLineSpacing(1.1f) + 10;
	mComponentGroupHeight = 28;
	gCEGUIWM.getWindow("EditorRoot")->setMousePassThroughEnabled(true);
	gCEGUIWM.getWindow(ENTITY_EDITOR_NAME)->addProperty(&gEditorMouseWheelProperty);

	// Initialize EditorMenu
	mEditorMenu = new EditorMenu();
	mEditorMenu->Init();

	// Initialize LayerMgrWidget
	mLayerWindow = new LayerWindow();
	mLayerWindow->Init();

	// Initialize resource window
	mResourceWindow = new ResourceWindow();
	mResourceWindow->Init();

	// init prototype window
	mPrototypeWindow = new PrototypeWindow();
	mPrototypeWindow->Init();

	// init hierarchy window
	mHierarchyWindow = new HierarchyWindow();
	mHierarchyWindow->Init();

	// Initialize top viewport
	mGameViewport = static_cast<GUISystem::ViewportWindow*>(gCEGUIWM.getWindow("EditorRoot/TopViewport"));
	mGameViewport->SetDragAndDropCamera(false);

	// Initialize bottom viewport
	mEditorViewport = static_cast<GUISystem::ViewportWindow*>(gCEGUIWM.getWindow("EditorRoot/BottomViewport"));
	mEditorViewport->SetDragAndDropCamera(true);
	mEditorViewport->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&EditorGUI::OnEditorViewportItemDropped, this));

	// Create cameras
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

	mPropertyUpdateTimer += delta;
	if (mPropertyUpdateTimer > 0.3 && gEditorMgr.GetCurrentEntity().IsValid())
	{
		if (!gEditorMgr.GetCurrentEntity().Exists() || mNeedEntityEditorUpdate)
		{
			UpdateEntityEditorWindow();
		}
		else
		{
			mPropertyUpdateTimer = 0;
			for (PropertyEditors::const_iterator it = mPropertyEditors.begin(); it != mPropertyEditors.end(); ++it)
			{
				(*it)->Update();
			}
		}
	}
	mLayerWindow->Update(delta);
}

void EditorGUI::Draw(float32 delta)
{
	OC_UNUSED(delta);	// It may be handy one day
}


void EditorGUI::UpdateEntityEditorWindow()
{
	PROFILE_FNC();

	mNeedEntityEditorUpdate = false;
	EntitySystem::EntityHandle currentEntity = gEditorMgr.GetCurrentEntity();

	CEGUI::ScrollablePane* entityEditorPane = static_cast<CEGUI::ScrollablePane*>(gCEGUIWM.getWindow(ENTITY_EDITOR_NAME));
	float savedScrollPosition = entityEditorPane->getVerticalScrollPosition() * entityEditorPane->getContentPaneArea().getHeight();
	OC_ASSERT(entityEditorPane);

	{
		const CEGUI::Window* entityEditorContentPane = entityEditorPane->getContentPane();
		OC_ASSERT(entityEditorContentPane);

		// Set layout on Entity Editor
		if (!mEntityEditorLayout)
			mEntityEditorLayout = new GUISystem::VerticalLayout(entityEditorPane, entityEditorContentPane);

		// Clear all the content of Entity Editor.
		size_t childCount = entityEditorContentPane->getChildCount();
		for (int i = (childCount - 1); i >= 0; --i)
		{
			gCEGUIWM.destroyWindow(entityEditorContentPane->getChildAtIdx(i));
		}
		
	}

	// Clear all property editors.
	for (PropertyEditors::const_iterator it = mPropertyEditors.begin(); it != mPropertyEditors.end(); ++it)
	{
		delete (*it);
	}
	mPropertyEditors.clear();

	// There is no entity to be selected.
	if (!currentEntity.Exists()) return;

	mEntityEditorLayout->LockUpdates();

	// First "component" is general entity info
	{
		const string namePrefix = ENTITY_EDITOR_NAME + "/ComponentGeneralInfo";
		CEGUI::GroupBox* componentGroup = static_cast<CEGUI::GroupBox*>(gCEGUIWM.createWindow("Editor/GroupBox", namePrefix));
		componentGroup->setText(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "general_info"));
		mEntityEditorLayout->AddChildWindow(componentGroup);
		GUISystem::VerticalLayout* layout = new GUISystem::VerticalLayout(componentGroup, componentGroup->getContentPane(), true);

		{
			AbstractValueEditor* editor = CreateEntityIDEditor(currentEntity);
			mPropertyEditors.push_back(editor);
			layout->AddChildWindow(editor->CreateWidget(namePrefix + "/EntityIdEditor"));
		}
		{
			AbstractValueEditor* editor = CreateEntityNameEditor(currentEntity);
			mPropertyEditors.push_back(editor);
			layout->AddChildWindow(editor->CreateWidget(namePrefix + "/EntityNameEditor"));
		}
		{
			AbstractValueEditor* editor = CreateEntityTagEditor(currentEntity);
			mPropertyEditors.push_back(editor);
			layout->AddChildWindow(editor->CreateWidget(namePrefix + "/EntityTagEditor"));
		}

		layout->UpdateLayout();
	}

	int32 componentCount = EntitySystem::EntityMgr::GetSingleton().GetNumberOfEntityComponents(currentEntity);

	for (int componentID = 0; componentID < componentCount; ++componentID)
	{
		const string namePrefix = ENTITY_EDITOR_NAME + "/Component" + StringConverter::ToString(componentID);
		const string componentName = EntitySystem::GetComponentTypeName(gEntityMgr.GetEntityComponentType(currentEntity, componentID));

		PropertyList propertyList;
		gEntityMgr.GetEntityComponentProperties(currentEntity, componentID, propertyList, Reflection::PA_EDIT_READ);
		
		set<PropertyHolder> sortedPropertyList;
		for (PropertyList::iterator it = propertyList.begin(); it != propertyList.end(); ++it)
		{
		  sortedPropertyList.insert(*it);
		}

		CEGUI::GroupBox* componentGroup = static_cast<CEGUI::GroupBox*>(gCEGUIWM.createWindow("Editor/GroupBox", namePrefix));
		componentGroup->setText(componentName);
		mEntityEditorLayout->AddChildWindow(componentGroup);


		CEGUI::PushButton* removeComponentButton = static_cast<CEGUI::PushButton*>(gCEGUIWM.createWindow("Editor/Button", namePrefix + "/RemoveButton"));
		removeComponentButton->setArea(CEGUI::URect(CEGUI::UDim(1, -8), CEGUI::UDim(0, -24), CEGUI::UDim(1, 12), CEGUI::UDim(0, -4)));
		removeComponentButton->setText("x");
		removeComponentButton->setClippedByParent(false);

		if (gEntityMgr.CanDestroyEntityComponent(currentEntity, componentID))
		{
			removeComponentButton->setTooltipText(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "remove_component_hint"));
			removeComponentButton->setUserString("ComponentID", StringConverter::ToString(componentID));
			removeComponentButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::EditorGUI::OnComponentRemoveClicked, this));
		}
		else
		{
			removeComponentButton->setTooltipText(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "remove_component_error_hint"));
			removeComponentButton->setEnabled(false);
		}

		componentGroup->addChildWindow(removeComponentButton);

		GUISystem::VerticalLayout* layout = new GUISystem::VerticalLayout(componentGroup, componentGroup->getContentPane(), true);
		
		for (set<PropertyHolder>::iterator it = sortedPropertyList.begin(); it != sortedPropertyList.end(); ++it)
		{
			AbstractValueEditor* editor = CreatePropertyEditor(*it);
			mPropertyEditors.push_back(editor);
			layout->AddChildWindow(editor->CreateWidget(namePrefix + "/" + it->GetKey().ToString() + "Editor"));
		}
		layout->UpdateLayout();
	}

	mEntityEditorLayout->UnlockUpdates();
	mEntityEditorLayout->UpdateLayout();

	entityEditorPane->setVerticalScrollPosition(savedScrollPosition / entityEditorPane->getContentPaneArea().getHeight());
	
	/// Nasty hack to solve scrollbar issue
	{
		const CEGUI::UDim height = entityEditorPane->getHeight();
		entityEditorPane->setHeight(height + CEGUI::UDim(0, 1));
		entityEditorPane->setHeight(height);
	}
}

bool Editor::EditorGUI::OnComponentRemoveClicked(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& args = static_cast<const CEGUI::WindowEventArgs&>(e);
	const CEGUI::String& componentIDString = args.window->getUserString("ComponentID");
	EntitySystem::ComponentID componentID = StringConverter::FromString<EntitySystem::ComponentID>(componentIDString.c_str());
	gEditorMgr.RemoveComponent(componentID);
	return true;
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