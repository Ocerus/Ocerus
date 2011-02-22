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
#include "Core/Application.h"
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
	GetEditorViewport()->subscribeEvent(CEGUI::Window::EventShown, CEGUI::Event::Subscriber(&EditorMgr::OnEditorViewportActivated, this));
	GetEditorViewport()->subscribeEvent(CEGUI::Window::EventHidden, CEGUI::Event::Subscriber(&EditorMgr::OnEditorViewportDeactivated, this));
	mEditorGUI->DisableViewports();
}

void Editor::EditorMgr::Deinit()
{
	ocInfo << "*** EditorMgr deinit ***";

	mEditorGUI->Deinit();
	GetEditorViewport()->RemoveInputListener(this);

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

void EditorMgr::CenterCameraOnSelectedEntity()
{
	CenterCameraOnEntity(GetSelectedEntity());
}


void Editor::EditorMgr::ShowCreateEntityPrompt(EntityHandle parent)
{
	GUISystem::PromptBox* prompt = new GUISystem::PromptBox(parent != EntitySystem::EntityHandle::Null ? parent.GetID() : 0);
	prompt->SetLabel(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "new_entity_prompt"));
	prompt->RegisterCallback(GUISystem::PromptBox::Callback(this, &Editor::EditorMgr::CreateEntityPromptCallback));
	prompt->Show();
}

void Editor::EditorMgr::ResolveSaveMessageBoxClicked(GUISystem::MessageBox::eMessageBoxButton button, int32 t)
{
	OC_UNUSED(t);
	if (button == GUISystem::MessageBox::MBB_YES)
		SaveOpenedScene();
}

void Editor::EditorMgr::OnCreateProjSaveMessageBoxClicked(GUISystem::MessageBox::eMessageBoxButton button, int32 t)
{
	ResolveSaveMessageBoxClicked(button, t);
	mCreateProjectDialog->Show();
}

void Editor::EditorMgr::OnOpenProjSaveMessageBoxClicked(GUISystem::MessageBox::eMessageBoxButton button, int32 t)
{
	ResolveSaveMessageBoxClicked(button, t);

	GUISystem::FolderSelector* folderSelector = new GUISystem::FolderSelector("", (int)EditorMgr::FST_OPENPROJECT);
	folderSelector->RegisterCallback(GUISystem::FolderSelector::Callback(this, &Editor::EditorMgr::OnFolderSelected));
	folderSelector->Show(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "open_project_folder"));
}

void Editor::EditorMgr::OnCloseProjSaveMessageBoxClicked(GUISystem::MessageBox::eMessageBoxButton button, int32 t)
{
	ResolveSaveMessageBoxClicked(button, t);
	mCurrentProject->CloseProject();
}

void Editor::EditorMgr::OnNewSceneSaveMessageBoxClicked(GUISystem::MessageBox::eMessageBoxButton button, int32 t)
{
	ResolveSaveMessageBoxClicked(button, t);
	
	const string& projectPath = gEditorMgr.GetCurrentProject()->GetAbsoluteOpenedProjectPath();
	GUISystem::FolderSelector* folderSelector = new GUISystem::FolderSelector(projectPath, (int)EditorMgr::FST_NEWSCENE);
	folderSelector->RegisterCallback(GUISystem::FolderSelector::Callback(this, &Editor::EditorMgr::OnFolderSelected));
	folderSelector->Show(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "new_scene_folder"), true, 
	  gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "new_scene_filename"));
}

void Editor::EditorMgr::OnOpenSceneSaveMessageBoxClicked(GUISystem::MessageBox::eMessageBoxButton button, int32 t)
{
	ResolveSaveMessageBoxClicked(button, t);
	
	Reset();
	GetCurrentProject()->OpenSceneAtIndex(mOpeningSceneIndex);
}

void Editor::EditorMgr::OnCloseSceneSaveMessageBoxClicked(GUISystem::MessageBox::eMessageBoxButton button, int32 t)
{
	ResolveSaveMessageBoxClicked(button, t);
	
	if (GetCurrentProject()->IsSceneOpened())
	{
		GetCurrentProject()->CloseOpenedScene();
	}
}

void Editor::EditorMgr::OnQuitSaveMessageBoxClicked(GUISystem::MessageBox::eMessageBoxButton button, int32 t)
{
	OC_UNUSED(t);
	if (button == GUISystem::MessageBox::MBB_YES)
	{
		SaveOpenedScene();
		gApp.Shutdown();
	}
	if (button == GUISystem::MessageBox::MBB_NO)
		gApp.Shutdown();
}

void Editor::EditorMgr::OnQuitMessageBoxClicked(GUISystem::MessageBox::eMessageBoxButton button, int32 t)
{
	OC_UNUSED(t);
	if (button == GUISystem::MessageBox::MBB_YES)
		gApp.Shutdown();
}

void Editor::EditorMgr::OnFolderSelected(const string& path, const string& editboxValue, bool canceled, int32 t)
{
	if (canceled) return;
	eFolderSelectorTags tag = (eFolderSelectorTags)t;
	switch(tag)
	{
	case FST_CREATEPROJECT:
		OC_FAIL("not used; project creation is done in CreateProjectDialog.cpp");
		return;
	case FST_OPENPROJECT:
		Reset();
		OpenProject(path);
		return;
	case FST_DEPLOYPROJECT:
		DeployCurrentProject(mDeployPlatform, path);
		return;
	case FST_NEWSCENE:
		Reset();
		CreateScene(path + '/' + editboxValue, editboxValue);
		return;
	}
	ocWarning << "Folder through FolderSelector with tag " << tag << " selected, but no action defined.";
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
	EntitySystem::EntityHandle newEntity = gEntityMgr.CreateEntity(desc, Editor::HierarchyWindow::ADD_PREPEND);
	newEntity.GetProperty("Layer").SetValue(gLayerMgr.GetActiveLayer());
	newEntity.FinishInit();

	GetHierarchyWindow()->SetCurrentParent(EntitySystem::EntityHandle::Null);
}

EntityHandle Editor::EditorMgr::DuplicateEntity(EntitySystem::EntityHandle entity)
{
	if (!entity.Exists()) return EntityHandle::Null;
	EntityHandle parent = GetHierarchyWindow()->GetParent(entity);
	GetHierarchyWindow()->SetCurrentParent(parent);
	EntityHandle newEntity = gEntityMgr.DuplicateEntity(entity, "", Editor::HierarchyWindow::ADD_PREPEND);
	newEntity.FinishInit();
	if (gEntityMgr.IsEntityPrototype(entity))
	{
		gEntityMgr.SavePrototypes();
		UpdatePrototypeWindow();
	}
	SelectEntity(newEntity);
	GetHierarchyWindow()->SetCurrentParent(EntityHandle::Null);
	return newEntity;
}

EntityHandle Editor::EditorMgr::DuplicateSelectedEntity()
{
	return DuplicateEntity(GetSelectedEntity());
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
	if (mSelectedEntities.size() == 0)
		return;

	EntityList tmpOldSelected = mSelectedEntities;
	EntityList tmpNewSelected;
	for (EntityList::iterator it = tmpOldSelected.begin(); it != tmpOldSelected.end(); ++it)
	{
		if (it->Exists())
		{
			tmpNewSelected.push_back(DuplicateEntity(*it));
		}
	}
	SelectEntity(*tmpNewSelected.begin());
	mSelectedEntities.assign(tmpNewSelected.begin(), tmpNewSelected.end());
}

void Editor::EditorMgr::DeleteSelectedEntities()
{
	EntityList tmpOldSelected = mSelectedEntities;
	for (EntityList::iterator it = tmpOldSelected.begin(); it != tmpOldSelected.end(); ++it)
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

bool EditorMgr::IsActionRunning()
{
	return GlobalProperties::Get<Core::Game>("Game").IsActionRunning();
}

bool Editor::EditorMgr::WasActionRestarted()
{
	return mIsInitialTime && !IsActionRunning();
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

void EditorMgr::UpdateMenuItemsEnabled()
{
	mEditorGUI->GetEditorMenu()->UpdateItemsEnabled();
}

void EditorMgr::ShowSaveMessageBox(GUISystem::MessageBox::Callback callback)
{
	GUISystem::MessageBox* messageBox = new GUISystem::MessageBox(GUISystem::MessageBox::MBT_YES_NO, EditorMgr::MBT_SAVE);
	messageBox->SetText(StringSystem::FormatText(gStringMgrSystem.GetTextData
			(GUISystem::GUIMgr::GUIGroup, "save_message_text")) << mCurrentProject->GetOpenedSceneName());
	messageBox->RegisterCallback(callback);
	messageBox->Show();
}

void EditorMgr::ShowCreateProjectDialog()
{
	if (IsCurrentSceneUnsaved())
	{
		ShowSaveMessageBox(GUISystem::MessageBox::Callback(this, &Editor::EditorMgr::OnCreateProjSaveMessageBoxClicked));
	}
	else
	{
		mCreateProjectDialog->Show();
	}
}

void EditorMgr::CreateProject(const string& name, const string& projectPath)
{
	if (!mCurrentProject->CreateProject(name, projectPath))
	{
		GUISystem::MessageBox* messageBox = new GUISystem::MessageBox(GUISystem::MessageBox::MBT_OK);
		messageBox->SetText(StringSystem::FormatText(gStringMgrSystem.GetTextData
			(GUISystem::GUIMgr::GUIGroup, "create_project_error")) << projectPath);
		messageBox->Show();
	}
}

void EditorMgr::ShowOpenProjectDialog()
{
	if (IsCurrentSceneUnsaved())
	{
		ShowSaveMessageBox(GUISystem::MessageBox::Callback(this, &Editor::EditorMgr::OnOpenProjSaveMessageBoxClicked));
	}
	else
	{
		GUISystem::FolderSelector* folderSelector = new GUISystem::FolderSelector("", (int)EditorMgr::FST_OPENPROJECT);
		folderSelector->RegisterCallback(GUISystem::FolderSelector::Callback(this, &Editor::EditorMgr::OnFolderSelected));
		folderSelector->Show(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "open_project_folder"));
	}
}

void EditorMgr::OpenProject(const string& projectPath)
{
	if (projectPath.empty()) return;

	if (!IsStringValid(projectPath))
	{
		GUISystem::MessageBox* messageBox = new GUISystem::MessageBox(GUISystem::MessageBox::MBT_OK);
		messageBox->SetText(StringSystem::FormatText(gStringMgrSystem.GetTextData
			(GUISystem::GUIMgr::GUIGroup, "open_project_error_wrong_path")) << projectPath);
		messageBox->Show();
	}
	else if (!mCurrentProject->OpenProject(projectPath))
	{
		GUISystem::MessageBox* messageBox = new GUISystem::MessageBox(GUISystem::MessageBox::MBT_OK);
		messageBox->SetText(StringSystem::FormatText(gStringMgrSystem.GetTextData
			(GUISystem::GUIMgr::GUIGroup, "open_project_error")) << projectPath);
		messageBox->Show();
	}
	else
	{
		GlobalProperties::Get<Core::Config>("GlobalConfig").SetString("LastProject", projectPath);
	}
}

void EditorMgr::ShowDeployProjectDialog(const string& platform)
{
	if (!GetCurrentProject() || !GetCurrentProject()->IsProjectOpened())
	{
		GUISystem::ShowMessageBox(TR("error_no_project_opened"));
		return;
	}

	mDeployPlatform = platform;
	GUISystem::FolderSelector* folderSelector = new GUISystem::FolderSelector("", (int)EditorMgr::FST_DEPLOYPROJECT);
	folderSelector->RegisterCallback(GUISystem::FolderSelector::Callback(this, &Editor::EditorMgr::OnFolderSelected));
	folderSelector->Show(TR("deploy_project_folder"));
}

void EditorMgr::DeployCurrentProject(const string& platform, const string& destination)
{
	if (!gApp.CheckDeployDestination(destination))
	{
		GUISystem::ShowMessageBox(TR("deploy_project_bad_destination"));
		return;
	}

	if (gApp.DeployCurrentProject(platform, destination))
	{
		GUISystem::ShowMessageBox(TR("deploy_project_success"));
	}
	else
	{
		GUISystem::ShowMessageBox(TR("deploy_project_fail"));
	}
}

void EditorMgr::CloseProject()
{
	if (IsCurrentSceneUnsaved())
	{
		ShowSaveMessageBox(GUISystem::MessageBox::Callback(this, &Editor::EditorMgr::OnCloseProjSaveMessageBoxClicked));
	}
	else
	{
		mCurrentProject->CloseProject();
	}
}

void EditorMgr::SaveOpenedScene()
{
	if (mCurrentProject->IsSceneOpened() && !IsLockedToGame())
	{
		mCurrentProject->SaveOpenedScene();
		GUISystem::ShowMessageBox(TR("info_scene_saved"));
	}
}

void EditorMgr::ShowNewSceneDialog()
{
	if (IsCurrentSceneUnsaved())
	{
		ShowSaveMessageBox(GUISystem::MessageBox::Callback(this, &Editor::EditorMgr::OnNewSceneSaveMessageBoxClicked));
	}
	else
	{
		const string& projectPath = gEditorMgr.GetCurrentProject()->GetAbsoluteOpenedProjectPath();
		GUISystem::FolderSelector* folderSelector = new GUISystem::FolderSelector(projectPath, (int)EditorMgr::FST_NEWSCENE);
		folderSelector->RegisterCallback(GUISystem::FolderSelector::Callback(this, &Editor::EditorMgr::OnFolderSelected));
		folderSelector->Show(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "new_scene_folder"), true, 
		  gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "new_scene_filename"));
	}

}

void EditorMgr::OpenSceneAtIndex(const int32 index)
{
	if (IsCurrentSceneUnsaved())
	{
		mOpeningSceneIndex = index;
		ShowSaveMessageBox(GUISystem::MessageBox::Callback(this, &Editor::EditorMgr::OnOpenSceneSaveMessageBoxClicked));
	}
	else
	{
		Reset();
		GetCurrentProject()->OpenSceneAtIndex(index);
	}
}

void EditorMgr::CloseScene()
{
	if (IsCurrentSceneUnsaved())
	{
		ShowSaveMessageBox(GUISystem::MessageBox::Callback(this, &Editor::EditorMgr::OnCloseSceneSaveMessageBoxClicked));
	}
	else
	{
		if (GetCurrentProject()->IsSceneOpened())
		{
			GetCurrentProject()->CloseOpenedScene();
		}
	}
}

void EditorMgr::CreateScene(const string& sceneFilename, const string& sceneName)
{
	if (sceneName.empty())
	{
		GUISystem::MessageBox* messageBox = new GUISystem::MessageBox(GUISystem::MessageBox::MBT_OK);
		messageBox->SetText(TR("new_scene_error_empty_name"));
		messageBox->Show();
		return;
	}

	string fixedName = Core::Project::FixSceneName(sceneName);
	string fixedFilename = Core::Project::FixSceneFilename(sceneFilename);

	if (!mCurrentProject->CreateScene(fixedFilename, fixedName))
	{
		GUISystem::MessageBox* messageBox = new GUISystem::MessageBox(GUISystem::MessageBox::MBT_OK);
		messageBox->SetText(StringSystem::FormatText(gStringMgrSystem.GetTextData
			(GUISystem::GUIMgr::GUIGroup, "new_scene_error")) << fixedFilename);
		messageBox->Show();
	}
	else
	{
		GetResourceWindow()->Update();
	}
}

void EditorMgr::ShowQuitDialog()
{
	if (IsCurrentSceneUnsaved())
	{
		GUISystem::MessageBox* messageBox = new GUISystem::MessageBox(GUISystem::MessageBox::MBT_YES_NO_CANCEL, EditorMgr::MBT_QUIT);
		messageBox->SetText(StringSystem::FormatText(gStringMgrSystem.GetTextData
			(GUISystem::GUIMgr::GUIGroup, "save_message_text")) << mCurrentProject->GetOpenedSceneName());
		messageBox->RegisterCallback(GUISystem::MessageBox::Callback(this, &Editor::EditorMgr::OnQuitSaveMessageBoxClicked));
		messageBox->Show();
	}
	else
	{
		GUISystem::MessageBox* messageBox = new GUISystem::MessageBox(GUISystem::MessageBox::MBT_YES_NO, EditorMgr::MBT_QUIT);
		messageBox->SetText(gStringMgrSystem.GetTextData(GUISystem::GUIMgr::GUIGroup, "quit_message_text"));
		messageBox->RegisterCallback(GUISystem::MessageBox::Callback(this, &Editor::EditorMgr::OnQuitMessageBoxClicked));
		messageBox->Show();
	}
}

bool Editor::EditorMgr::KeyPressed( const InputSystem::KeyInfo& ke )
{
	bool result = false;

	if (mEditorGUI->GetEditorViewport()->isCapturedByThis())
	{
		result = GlobalProperties::Get<Core::Game>("Game").KeyPressed(ke);
	}

	if (!result)
	{
		result = HandleShortcuts(ke.keyCode);
	}

	return result;
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
	OC_ASSERT(mEditorGUI);
	if (!mEditorGUI->GetEditorViewport()->isVisible()) return false;

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
	if (!mEditorGUI->GetEditorViewport()->isVisible()) return false;
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
					if (!mSelectedEntities[i].HasProperty("Position"))
					{
						mEditToolWorking = false;
						break;
					}
					mEditToolBodyPositions[i] = mSelectedEntities[i].GetProperty("Position").GetValue<Vector2>();
				}
				break;
			case ET_ROTATE:
				mEditToolBodyAngles.resize(mSelectedEntities.size());
				mEditToolBodyPositions.resize(mSelectedEntities.size());
				for (size_t i=0; i<mSelectedEntities.size(); ++i)
				{
					if (!mSelectedEntities[i].HasProperty("Angle")
						|| !mSelectedEntities[i].HasProperty("Position"))
					{
						mEditToolWorking = false;
						break;
					}
					mEditToolBodyAngles[i] = mSelectedEntities[i].GetProperty("Angle").GetValue<float32>();
					mEditToolBodyPositions[i] = mSelectedEntities[i].GetProperty("Position").GetValue<Vector2>();
				}
				break;
			case ET_ROTATE_Y:
				mEditToolBodyAngles.resize(mSelectedEntities.size());
				for (size_t i=0; i<mSelectedEntities.size(); ++i)
				{
					if (!gEntityMgr.HasEntityComponentOfType(mSelectedEntities[i], EntitySystem::CT_Model)
						|| !mSelectedEntities[i].HasProperty("YAngle"))
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
	if (!mEditorGUI->GetEditorViewport()->isVisible()) return false;

	bool editToolWasWorking = mEditToolWorking;
	mEditToolWorking = false;
	bool mouseWasPressedInSceneWindow = mMousePressedInSceneWindow;
	mMousePressedInSceneWindow = false;

	Vector2 worldCursorPos;
	if (!GetWorldCursorPos(worldCursorPos))
	{
		// we're not in the current viewport
		mMultiselectStarted = false;
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

	// detect if there is actually any data to draw
	bool noCollider = true;
	for (EntitySystem::ComponentIdList::iterator it=components.begin(); it!=components.end(); ++it)
	{
		PhysicalShape* shape = gEntityMgr.GetEntityComponentProperty(entity, *it, "PhysicalShape").GetValue<PhysicalShape*>();
		if (!shape) continue;
		int32 vertexCount = ((b2PolygonShape*)shape->GetShape())->m_vertexCount;
		if (vertexCount > 0) noCollider = false;
	}

	// there's no physical shape data, so we'll just use some dummy rectangle defined in the Transform component
	if (noCollider)
	{
		components.clear();
		gEntityMgr.GetEntityComponentsOfType(entity, EntitySystem::CT_Transform, components);
	}

	// now draw the physical stuff
	bool drawn = false;
	Vector2 centre = Vector2_Zero;
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

		for (int32 i = 0; i < vertexCount; ++i)
		{
			vertices[i] = MathUtils::Multiply(xf, poly->m_vertices[i]);
			centre += vertices[i];
		}
		centre *= 1.0f / vertexCount;

		gGfxRenderer.DrawPolygon(vertices, vertexCount, shapeColor, false, shapeWidth);
		drawn = true;
	}

	// nothing was drawn, so no center was computed; try to use the Transform component
	bool canDrawSelector = drawn;
	if (!drawn)
	{
		if (gEntityMgr.HasEntityProperty(entity, "Position"))
		{
			canDrawSelector = true;
			centre = gEntityMgr.GetEntityProperty(entity, "Position").GetValue<Vector2>();
		}
	}

	// selector square (sometimes the poly is too small to be visible; this makes the entity visible somehow)
	if (canDrawSelector)
	{
		const float32 squareRadius = 0.3f;
		gGfxRenderer.DrawLine(centre + Vector2(-squareRadius, -squareRadius), centre + Vector2(squareRadius, -squareRadius), fillColor, shapeWidth);
		gGfxRenderer.DrawLine(centre + Vector2(squareRadius, -squareRadius), centre + Vector2(squareRadius, squareRadius), fillColor, shapeWidth);
		gGfxRenderer.DrawLine(centre + Vector2(squareRadius, squareRadius), centre + Vector2(-squareRadius, squareRadius), fillColor, shapeWidth);
		gGfxRenderer.DrawLine(centre + Vector2(-squareRadius, squareRadius), centre + Vector2(-squareRadius, -squareRadius), fillColor, shapeWidth);
	}

	return drawn;
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

bool Editor::EditorMgr::IsLockedToGame()
{
	return !WasActionRestarted();
}

bool Editor::EditorMgr::HandleShortcuts( InputSystem::eKeyCode keyCode )
{
	mShortcuts->KeyPressed(keyCode);

	// shortcuts that are active even if the game is running

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
	
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_PLAY_PAUSE) && IsActionRunning())
	{
		SwitchActionTool(AT_PAUSE);
		return true;
	}
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_PLAY_PAUSE) && !IsActionRunning())
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
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_FULLSCREEN))
	{
		gGfxWindow.SwitchFullscreen();
	}


	if (IsLockedToGame()) return false;

	// shortcuts that are NOT active when the game is running 

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
	if (mShortcuts->IsShortcutActive(KeyShortcuts::KS_CENTER_CAM))
	{
		CenterCameraOnSelectedEntity();
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

void Editor::EditorMgr::CenterCameraOnEntity(const EntitySystem::EntityHandle entity)
{
	EntitySystem::EntityHandle camera = mEditorGUI->GetEditorViewport()->GetCamera();
	OC_ASSERT(camera.IsValid());
	if (entity.HasProperty("Position"))
		camera.GetProperty("Position").SetValue<Vector2>(entity.GetProperty("Position").GetValue<Vector2>());
}

ResourceSystem::ResourcePtr EditorMgr::ChangeResourceType(ResourceSystem::ResourcePtr resource, ResourceSystem::eResourceType newType)
{
	GetCurrentProject()->SetResourceType(resource, newType);
	return gResourceMgr.ChangeResourceType(resource, newType);
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
			if (!gEntityMgr.IsEntityPropertyShared(mSelectedEntities[i], "Position"))
			{
				mSelectedEntities[i].GetProperty("Position").SetValue<Vector2>(mEditToolBodyPositions[i] + worldCursorPos - mEditToolCursorPosition);
				if (gEntityMgr.HasEntityComponentOfType(mSelectedEntities[i], EntitySystem::CT_DynamicBody))
				{
					PropertyFunctionParameters params;
					mSelectedEntities[i].GetFunction("ZeroVelocity").CallFunction(params);
				}
			}
		}
		break;
	case ET_ROTATE:
	{
		if (mSelectedEntities.size() == 1)
		{
			if (!gEntityMgr.IsEntityPropertyShared(mSelectedEntities[0], "Angle"))
			{
				mSelectedEntities[0].GetProperty("Angle").SetValue<float32>(mEditToolBodyAngles[0] + EDIT_TOOL_ANGLE_CHANGE_RATIO * scalarDelta);
			}
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

			if (!gEntityMgr.IsEntityPropertyShared(mSelectedEntities[i], "Position"))
			{
				mSelectedEntities[i].GetProperty("Position").SetValue<Vector2>(centerPos + fromCenterVec);
			}
			if (!gEntityMgr.IsEntityPropertyShared(mSelectedEntities[i], "Angle"))
			{
				mSelectedEntities[i].GetProperty("Angle").SetValue<float32>(mEditToolBodyAngles[i] + EDIT_TOOL_ANGLE_CHANGE_RATIO * scalarDelta);
			}
		}
		break;
	}
	case ET_ROTATE_Y:
		for (size_t i=0; i<mSelectedEntities.size(); ++i)
		{
			if (!gEntityMgr.IsEntityPropertyShared(mSelectedEntities[i], "YAngle"))
			{
				mSelectedEntities[i].GetProperty("YAngle").SetValue<float32>(mEditToolBodyAngles[i] + EDIT_TOOL_ANGLE_CHANGE_RATIO * scalarDelta);
			}
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
			float32 angle = mSelectedEntities[i].GetProperty("Angle").GetValue<float32>();

			Vector2 deltax(delta.x, 0);

			Vector2 transformedDeltax = MathUtils::Multiply(Matrix22(angle), deltax);

			transformedDeltax.x = MathUtils::Abs(transformedDeltax.x);
			transformedDeltax.y = MathUtils::Abs(transformedDeltax.y);

			if (delta.x < 0)
				transformedDeltax *= -1;


			Vector2 deltay(0, delta.y);

			Vector2 transformedDeltay = MathUtils::Multiply(Matrix22(angle), deltay);

			transformedDeltay.x = MathUtils::Abs(transformedDeltay.x);
			transformedDeltay.y = MathUtils::Abs(transformedDeltay.y);

			if (delta.y < 0)
				transformedDeltay *= -1;

			Vector2 transformedDelta = transformedDeltay + transformedDeltax;

			if (!gEntityMgr.IsEntityPropertyShared(mSelectedEntities[i], "Scale"))
			{
				mSelectedEntities[i].GetProperty("Scale").SetValue<Vector2>(mEditToolBodyScales[i] + transformedDelta);
			}
		}
		break;
	}
}

void EditorMgr::UnselectEntity(EntitySystem::EntityHandle entity)
{
	EntitySystem::EntityHandle oldEntity = GetSelectedEntity();

	EntitySystem::EntityList::iterator it = std::find(mSelectedEntities.begin(), mSelectedEntities.end(), entity);
	if (it != mSelectedEntities.end())
		mSelectedEntities.erase(it);

	EntitySystem::EntityHandle newEntity = GetSelectedEntity();
	if (oldEntity != newEntity)
		GetEditorGUI()->SetSelectedEntity(newEntity);
}

bool Editor::EditorMgr::IsEntitySelected( const EntitySystem::EntityHandle entity ) const
{
	return std::find(mSelectedEntities.begin(), mSelectedEntities.end(), entity) != mSelectedEntities.end();
}

bool Editor::EditorMgr::IsProjectOpened() const
{ 
	return mCurrentProject->IsProjectOpened();
}

bool Editor::EditorMgr::OnEditorViewportActivated( const CEGUI::EventArgs& )
{
	gInputMgr.RemoveInputListener(this);
	return true;
}

bool Editor::EditorMgr::OnEditorViewportDeactivated( const CEGUI::EventArgs& )
{
	gInputMgr.AddInputListener(this);
	return true;
}

bool Editor::EditorMgr::IsCurrentSceneUnsaved() const
{
	return mCurrentProject && mCurrentProject->IsSceneOpened();
}

void Editor::EditorMgr::PropertyValueSetterFailed( PropertyHolder prop )
{
	if (prop.GetType() == Reflection::PT_RESOURCE)
	{
		GUISystem::ShowMessageBox(TR("error_drag_wrong_resource") + "\"" + prop.GetName() + "\".");
	}
	else if (Reflection::PropertyTypes::IsArray(prop.GetType()))
	{
		GUISystem::ShowMessageBox(TR("error_property_array_save"));
	}
}

void EditorMgr::OnProjectOpened()
{
	GetPrototypeWindow()->Update();
	GetResourceWindow()->Update();
	UpdateMenuItemsEnabled();
}

void EditorMgr::OnProjectClosed()
{
	GetPrototypeWindow()->Update();
	GetResourceWindow()->Clear();
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

void EditorMgr::OnEntityDestroyed(EntitySystem::EntityHandle destroyedEntity)
{
	GetHierarchyWindow()->RemoveEntityFromHierarchy(destroyedEntity);
	UnselectEntity(destroyedEntity);
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

