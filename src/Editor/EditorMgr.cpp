#include "Common.h"
#include "EditorMgr.h"
#include "EditorGUI.h"
#include "PopupMenu.h"
#include "ResourceWindow.h"
#include "PrototypeWindow.h"
#include "GUISystem/GUIMgr.h"
#include "GUISystem/ViewportWindow.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"
#include "Core/Project.h"
#include "Core/Game.h"
#include "CEGUI.h"

#include <Box2D.h>

using namespace Editor;

const float32 SELECTION_MIN_DISTANCE = 0.2f; ///< Minimum distance of the cursor position for the selection to be considered as multi-selection. The distance is given in pixels!
const float32 EDIT_TOOL_ANGLE_CHANGE_RATIO = 0.3f; ///< How fast the edit tool will change the angle.
const float32 EDIT_TOOL_SCALE_CHANGE_RATIO = 0.1f; ///< How fast the edit tool will change the scale.
const float32 CAMERA_MOVEMENT_SPEED = 0.2f; ///< How fast the camera moves by keys.


EditorMgr::EditorMgr():
	mEditorGUI(0),
	mCurrentProject(0),
	mCurrentEntity(EntityHandle::Null)
{
	ocInfo << "*** EditorMgr init ***";
}

EditorMgr::~EditorMgr()
{

}

void EditorMgr::LoadEditor()
{
	mEditorGUI = new EditorGUI();
	mCurrentProject = new Core::Project();

	mMousePressedInSceneWindow = false;
	mPopupClosingEnabled = false;
	mMultiselectStarted = false;
	SetCurrentEditTool(ET_MOVE);
	mHoveredEntity.Invalidate();
	mIsInitialTime = true;

	mEditorGUI->LoadGUI();
	mEditorGUI->GetEditorViewport()->AddInputListener(this);
}

void Editor::EditorMgr::UnloadEditor()
{
	if (mEditorGUI) mEditorGUI->GetEditorViewport()->RemoveInputListener(this);
	gGUIMgr.UnloadRootLayout();

	delete mEditorGUI;
	mEditorGUI = 0;
	delete mCurrentProject;
	mCurrentProject = 0;
}

void Editor::EditorMgr::Update(const float32 delta)
{
	// is the mouse above the window?
	InputSystem::MouseState& mouse = gInputMgr.GetMouseState();
	GfxSystem::RenderTargetID rt = mEditorGUI->GetEditorViewport()->GetRenderTarget();
	Vector2 worldPosition;
	bool mouseAboveWindow = gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mouse.x, mouse.y), worldPosition, rt);


	// keys camera control
	if (mouseAboveWindow)
	{
		EntitySystem::EntityHandle camera = mEditorGUI->GetEditorViewport()->GetCamera();
		OC_ASSERT(camera);

		float32 movementSpeed = CAMERA_MOVEMENT_SPEED / camera.GetProperty("Zoom").GetValue<float32>();
		if (gInputMgr.IsKeyDown(InputSystem::KC_LEFT))
		{
			Vector2 cameraPos = camera.GetProperty("Position").GetValue<Vector2>();
			camera.GetProperty("Position").SetValue<Vector2>(cameraPos + Vector2(-movementSpeed, 0));
		}
		if (gInputMgr.IsKeyDown(InputSystem::KC_RIGHT))
		{
			Vector2 cameraPos = camera.GetProperty("Position").GetValue<Vector2>();
			camera.GetProperty("Position").SetValue<Vector2>(cameraPos + Vector2(movementSpeed, 0));
		}
		if (gInputMgr.IsKeyDown(InputSystem::KC_UP))
		{
			Vector2 cameraPos = camera.GetProperty("Position").GetValue<Vector2>();
			camera.GetProperty("Position").SetValue<Vector2>(cameraPos + Vector2(0, -movementSpeed));
		}
		if (gInputMgr.IsKeyDown(InputSystem::KC_DOWN))
		{
			Vector2 cameraPos = camera.GetProperty("Position").GetValue<Vector2>();
			camera.GetProperty("Position").SetValue<Vector2>(cameraPos + Vector2(0, movementSpeed));
		}
	}


	// pick entity the mouse is hovering over right now
	if (mouseAboveWindow)
	{
		EntityPicker picker(worldPosition);
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
	if (GetSelectedEntity() != selectedEntity && GetCurrentEntity() == selectedEntity) SetCurrentEntity(GetSelectedEntity());

	// update the entities if necessary
	if (!GlobalProperties::Get<Core::Game>("Game").IsActionRunning())
	{
		// synchronize properties before physics
		gEntityMgr.BroadcastMessage(EntityMessage(EntityMessage::SYNC_PRE_PHYSICS, Reflection::PropertyFunctionParameters() << 0.0f));

		// destroy entities marked for destruction
		gEntityMgr.ProcessDestroyQueue();
	}

	if (!GetCurrentEntity().Exists()) SetCurrentEntity(EntitySystem::EntityHandle::Null);

	// update the gui elements on the screen
	mEditorGUI->Update(delta);
}

void Editor::EditorMgr::Draw(float32 delta)
{
	// Render editor viewport (the game viewport is rendered by the Game object)
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
		DrawEntityPhysicalShape(*it, GfxSystem::Color(0,255,0,100), 2.0f);
	}
	if (!hoveredDrawn && mHoveredEntity.IsValid())
	{
		DrawEntityPhysicalShape(mHoveredEntity, GfxSystem::Color(200,200,200,150), 3.5f);
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

	mEditorGUI->Draw(delta);
}

void Editor::EditorMgr::SetCurrentEntity(const EntitySystem::EntityHandle newCurrentEntity)
{
	if (mCurrentEntity == newCurrentEntity) return;
	mCurrentEntity = newCurrentEntity;
	mEditorGUI->UpdateEntityEditorWindow();
}

void Editor::EditorMgr::UpdateCurrentEntityName(const string& newName)
{
	if (!mCurrentEntity.IsValid()) return;

	gEntityMgr.SetEntityName(mCurrentEntity, newName);
}

void Editor::EditorMgr::UpdateCurrentEntityProperty(const EntitySystem::ComponentID& componentId, const StringKey propertyKey, const string& newValue)
{
	if (!mCurrentEntity.IsValid()) return;

	PropertyHolder property = gEntityMgr.GetEntityComponentProperty(mCurrentEntity, componentId, propertyKey);
	property.SetValueFromString(newValue);
}

void Editor::EditorMgr::CreateEntity()
{
	EntitySystem::EntityDescription desc;
	desc.SetName("New Entity");
	desc.AddComponent(EntitySystem::CT_Transform);
	EntityHandle newEntity = gEntityMgr.CreateEntity(desc);
	newEntity.GetProperty("Layer").SetValue(gLayerMgr.GetActiveLayer());
	newEntity.FinishInit();
}

void Editor::EditorMgr::DuplicateCurrentEntity()
{
	if (!mCurrentEntity.Exists()) return;
	EntityHandle newEntity = gEntityMgr.DuplicateEntity(mCurrentEntity);
	newEntity.FinishInit();
	if (IsEditingPrototype())
	{
		gEntityMgr.SavePrototypes();
		RefreshPrototypeWindow();
	}
	SetCurrentEntity(newEntity);
}

void Editor::EditorMgr::DeleteCurrentEntity()
{
	if (!mCurrentEntity.Exists()) return;
	gEntityMgr.DestroyEntity(mCurrentEntity);
	gEntityMgr.ProcessDestroyQueue();
	if (IsEditingPrototype())
	{
		gEntityMgr.SavePrototypes();
		RefreshPrototypeWindow();
	}
	SetCurrentEntity(EntityHandle::Null);
}

void Editor::EditorMgr::DuplicateSelectedEntities()
{
	for (EntityList::iterator it = mSelectedEntities.begin(); it != mSelectedEntities.end(); ++it)
	{
		if (it->Exists())
		{
			gEntityMgr.DuplicateEntity(*it).FinishInit();
		}
	}
}

void Editor::EditorMgr::DeleteSelectedEntities()
{
	for (EntityList::iterator it = mSelectedEntities.begin(); it != mSelectedEntities.end(); ++it)
	{
		if (it->Exists())
		{
			if (GetCurrentEntity() == (*it)) { SetCurrentEntity(EntityHandle::Null); }
			gEntityMgr.DestroyEntity(*it);
		}
	}
	mSelectedEntities.clear();
}

void Editor::EditorMgr::AddComponent(EntitySystem::eComponentType componentType)
{
	if (!mCurrentEntity.IsValid()) return;
	gEntityMgr.AddComponentToEntity(mCurrentEntity, componentType);
	mEditorGUI->UpdateEntityEditorWindow();
	if (IsEditingPrototype()) gEntityMgr.SavePrototypes();
}


void Editor::EditorMgr::RemoveComponent(const EntitySystem::ComponentID& componentId)
{
	if (!mCurrentEntity.IsValid()) return;
	gEntityMgr.DestroyEntityComponent(mCurrentEntity, componentId);
	mEditorGUI->UpdateEntityEditorWindow();
	if (IsEditingPrototype()) gEntityMgr.SavePrototypes();
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
}

void Editor::EditorMgr::PauseAction()
{
	GlobalProperties::Get<Core::Game>("Game").PauseAction();
}

void Editor::EditorMgr::RestartAction()
{
	if (!mIsInitialTime)
	{
		SetCurrentEntity(EntityHandle::Null);
		mSelectedEntities.clear();
		GlobalProperties::Get<Core::Game>("Game").RestartAction();
		mIsInitialTime = true;
	}
}

void Editor::EditorMgr::SetCurrentEditTool(eEditTool newEditTool)
{
	mEditToolWorking = false;
	mEditTool = newEditTool;
}

void Editor::EditorMgr::SwitchEditTool( eEditTool newEditTool )
{
	mEditorGUI->GetEditorMenu()->SwitchToolButton((uint32)newEditTool);
}

void Editor::EditorMgr::RefreshResourceWindow()
{
	ResourceWindow* resourceWindow = mEditorGUI->GetResourceWindow();
	if (resourceWindow) resourceWindow->Refresh();
}

void Editor::EditorMgr::UpdateSceneMenu()
{
	mEditorGUI->GetEditorMenu()->UpdateSceneMenu();
}

void EditorMgr::OpenProject(const string& projectPath)
{
	if (!mCurrentProject->OpenProject(projectPath, true))
	{
		GUISystem::MessageBox* messageBox = new GUISystem::MessageBox(GUISystem::MessageBox::MBT_OK);
		messageBox->SetText("Cannot open project in " + projectPath);
		messageBox->Show();
	}
}

bool Editor::EditorMgr::KeyPressed( const InputSystem::KeyInfo& ke )
{
	if (ke.keyCode == KS_TOOL_MOVE)
	{
		SwitchEditTool(ET_MOVE);
		return true;
	}
	else if (ke.keyCode == KS_TOOL_ROTATE)
	{
		SwitchEditTool(ET_ROTATE);
		return true;
	}
	else if (ke.keyCode == KS_TOOL_ROTATE_Y)
	{
		SwitchEditTool(ET_ROTATE_Y);
		return true;
	}
	else if (ke.keyCode == KS_TOOL_SCALE)
	{
		SwitchEditTool(ET_SCALE);
		return true;
	}

	return false;
}

bool Editor::EditorMgr::KeyReleased( const InputSystem::KeyInfo& ke )
{
	OC_UNUSED(ke);
	return false;
}

bool Editor::EditorMgr::MouseMoved( const InputSystem::MouseInfo& mi )
{
	if (mEditToolWorking)
	{
		Vector2 worldCursorPos;
		if (gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mi.x, mi.y), worldCursorPos, mEditorGUI->GetEditorViewport()->GetRenderTarget()))
		{
			Vector2 delta = worldCursorPos - mEditToolCursorPosition;
			delta.y = -delta.y; // so that the editing tool seems more intuitive
			float32 scalarDelta = delta.x;
			switch (mEditTool)
			{
			case ET_MOVE:
				for (size_t i=0; i<mSelectedEntities.size(); ++i)
				{
					mSelectedEntities[i].GetProperty("Position").SetValue<Vector2>(mEditToolBodyPositions[i] + worldCursorPos - mEditToolCursorPosition);
				}
				break;
			case ET_ROTATE:
				for (size_t i=0; i<mSelectedEntities.size(); ++i)
				{
					mSelectedEntities[i].GetProperty("Angle").SetValue<float32>(mEditToolBodyAngles[i] + EDIT_TOOL_ANGLE_CHANGE_RATIO * scalarDelta);
				}
				break;
			case ET_ROTATE_Y:
				for (size_t i=0; i<mSelectedEntities.size(); ++i)
				{
					mSelectedEntities[i].GetProperty("YAngle").SetValue<float32>(mEditToolBodyAngles[i] + EDIT_TOOL_ANGLE_CHANGE_RATIO * scalarDelta);
				}
				break;
			case ET_SCALE:
				delta *= EDIT_TOOL_SCALE_CHANGE_RATIO;
				for (size_t i=0; i<mSelectedEntities.size(); ++i)
				{
					Vector2 transformedDelta = MathUtils::Multiply(Matrix22(mSelectedEntities[i].GetProperty("Angle").GetValue<float32>()), delta);
					mSelectedEntities[i].GetProperty("Scale").SetValue<Vector2>(mEditToolBodyScales[i] + transformedDelta);
				}
				break;
			}
			return true;
		}				
	}

	return false;
}

bool Editor::EditorMgr::MouseButtonPressed( const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn )
{
	if (btn == InputSystem::MBTN_LEFT)
	{
		Vector2 worldCursorPos;
		if (!gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mi.x, mi.y), worldCursorPos, mEditorGUI->GetEditorViewport()->GetRenderTarget()))
		{
			// the cursor is out of the window
			return false;
		}

		mMousePressedInSceneWindow = true;
		
		if (gInputMgr.IsKeyDown(InputSystem::KC_LSHIFT) || gInputMgr.IsKeyDown(InputSystem::KC_RSHIFT))
		{
			mMultiselectStarted = true;
			mSelectionCursorPosition = worldCursorPos;
			return true;
		}
		else
		{
			if (mHoveredEntity.IsValid())
			{
				if (find(mSelectedEntities.begin(), mSelectedEntities.end(), mHoveredEntity) == mSelectedEntities.end())
				{
					mSelectedEntities.clear();
				}
				if (mSelectedEntities.empty())
				{
					mSelectedEntities.push_back(mHoveredEntity);
					SetCurrentEntity(GetSelectedEntity());
				}
			}

			if (mHoveredEntity.IsValid() && !mSelectedEntities.empty())
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
					for (size_t i=0; i<mSelectedEntities.size(); ++i)
					{
						mEditToolBodyAngles[i] = mSelectedEntities[i].GetProperty("Angle").GetValue<float32>();
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
	}

	return false;
}

bool Editor::EditorMgr::MouseButtonReleased( const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn )
{
	OC_UNUSED(mi);
	OC_UNUSED(btn);

	bool editToolWasWorking = mEditToolWorking;
	mEditToolWorking = false;
	bool mouseWasPressedInSceneWindow = mMousePressedInSceneWindow;
	mMousePressedInSceneWindow = false;

	Vector2 worldCursorPos;
	if (!GetWorldCursorPos(worldCursorPos))
	{
		// we're not in the corrent viewport
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
			EntityPicker picker(mSelectionCursorPosition);
			float32 cameraRotation = gGfxRenderer.GetRenderTargetCameraRotation(rt);
			if (picker.PickMultipleEntities(worldCursorPos, cameraRotation, mSelectedEntities))
			{
				SetCurrentEntity(GetSelectedEntity());
			}
			else
			{
				SetCurrentEntity(EntityHandle::Null);
			}		
		}
		else if (mHoveredEntity.IsValid())
		{
			// selection of a single entity under the cursor
			mSelectedEntities.clear();
			mSelectedEntities.push_back(mHoveredEntity);
			SetCurrentEntity(GetSelectedEntity());
		}
		else
		{
			// nothing is selected
			mSelectedEntities.clear();
			SetCurrentEntity(GetSelectedEntity());
		}

		return true;
	}
	else
	{
		if (!mHoveredEntity.IsValid())
		{
			// nothing is under the cursor
			mSelectedEntities.clear();
			SetCurrentEntity(GetSelectedEntity());
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

bool Editor::EditorMgr::DrawEntityPhysicalShape( const EntitySystem::EntityHandle entity, const GfxSystem::Color shapeColor, const float32 shapeWidth )
{
	EntitySystem::ComponentIdList components;
	gEntityMgr.GetEntityComponentsOfType(entity, EntitySystem::CT_PolygonCollider, components);
	bool result = false;

	for (EntitySystem::ComponentIdList::iterator it=components.begin(); it!=components.end(); ++it)
	{
		PhysicalShape* shape = gEntityMgr.GetEntityComponentProperty(entity, *it, "PhysicalShape").GetValue<PhysicalShape*>();
		OC_ASSERT(shape);

		XForm xf = shape->GetBody()->GetTransform();
		b2PolygonShape* poly = (b2PolygonShape*)shape->GetShape();
		int32 vertexCount = poly->m_vertexCount;
		OC_ASSERT(vertexCount <= b2_maxPolygonVertices);
		Vector2 vertices[b2_maxPolygonVertices];

		for (int32 i = 0; i < vertexCount; ++i)
		{
			vertices[i] = MathUtils::Multiply(xf, poly->m_vertices[i]);
		}

		gGfxRenderer.DrawPolygon(vertices, vertexCount, shapeColor, false, shapeWidth);
		result = true;
	}

	return result;
}

void Editor::EditorMgr::RegisterPopupMenu( PopupMenu* menu )
{
	mPopupMenus.push_back(menu);
}

void Editor::EditorMgr::CloseAllPopupMenus()
{
	if (!mPopupClosingEnabled) return;

	for (list<PopupMenu*>::iterator it=mPopupMenus.begin(); it!=mPopupMenus.end(); ++it)
	{
		(*it)->Close();
	}

	while (!mPopupMenus.empty())
	{
		delete mPopupMenus.front();
	}
}

void Editor::EditorMgr::UnregisterPopupMenu( PopupMenu* menu )
{
	list<PopupMenu*>::iterator it = find(mPopupMenus.begin(), mPopupMenus.end(), menu);
	if (it != mPopupMenus.end())
	{
		mPopupMenus.erase(it);
	}
}

void Editor::EditorMgr::ClearSelection()
{
	mSelectedEntities.clear();
	mHoveredEntity.Invalidate();
}

bool Editor::EditorMgr::IsEditingPrototype() const
{
	OC_ASSERT(mEditorGUI);
	return mEditorGUI->GetPrototypeWindow()->GetSelectedItem() == GetCurrentEntity();
}

void Editor::EditorMgr::RefreshPrototypeWindow()
{
	OC_ASSERT(mEditorGUI);
	mEditorGUI->GetPrototypeWindow()->Refresh();
}

void Editor::EditorMgr::PropertyValueChanged()
{
	if (gEditorMgr.IsEditingPrototype())
	{
		gEntityMgr.UpdatePrototypeInstances(GetCurrentEntity());
		gEntityMgr.SavePrototypes();
	}
}
