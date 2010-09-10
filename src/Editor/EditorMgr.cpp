#include "Common.h"
#include "EditorMgr.h"
#include "EditorGUI.h"
#include "EditorMenu.h"
#include "KeyShortcuts.h"
#include "EntityWindow.h"
#include "LayerWindow.h"
#include "ResourceWindow.h"
#include "PrototypeWindow.h"
#include "HierarchyWindow.h"
#include "CreateProjectDialog.h"
#include "GUISystem/GUIMgr.h"
#include "GUISystem/ViewportWindow.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"
#include "Core/Project.h"
#include "Core/Game.h"
#include "CEGUI.h"
#include "GUISystem/FolderSelector.h"
#include "GUISystem/MessageBox.h"
#include "GUISystem/PromptBox.h"

#include <Box2D.h>


using namespace Editor;

const float32 SELECTION_MIN_DISTANCE = 0.2f; ///< Minimum distance of the cursor position for the selection to be considered as multi-selection. The distance is given in pixels!
const float32 EDIT_TOOL_ANGLE_CHANGE_RATIO = 0.3f; ///< How fast the edit tool will change the angle.
const float32 EDIT_TOOL_SCALE_CHANGE_RATIO = 2.0f; ///< How fast the edit tool will change the scale.
const float32 CAMERA_MOVEMENT_SPEED = 10.0f; ///< How fast the camera moves by keys.

EditorMgr::EditorMgr():
	mEditorGUI(0),
	mCurrentProject(0),
	mShortcuts(0),
	mCreateProjectDialog(0)
{
	mShortcuts = new KeyShortcuts();
	mCreateProjectDialog = new CreateProjectDialog();
}

EditorMgr::~EditorMgr()
{
	delete mShortcuts;
	delete mCreateProjectDialog;
	delete mEditorGUI;
	delete mCurrentProject;
}

void EditorMgr::Init()
{
	ocInfo << "*** EditorMgr init ***";

	mEditorGUI = new EditorGUI();
	mCurrentProject = new Core::Project(true);

	mMousePressedInSceneWindow = false;
	mMultiselectStarted = false;
	SetCurrentEditTool(ET_MOVE);
	mHoveredEntity.Invalidate();
	mIsInitialTime = true;

	mEditorGUI->Init();
	GetEditorViewport()->AddInputListener(this);
}

void Editor::EditorMgr::Deinit()
{
	ocInfo << "*** EditorMgr deinit ***";

	mEditorGUI->Deinit();
	GetEditorViewport()->RemoveInputListener(this);

	if (GUISystem::GUIMgr::SingletonExists())
		gGUIMgr.DestroyWindow(gGUIMgr.GetGUISheet());

	delete mEditorGUI;
	mEditorGUI = 0;
	delete mCurrentProject;
	mCurrentProject = 0;
}

void Editor::EditorMgr::Update(const float32 delta)
{
	OC_ASSERT(mEditorGUI);

	if (mEditorGUI->GetEditorViewport()->isVisible())
	{
		// process the move edit tool
		if (mEditToolWorking && mEditTool == ET_MOVE) ProcessCurrentEditTool(GfxSystem::Point(gInputMgr.GetMouseState().x, gInputMgr.GetMouseState().y));


		// is the mouse above the window?
		InputSystem::MouseState& mouse = gInputMgr.GetMouseState();
		GfxSystem::RenderTargetID rt = mEditorGUI->GetEditorViewport()->GetRenderTarget();
		Vector2 worldPosition;
		bool mouseAboveWindow = gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mouse.x, mouse.y), worldPosition, rt);


		// keys camera control
		if (mEditorGUI->GetEditorViewport()->isActive() && !IsLockedToGame())
		{
			HandleHeldShortcuts(delta);
		}


		// pick entity the mouse is hovering over right now
		if (mouseAboveWindow)
		{
			EntityPicker picker(worldPosition, gLayerMgr.GetActiveLayer(), gLayerMgr.GetActiveLayer());
			mHoveredEntity = picker.PickSingleEntity();
		}


		// if some of the selected entities were destroyed, remove them from the selection
		EntitySystem::EntityHandle selectedEntity = GetSelectedEntity();
		if (!mHoveredEntity.Exists()) mHoveredEntity.Invalidate();
		for (EntityList::iterator it=mSelectedEntities.begin(); it!=mSelectedEntities.end();)
		{
			if (!it->Exists())
				it = mSelectedEntities.erase(it);
			else
				++it;
		}
		if (GetSelectedEntity() != selectedEntity && GetSelectedEntity() == selectedEntity) SelectEntity(GetSelectedEntity());

		// update the entities if necessary
		if (!GlobalProperties::Get<Core::Game>("Game").IsActionRunning())
		{
			// synchronize properties before physics
			gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::SYNC_PRE_PHYSICS, Reflection::PropertyFunctionParameters() << 0.0f));

			// destroy entities marked for destruction
			gEntityMgr.ProcessDestroyQueue();
		}

		if (!GetSelectedEntity().Exists()) SelectEntity(EntitySystem::EntityHandle::Null);
	}

	// update the gui elements on the screen
	mEditorGUI->Update(delta);
}

void Editor::EditorMgr::Draw(float32 delta)
{
	// Render game viewport (if visible)
	if (mEditorGUI->GetGameViewport()->isVisible()) GlobalProperties::Get<Core::Game>("Game").Draw(delta);
	
	// Render editor viewport (if visible)
	if (mEditorGUI->GetEditorViewport()->isVisible())
	{
		GfxSystem::RenderTargetID rt = mEditorGUI->GetEditorViewport()->GetRenderTarget();
		gGfxRenderer.SetCurrentRenderTarget(rt);
		gGfxRenderer.ClearCurrentRenderTarget(GfxSystem::Color(0, 0, 0));
		gGfxRenderer.DrawEntities();

		gGfxRenderer.FlushGraphics();

		// draw the physical shape of representation of selected entities
		bool hoveredDrawn = false;
		for (EntityList::iterator it=mSelectedEntities.begin(); it!=mSelectedEntities.end(); ++it)
		{
			if (*it == mHoveredEntity) hoveredDrawn = true;
			DrawEntityPhysicalShape(*it, GfxSystem::Color(0,100,250,200), GfxSystem::Color(255,0,0,200), 1.0f);
		}
		if (!hoveredDrawn && mHoveredEntity.IsValid())
		{
			DrawEntityPhysicalShape(mHoveredEntity, GfxSystem::Color(250,250,250,150), GfxSystem::Color(0,0,0,0), 3.5f);
		}

		// draw the multi-selection stuff
		if (mMultiselectStarted)
		{
			InputSystem::MouseState& mouse = gInputMgr.GetMouseState();
			Vector2 worldCursorPos;
			if (gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mouse.x, mouse.y), worldCursorPos, rt))
			{
				float32 minDistance = SELECTION_MIN_DISTANCE / gGfxRenderer.GetRenderTargetCameraZoom(rt);
				if ((worldCursorPos-mSelectionCursorPosition).LengthSquared() >= MathUtils::Sqr(minDistance))
				{
					float32 rotation = gGfxRenderer.GetRenderTargetCameraRotation(rt);
					gGfxRenderer.DrawRect(mSelectionCursorPosition, worldCursorPos, rotation, GfxSystem::Color(255,255,0,150), false);
				}
			}
		}

		gGfxRenderer.FinalizeRenderTarget();
	}
}

void Editor::EditorMgr::SelectEntity(EntitySystem::EntityHandle entity)
{
	EntitySystem::EntityHandle oldEntity = GetSelectedEntity();
	mSelectedEntities.clear();
	if (entity.IsValid())
		mSelectedEntities.push_back(entity);

	if (oldEntity != entity)
		GetEditorGUI()->SetSelectedEntity(entity);
}

void EditorMgr::SelectEntities(const EntitySystem::EntityList& entities)
{
	EntitySystem::EntityHandle oldEntity = GetSelectedEntity();
	mSelectedEntities.clear();
	mSelectedEntities = entities;

	if (oldEntity != GetSelectedEntity())
		GetEditorGUI()->SetSelectedEntity(GetSelectedEntity());
}

void Editor::EditorMgr::ShowCreateEntityPrompt(EntityHandle parent)
{
	GUISystem::PromptBox* prompt = new GUISystem::PromptBox(parent != EntitySystem::EntityHandle::Null ? parent.GetID() : 0);
	prompt->SetText(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "new_entity_prompt"));
	prompt->RegisterCallback(GUISystem::PromptBox::Callback(this, &Editor::EditorMgr::CreateEntityPromptCallback));
	prompt->Show();
}

void Editor::EditorMgr::CreateEntityPromptCallback(bool clickedOK, const string& entityName, int32 parentID)
{
	if (clickedOK == false)
		return;
	EntitySystem::EntityHandle parent = (parentID != 0 ? EntitySystem::EntityHandle(parentID) : EntitySystem::EntityHandle::Null);
	CreateEntity(entityName, parent);
}

void Editor::EditorMgr::CreateEntity(const string& name, EntitySystem::EntityHandle parent)
{
	GetHierarchyWindow()->SetCurrentParent(parent);

	EntitySystem::EntityDescription desc;
	desc.SetName(name.empty() ? TR("new_entity").c_str() : name);
	desc.AddComponent(EntitySystem::CT_Transform);
	EntitySystem::EntityHandle newEntity = gEntityMgr.CreateEntity(desc);
	newEntity.GetProperty("Layer").SetValue(gLayerMgr.GetActiveLayer());
	newEntity.FinishInit();

	GetHierarchyWindow()->SetCurrentParent(EntitySystem::EntityHandle::Null);
}

void Editor::EditorMgr::DuplicateEntity(EntitySystem::EntityHandle entity)
{
	if (!entity.Exists()) return;
	EntityHandle parent = GetHierarchyWindow()->GetParent(entity);
	GetHierarchyWindow()->SetCurrentParent(parent);
	EntityHandle newEntity = gEntityMgr.DuplicateEntity(entity);
	newEntity.FinishInit();
	if (gEntityMgr.IsEntityPrototype(entity))
	{
		gEntityMgr.SavePrototypes();
		UpdatePrototypeWindow();
	}
	SelectEntity(newEntity);
	GetHierarchyWindow()->SetCurrentParent(EntityHandle::Null);
}

void Editor::EditorMgr::DuplicateSelectedEntity()
{
	DuplicateEntity(GetSelectedEntity());
}

void Editor::EditorMgr::DeleteEntity(EntitySystem::EntityHandle entity)
{
	if (!entity.Exists()) return;
	bool isPrototype = gEntityMgr.IsEntityPrototype(entity);
	gEntityMgr.DestroyEntity(entity);
	gEntityMgr.ProcessDestroyQueue();
	if (isPrototype)
	{
		gEntityMgr.SavePrototypes();
		UpdatePrototypeWindow();
	}
}

void Editor::EditorMgr::DeleteSelectedEntity()
{
	DeleteEntity(GetSelectedEntity());
	SelectEntity(EntityHandle::Null);
}

void EditorMgr::CreatePrototypeFromEntity(EntitySystem::EntityHandle entity)
{
	if (!entity.Exists()) return;
	EntitySystem::EntityHandle prototype = gEntityMgr.ExportEntityToPrototype(entity);
	gEntityMgr.SavePrototypes();
	UpdatePrototypeWindow();
	SelectEntity(prototype);
	ClearSelection();
}

void Editor::EditorMgr::CreatePrototypeFromSelectedEntity()
{
	CreatePrototypeFromEntity(GetSelectedEntity());
}

EntitySystem::EntityHandle Editor::EditorMgr::InstantiatePrototype(EntitySystem::EntityHandle prototype, EntitySystem::EntityHandle parent)
{
	GetHierarchyWindow()->SetCurrentParent(parent);

	// instantiate
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

	gEditorMgr.SelectEntity(newEntity);
	return newEntity;
}

void Editor::EditorMgr::DuplicateSelectedEntities()
{
	EntityList tmpVec(mSelectedEntities.size());
	for (EntityList::iterator it = mSelectedEntities.begin(); it != mSelectedEntities.end(); ++it)
	{
		if (it->Exists())
		{
			EntitySystem::EntityHandle h = gEntityMgr.DuplicateEntity(*it);
			h.FinishInit();
			tmpVec.push_back(h);
		}
	}
	mSelectedEntities.assign(tmpVec.begin(), tmpVec.end());
}

void Editor::EditorMgr::DeleteSelectedEntities()
{
	for (EntityList::iterator it = mSelectedEntities.begin(); it != mSelectedEntities.end(); ++it)
	{
		DeleteEntity(*it);
	}
	SelectEntity(EntityHandle::Null);
	mSelectedEntities.clear();
}

void Editor::EditorMgr::AddComponentToEntity(EntitySystem::EntityHandle entity, EntitySystem::eComponentType componentType)
{
	if (!entity.IsValid()) return;
	if (gEntityMgr.IsEntityLinkedToPrototype(entity))
	{
		GUISystem::MessageBox* messageBox = new GUISystem::MessageBox(GUISystem::MessageBox::MBT_OK);
		messageBox->SetText(StringSystem::FormatText(TR("new_component_prototype_error")));
		messageBox->Show();
	}
	else
	{
		gEntityMgr.AddComponentToEntity(entity, componentType);
		GetEntityWindow()->Rebuild();
		if (gEntityMgr.IsEntityPrototype(entity))
			gEntityMgr.SavePrototypes();
	}
}

void Editor::EditorMgr::AddComponentToSelectedEntity(EntitySystem::eComponentType componentType)
{
	AddComponentToEntity(GetSelectedEntity(), componentType);
}

void EditorMgr::RemoveComponentFromEntity(EntitySystem::EntityHandle entity, const EntitySystem::ComponentID& componentId)
{
	if (!entity.IsValid()) return;
	gEntityMgr.DestroyEntityComponent(entity, componentId);
	GetEntityWindow()->Rebuild();
	if (gEntityMgr.IsEntityPrototype(entity))
		gEntityMgr.SavePrototypes();
}


void Editor::EditorMgr::RemoveComponentFromSelectedEntity(const EntitySystem::ComponentID& componentId)
{
	RemoveComponentFromEntity(GetSelectedEntity(), componentId);
}

void Editor::EditorMgr::ResumeAction()
{
	Core::Game& game = GlobalProperties::Get<Core::Game>("Game");
	if (mIsInitialTime)
	{
		game.SaveAction();
		mIsInitialTime = false;
	}
	game.ResumeAction();
	ocInfo << "Game action resumed";
}

void Editor::EditorMgr::PauseAction()
{
	if (GlobalProperties::Get<Core::Game>("Game").IsActionRunning())
	{
		if (gProfiler.IsRunning())
		{
			gProfiler.Stop();
			gProfiler.DumpIntoConsole();
		}

		GlobalProperties::Get<Core::Game>("Game").PauseAction();
		ocInfo << "Game action paused";
	}
}

void Editor::EditorMgr::RestartAction()
{
	if (!mIsInitialTime)
	{
		if (gProfiler.IsRunning())
		{
			gProfiler.Stop();
			gProfiler.DumpIntoConsole();
		}

		if (!GetCurrentProject()->IsSceneOpened()) return;
		
		GlobalProperties::Get<Core::Game>("Game").RestartAction();
		mIsInitialTime = true;
		if (!GetSelectedEntity().Exists())
		{
			SelectEntity(EntityHandle::Null);
		}
		else
		{
			GetEntityWindow()->Rebuild();
		}
		ocInfo << "Game action restarted";
	}
}

void Editor::EditorMgr::Reset()
{
	SelectEntity(EntityHandle::Null);
	ClearSelection();
	mIsInitialTime = true;
}

void Editor::EditorMgr::SetCurrentEditTool(eEditTool newEditTool)
{
	mEditToolWorking = false;
	mEditTool = newEditTool;
}

void Editor::EditorMgr::SwitchEditTool( eEditTool newTool )
{
	mEditorGUI->GetEditorMenu()->SwitchToolButton((uint32)newTool);
}

void Editor::EditorMgr::SwitchActionTool( eActionTool newTool )
{
	mEditorGUI->GetEditorMenu()->SwitchActionButton((uint32)newTool);
}

void Editor::EditorMgr::UpdateResourceWindow()
{
	mEditorGUI->GetResourceWindow()->Update();
}

void Editor::EditorMgr::UpdateSceneMenu()
{
	mEditorGUI->GetEditorMenu()->UpdateSceneMenu();
}

void EditorMgr::UpdateMenuItemsEnabled()
{
	mEditorGUI->GetEditorMenu()->UpdateItemsEnabled();
}

void EditorMgr::ShowCreateProjectDialog()
{
	mCreateProjectDialog->Show();
}

void EditorMgr::CreateProject(const string& projectPath)
{
	if (!mCurrentProject->CreateProject(projectPath))
	{
		GUISystem::MessageBox* messageBox = new GUISystem::MessageBox(GUISystem::MessageBox::MBT_OK);
		messageBox->SetText(StringSystem::FormatText(gStringMgrSystem.GetTextData
			(GUISystem::GUIMgr::GUIGroup, "create_project_error")) << projectPath);
		messageBox->Show();
	}
}

void EditorMgr::ShowOpenProjectDialog()
{
	GUISystem::FolderSelector* folderSelector = new GUISystem::FolderSelector("", (int)EditorMenu::FST_OPENPROJECT);
	folderSelector->RegisterCallback(GUISystem::FolderSelector::Callback(mEditorGUI->GetEditorMenu(), &Editor::EditorMenu::OnFolderSelected));
	folderSelector->Show(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "open_project_folder"));
}

void EditorMgr::OpenProject(const string& projectPath)
{
	if (!mCurrentProject->OpenProject(projectPath))
	{
		GUISystem::MessageBox* messageBox = new GUISystem::MessageBox(GUISystem::MessageBox::MBT_OK);
		messageBox->SetText(StringSystem::FormatText(gStringMgrSystem.GetTextData
			(GUISystem::GUIMgr::GUIGroup, "open_project_error")) << projectPath);
		messageBox->Show();
	}
}

void EditorMgr::CloseProject()
{
	mCurrentProject->CloseProject();
}

void EditorMgr::SaveOpenedScene()
{
	if (mCurrentProject->IsSceneOpened() && !IsLockedToGame())
	{
		mCurrentProject->SaveOpenedScene();
	}
}

void EditorMgr::ShowNewSceneDialog()
{
	const string& projectPath = gEditorMgr.GetCurrentProject()->GetAbsoluteOpenedProjectPath();
	GUISystem::FolderSelector* folderSelector = new GUISystem::FolderSelector(projectPath, (int)EditorMenu::FST_NEWSCENE);
	folderSelector->RegisterCallback(GUISystem::FolderSelector::Callback(mEditorGUI->GetEditorMenu(), &Editor::EditorMenu::OnFolderSelected));
	folderSelector->Show(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "new_scene_folder"), true, 
	  gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "new_scene_filename"));
}

void EditorMgr::CreateScene(const string& sceneFilename, const string& sceneName)
{
	if (!mCurrentProject->CreateScene(sceneFilename, sceneName))
	{
		GUISystem::MessageBox* messageBox = new GUISystem::MessageBox(GUISystem::MessageBox::MBT_OK);
		messageBox->SetText(StringSystem::FormatText(gStringMgrSystem.GetTextData
			(GUISystem::GUIMgr::GUIGroup, "new_scene_error")) << sceneFilename);
		messageBox->Show();
	}
}

void EditorMgr::ShowQuitDialog()
{
	GUISystem::MessageBox* messageBox = new GUISystem::MessageBox(GUISystem::MessageBox::MBT_YES_NO, EditorMenu::MBT_QUIT);
	messageBox->SetText(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "quit_message_text"));
	messageBox->RegisterCallback(GUISystem::MessageBox::Callback(mEditorGUI->GetEditorMenu(), &Editor::EditorMenu::OnMessageBoxClicked));
	messageBox->Show();
}

bool Editor::EditorMgr::KeyPressed( const InputSystem::KeyInfo& ke )
{
	bool result = false;

	if (mEditorGUI->GetEditorViewport()->isCapturedByThis())
	{
		result = GlobalProperties::Get<Core::Game>("Game").KeyPressed(ke);
	}

	return result || HandleShortcuts(ke.keyCode);
}

bool Editor::EditorMgr::KeyReleased( const InputSystem::KeyInfo& ke )
{
	bool result = false;

	if (mEditorGUI->GetEditorViewport()->isCapturedByThis())
	{
		result = GlobalProperties::Get<Core::Game>("Game").KeyReleased(ke);
	}

	if (IsLockedToGame()) return result;

	return result;
}

bool Editor::EditorMgr::MouseMoved( const InputSystem::MouseInfo& mi )
{
	if (mEditToolWorking)
	{
		ProcessCurrentEditTool(GfxSystem::Point(mi.x, mi.y));
		return true;
	}

	return false;
}

bool Editor::EditorMgr::MouseButtonPressed( const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn )
{
	OC_ASSERT(mEditorGUI);
	mEditorGUI->GetEditorViewport()->activate();
	mEditorGUI->GetEditorViewport()->captureInput();

	Vector2 worldCursorPos;
	if (!gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mi.x, mi.y), worldCursorPos, mEditorGUI->GetEditorViewport()->GetRenderTarget()))
	{
		// the cursor is out of the window
		return false;
	}

	mMousePressedInSceneWindow = true;

	if (btn == InputSystem::MBTN_LEFT && (gInputMgr.IsKeyDown(InputSystem::KC_LSHIFT) || gInputMgr.IsKeyDown(InputSystem::KC_RSHIFT)))
	{
		mMultiselectStarted = true;
		mSelectionCursorPosition = worldCursorPos;
		return true;
	}
	else
	{
		if (mHoveredEntity.IsValid())
		{
			if ((btn == InputSystem::MBTN_LEFT && !IsEntitySelected(mHoveredEntity)) || btn == InputSystem::MBTN_RIGHT)
			{
				// If user clicked right mouse button, cancel previous selection by select the hovering entity
				// (popups work only on one entity). If user clicked left mouse button and entity is not selected,
				// cancel previous selection by select the hovering entity.
				SelectEntity(mHoveredEntity);
			}
		}

		if (btn == InputSystem::MBTN_LEFT)
		{
			// we've got an entity to edit, so let's init the right tool
			mEditToolWorking = true;
			mEditToolCursorPosition = worldCursorPos;
			switch (mEditTool)
			{
			case ET_MOVE:
				mEditToolBodyPositions.resize(mSelectedEntities.size());
				for (size_t i=0; i<mSelectedEntities.size(); ++i)
				{
					mEditToolBodyPositions[i] = mSelectedEntities[i].GetProperty("Position").GetValue<Vector2>();
				}
				break;
			case ET_ROTATE:
				mEditToolBodyAngles.resize(mSelectedEntities.size());
				mEditToolBodyPositions.resize(mSelectedEntities.size());
				for (size_t i=0; i<mSelectedEntities.size(); ++i)
				{
					mEditToolBodyAngles[i] = mSelectedEntities[i].GetProperty("Angle").GetValue<float32>();
					mEditToolBodyPositions[i] = mSelectedEntities[i].GetProperty("Position").GetValue<Vector2>();
				}
				break;
			case ET_ROTATE_Y:
				mEditToolBodyAngles.resize(mSelectedEntities.size());
				for (size_t i=0; i<mSelectedEntities.size(); ++i)
				{
					if (!gEntityMgr.HasEntityComponentOfType(mSelectedEntities[i], EntitySystem::CT_Model))
					{
						mEditToolWorking = false;
						break;
					}
					mEditToolBodyAngles[i] = mSelectedEntities[i].GetProperty("YAngle").GetValue<float32>();
				}
				break;
			case ET_SCALE:
				mEditToolBodyScales.resize(mSelectedEntities.size());
				for (size_t i=0; i<mSelectedEntities.size(); ++i)
				{
					mEditToolBodyScales[i] = mSelectedEntities[i].GetProperty("Scale").GetValue<Vector2>();
				}
				break;
			}
		}
	}

	return false;
}

bool Editor::EditorMgr::MouseButtonReleased( const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn )
{
	OC_UNUSED(mi);
	OC_UNUSED(btn);

	OC_ASSERT(mEditorGUI);
	mEditorGUI->GetEditorViewport()->releaseInput();

	bool editToolWasWorking = mEditToolWorking;
	mEditToolWorking = false;
	bool mouseWasPressedInSceneWindow = mMousePressedInSceneWindow;
	mMousePressedInSceneWindow = false;

	Vector2 worldCursorPos;
	if (!GetWorldCursorPos(worldCursorPos))
	{
		// we're not in the current viewport
		return false;
	}

	if (!mouseWasPressedInSceneWindow)
	{
		// the mouse press started somewhere outside the window
		return false;
	}

	if (editToolWasWorking)
	{
		// the edit tool was working, so we don't want any entity to be selected or the selection to be canceled
		return false;
	}

	if (btn == InputSystem::MBTN_MIDDLE)
	{
		// ignore the middle mouse button for entity selection
		return false;
	}

	// now select the entity the mouse is at
	GfxSystem::RenderTargetID rt = mEditorGUI->GetEditorViewport()->GetRenderTarget();

	if (mMultiselectStarted)
	{
		mMultiselectStarted = false;
		bool multiSelection = false;
		float32 minDistance = SELECTION_MIN_DISTANCE / gGfxRenderer.GetRenderTargetCameraZoom(rt);
		if ((worldCursorPos-mSelectionCursorPosition).LengthSquared() >= MathUtils::Sqr(minDistance))
		{
			// the cursor moved far away enough
			multiSelection = true;
		}

		if (multiSelection)
		{
			// selection of multiple entities in a rectangle
			mSelectedEntities.clear();
			EntitySystem::EntityPicker picker(mSelectionCursorPosition, gLayerMgr.GetActiveLayer(), gLayerMgr.GetActiveLayer());
			float32 cameraRotation = gGfxRenderer.GetRenderTargetCameraRotation(rt);
			EntitySystem::EntityList entities;
			if (picker.PickMultipleEntities(worldCursorPos, cameraRotation, entities))
			{
				SelectEntities(entities);
			}
			else
			{
				SelectEntity(EntitySystem::EntityHandle::Null);
			}		
		}
		else if (mHoveredEntity.IsValid())
		{
			// selection of a single entity under the cursor
			SelectEntity(mHoveredEntity);
		}
		else
		{
			// nothing is selected
			SelectEntity(EntitySystem::EntityHandle::Null);
		}

		return true;
	}
	else
	{
		if (!mHoveredEntity.IsValid())
		{
			// nothing is under the cursor, clear selection
			ClearSelection();
		}
	}

	return false;
}

bool Editor::EditorMgr::GetWorldCursorPos(Vector2& worldCursorPos) const
{
	InputSystem::MouseState& ms = gInputMgr.GetMouseState();
	GfxSystem::RenderTargetID rt = mEditorGUI->GetEditorViewport()->GetRenderTarget();
	return gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(ms.x, ms.y), worldCursorPos, rt);
}

bool Editor::EditorMgr::DrawEntityPhysicalShape( const EntitySystem::EntityHandle entity, const GfxSystem::Color shapeColor, const GfxSystem::Color fillColor, const float32 shapeWidth )
{
	EntitySystem::ComponentIdList components;
	gEntityMgr.GetEntityComponentsOfType(entity, EntitySystem::CT_PolygonCollider, components);
	bool noCollider = components.empty();
	if (noCollider) gEntityMgr.GetEntityComponentsOfType(entity, EntitySystem::CT_Transform, components);
	bool result = false;

	for (EntitySystem::ComponentIdList::iterator it=components.begin(); it!=components.end(); ++it)
	{
		PhysicalShape* shape = 0;
		if (noCollider)	shape = gEntityMgr.GetEntityComponentProperty(entity, *it, "PhysicalShapeDummy").GetValue<PhysicalShape*>();
		else shape = gEntityMgr.GetEntityComponentProperty(entity, *it, "PhysicalShape").GetValue<PhysicalShape*>();
		if (!shape) continue;

		XForm xf = shape->GetBody()->GetTransform();
		b2PolygonShape* poly = (b2PolygonShape*)shape->GetShape();
		int32 vertexCount = poly->m_vertexCount;
		OC_ASSERT(vertexCount <= b2_maxPolygonVertices);
		Vector2 vertices[b2_maxPolygonVertices];

		Vector2 centre = Vector2_Zero;
		for (int32 i = 0; i < vertexCount; ++i)
		{
			vertices[i] = MathUtils::Multiply(xf, poly->m_vertices[i]);
			centre += vertices[i];
		}
		centre *= 1.0f / vertexCount;

		gGfxRenderer.DrawPolygon(vertices, vertexCount, shapeColor, false, shapeWidth);
		const float32 squareRadius = 0.3f;
		gGfxRenderer.DrawLine(centre + Vector2(-squareRadius, -squareRadius), centre + Vector2(squareRadius, -squareRadius), fillColor, shapeWidth);
		gGfxRenderer.DrawLine(centre + Vector2(squareRadius, -squareRadius), centre + Vector2(squareRadius, squareRadius), fillColor, shapeWidth);
		gGfxRenderer.DrawLine(centre + Vector2(squareRadius, squareRadius), centre + Vector2(-squareRadius, squareRadius), fillColor, shapeWidth);
		gGfxRenderer.DrawLine(centre + Vector2(-squareRadius, squareRadius), centre + Vector2(-squareRadius, -squareRadius), fillColor, shapeWidth);
		result = true;
	}

	return result;
}

bool Editor::EditorMgr::IsEditingPrototype() const
{
	OC_ASSERT(mEditorGUI);
	return gEntityMgr.IsEntityPrototype(GetSelectedEntity());
}

void EditorMgr::UpdateHierarchyWindow()
{
	OC_ASSERT(mEditorGUI);
	GetHierarchyWindow()->Update();
}


void Editor::EditorMgr::UpdatePrototypeWindow()
{
	OC_ASSERT(mEditorGUI);
	GetPrototypeWindow()->Update();
}

void Editor::EditorMgr::PropertyValueChanged()
{
	if (gEditorMgr.IsEditingPrototype())
	{
		gEntityMgr.UpdatePrototypeInstances(GetSelectedEntity());
		gEntityMgr.SavePrototypes();
	}
}

bool Editor::EditorMgr::IsLockedToGame() const
{
	return GlobalProperties::Get<Core::Game>("Game").IsActionRunning();
}

bool Editor::EditorMgr::HandleShortcuts( InputSystem::eKeyCode keyCode )
{
	mShortcuts->KeyPressed(keyCode);

	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_START_ACTION) && !IsLockedToGame())
	{
		SwitchActionTool(AT_RUN);
		return true;
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_RESTART_ACTION) && IsLockedToGame())
	{
		SwitchActionTool(AT_RESTART);
		return true;
	}
	
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_PLAY_PAUSE) && IsLockedToGame())
	{
		SwitchActionTool(AT_PAUSE);
		return true;
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_PLAY_PAUSE) && !IsLockedToGame())
	{
		SwitchActionTool(AT_RUN);
		return true;
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_PROFILER) && IsLockedToGame())
	{
		if (gProfiler.IsRunning())
		{
			gProfiler.Stop();
			gProfiler.DumpIntoConsole();
		}
		else
		{
			gProfiler.Start(false);
		}

		return true;
	}


	if (IsLockedToGame()) return false;


	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_TOOL_MOVE))
	{
		SwitchEditTool(ET_MOVE);
		return true;
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_TOOL_ROTATE))
	{
		SwitchEditTool(ET_ROTATE);
		return true;
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_TOOL_ROTATE_Y))
	{
		SwitchEditTool(ET_ROTATE_Y);
		return true;
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_TOOL_SCALE))
	{
		SwitchEditTool(ET_SCALE);
		return true;
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_CREATE_SCENE))
	{
		ShowNewSceneDialog();
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_OPEN_PROJECT))
	{
		ShowOpenProjectDialog();
	}	
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_SAVE_SCENE))
	{
		SaveOpenedScene();
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_DUPLICATE))
	{
		DuplicateSelectedEntities();
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_DELETE))
	{
		DeleteSelectedEntities();
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_QUIT))
	{
		ShowQuitDialog();
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_FULLSCREEN))
	{
		gGfxWindow.SwitchFullscreen();
	}

	return false;
}

void Editor::EditorMgr::HandleHeldShortcuts( float32 delta )
{
	mShortcuts->ShortcutsHeld();
	
	EntitySystem::EntityHandle camera = mEditorGUI->GetEditorViewport()->GetCamera();
	OC_ASSERT(camera.IsValid());
	float32 movementSpeed = delta * CAMERA_MOVEMENT_SPEED / camera.GetProperty("Zoom").GetValue<float32>();
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_CAM_LEFT))
	{
		Vector2 cameraPos = camera.GetProperty("Position").GetValue<Vector2>();
		camera.GetProperty("Position").SetValue<Vector2>(cameraPos + Vector2(-movementSpeed, 0));
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_CAM_RIGHT))
	{
		Vector2 cameraPos = camera.GetProperty("Position").GetValue<Vector2>();
		camera.GetProperty("Position").SetValue<Vector2>(cameraPos + Vector2(movementSpeed, 0));
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_CAM_UP))
	{
		Vector2 cameraPos = camera.GetProperty("Position").GetValue<Vector2>();
		camera.GetProperty("Position").SetValue<Vector2>(cameraPos + Vector2(0, -movementSpeed));
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_CAM_DOWN))
	{
		Vector2 cameraPos = camera.GetProperty("Position").GetValue<Vector2>();
		camera.GetProperty("Position").SetValue<Vector2>(cameraPos + Vector2(0, movementSpeed));
	}
	
}

void Editor::EditorMgr::ProcessCurrentEditTool(const GfxSystem::Point& screenCursorPos)
{
	Vector2 worldCursorPos;
	gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(screenCursorPos.x, screenCursorPos.y), worldCursorPos, mEditorGUI->GetEditorViewport()->GetRenderTarget());
	Vector2 delta = worldCursorPos - mEditToolCursorPosition;
	delta.y = -delta.y; // so that the editing tool seems more intuitive
	float32 scalarDelta = delta.x * delta.x / 2;

	if (scalarDelta > 8) scalarDelta = MathUtils::Abs(delta.x) * 2;

	if (delta.x < 0) scalarDelta *= -1;

	switch (mEditTool)
	{
	case ET_MOVE:
		for (size_t i=0; i<mSelectedEntities.size(); ++i)
		{
			mSelectedEntities[i].GetProperty("Position").SetValue<Vector2>(mEditToolBodyPositions[i] + worldCursorPos - mEditToolCursorPosition);
			if (gEntityMgr.HasEntityComponentOfType(mSelectedEntities[i], EntitySystem::CT_DynamicBody))
			{
				PropertyFunctionParameters params;
				mSelectedEntities[i].GetFunction("ZeroVelocity").CallFunction(params);
			}
		}
		break;
	case ET_ROTATE:
	{
		if (mSelectedEntities.size() == 1)
		{
			mSelectedEntities[0].GetProperty("Angle").SetValue<float32>(mEditToolBodyAngles[0] + EDIT_TOOL_ANGLE_CHANGE_RATIO * scalarDelta);
			break;
		}
		Matrix22 rotationMatrix(EDIT_TOOL_ANGLE_CHANGE_RATIO * scalarDelta);
		Vector2 centerPos(0,0);
		Vector2 fromCenterVec;

		for (size_t i=0; i<mSelectedEntities.size(); ++i)
		{
			centerPos += mEditToolBodyPositions[i];
		}

		centerPos.x = centerPos.x / mSelectedEntities.size();
		centerPos.y = centerPos.y / mSelectedEntities.size();

		for (size_t i=0; i<mSelectedEntities.size(); ++i)
		{
			fromCenterVec =  mEditToolBodyPositions[i] - centerPos;
			fromCenterVec = MathUtils::Multiply(rotationMatrix, fromCenterVec);

			mSelectedEntities[i].GetProperty("Position").SetValue<Vector2>(centerPos + fromCenterVec);
			mSelectedEntities[i].GetProperty("Angle").SetValue<float32>(mEditToolBodyAngles[i] + EDIT_TOOL_ANGLE_CHANGE_RATIO * scalarDelta);
		}
		break;
	}
	case ET_ROTATE_Y:
		for (size_t i=0; i<mSelectedEntities.size(); ++i)
		{
			mSelectedEntities[i].GetProperty("YAngle").SetValue<float32>(mEditToolBodyAngles[i] + EDIT_TOOL_ANGLE_CHANGE_RATIO * scalarDelta);
		}
		break;
	case ET_SCALE:
		if (delta.x < 0)
			delta.x *= -delta.x;
		else
			delta.x *= delta.x;

		if (delta.y < 0)
			delta.y *= -delta.y;
		else
			delta.y *= delta.y;


		delta *= EDIT_TOOL_SCALE_CHANGE_RATIO;
		for (size_t i=0; i<mSelectedEntities.size(); ++i)
		{
			Vector2 transformedDelta = MathUtils::Multiply(Matrix22(mSelectedEntities[i].GetProperty("Angle").GetValue<float32>()), delta);
			mSelectedEntities[i].GetProperty("Scale").SetValue<Vector2>(mEditToolBodyScales[i] + transformedDelta);
		}
		break;
	}
}

bool Editor::EditorMgr::IsEntitySelected( const EntitySystem::EntityHandle entity ) const
{
	return std::find(mSelectedEntities.begin(), mSelectedEntities.end(), entity) != mSelectedEntities.end();
}

bool Editor::EditorMgr::IsProjectOpened() const
{ 
	return mCurrentProject->IsProjectOpened();
}

void EditorMgr::OnProjectOpened()
{
	GetPrototypeWindow()->Update();
	GetResourceWindow()->Update();
	UpdateSceneMenu();
	UpdateMenuItemsEnabled();
}

void EditorMgr::OnProjectClosed()
{
	GetPrototypeWindow()->Update();
	GetResourceWindow()->Update();
	UpdateSceneMenu();
	UpdateMenuItemsEnabled();
}

void EditorMgr::OnSceneOpened()
{
	UpdateMenuItemsEnabled();
	GetLayerWindow()->Update();
}

void EditorMgr::OnSceneClosed()
{
	GetEditorGUI()->DisableViewports();
	GetEntityWindow()->Clear();
	GetHierarchyWindow()->Clear();
	GetLayerWindow()->Clear();
	SwitchActionTool(Editor::EditorMgr::AT_RESTART);
	UpdateMenuItemsEnabled();
}

GUISystem::ViewportWindow* EditorMgr::GetEditorViewport() const
{
	return mEditorGUI ? mEditorGUI->GetEditorViewport() : 0;
}

GUISystem::ViewportWindow* EditorMgr::GetGameViewport() const
{
	return mEditorGUI ? mEditorGUI->GetGameViewport() : 0;
}

EditorMenu* EditorMgr::GetEditorMenu() const
{
	return mEditorGUI ? mEditorGUI->GetEditorMenu() : 0;
}

EntityWindow* EditorMgr::GetEntityWindow() const
{
	return mEditorGUI ? mEditorGUI->GetEntityWindow() : 0;
}

HierarchyWindow* EditorMgr::GetHierarchyWindow() const
{
	return mEditorGUI ? mEditorGUI->GetHierarchyWindow() : 0;
}

LayerWindow* EditorMgr::GetLayerWindow() const
{
	return mEditorGUI ? mEditorGUI->GetLayerWindow() : 0;
}

PrototypeWindow* EditorMgr::GetPrototypeWindow() const
{
	return mEditorGUI ? mEditorGUI->GetPrototypeWindow() : 0;
}

ResourceWindow* EditorMgr::GetResourceWindow() const
{
	return mEditorGUI ? mEditorGUI->GetResourceWindow() : 0;
}

