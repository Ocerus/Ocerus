#include "Common.h"
#include "EditorMgr.h"
#include "EditorGUI.h"
#include "GUISystem/GUIMgr.h"
#include "GUISystem/ViewportWindow.h"
#include "Core/Game.h"
#include <Box2D.h>

#include "CEGUI.h"


using namespace Editor;

const float32 SELECTION_MIN_DISTANCE = 0.2f; ///< Minimum distance of the cursor position for the selection to be considered as multi-selection. The distance is given in pixels!


EditorMgr::EditorMgr():
	mEditorGUI(0),
	mCurrentEntity(EntityHandle::Null)
{
	ocInfo << "*** EditorMgr init ***";
	mEditorGUI = new EditorGUI();
}

EditorMgr::~EditorMgr()
{
	delete mEditorGUI;
}

void EditorMgr::LoadEditor()
{
	mMultiselectStarted = false;
	mEditToolWorking = false;
	mHoveredEntity.Invalidate();

	mEditorGUI->LoadGUI();
	gInputMgr.AddInputListener(this);
}

void Editor::EditorMgr::UnloadEditor()
{
	gInputMgr.RemoveInputListener(this);
	gGUIMgr.UnloadRootLayout();
}

void Editor::EditorMgr::Update(const float32 delta)
{
	// pick entity the mouse is hovering over right now
	InputSystem::MouseState& mouse = gInputMgr.GetMouseState();
	GfxSystem::RenderTargetID rt = mEditorGUI->GetEditorViewport()->GetRenderTarget();
	Vector2 worldPosition;
	if (gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mouse.x, mouse.y), worldPosition, rt))
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
	}

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

	// draw the physical representation of all entities
	GlobalProperties::Get<Physics>("Physics").DrawDebugData();

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
	newEntity.FinishInit();
}

void Editor::EditorMgr::DuplicateEntity()
{
	if (!mCurrentEntity.Exists()) return;
	/// @todo duplicate entity
}

void Editor::EditorMgr::DeleteEntity()
{
	if (!mCurrentEntity.Exists()) return;
	gEntityMgr.DestroyEntity(mCurrentEntity);
	SetCurrentEntity(EntityHandle::Null);
}

void Editor::EditorMgr::AddComponent(EntitySystem::eComponentType componentType)
{
	if (!mCurrentEntity.IsValid()) return;
	gEntityMgr.AddComponentToEntity(mCurrentEntity, componentType);
	mEditorGUI->UpdateEntityEditorWindow();
}


void Editor::EditorMgr::RemoveComponent(const EntitySystem::ComponentID& componentId)
{
	if (!mCurrentEntity.IsValid()) return;
	gEntityMgr.DestroyEntityComponent(mCurrentEntity, componentId);
	mEditorGUI->UpdateEntityEditorWindow();
}

void Editor::EditorMgr::ResumeAction()
{
	GlobalProperties::Get<Core::Game>("Game").ResumeAction();
}

void Editor::EditorMgr::PauseAction()
{
	GlobalProperties::Get<Core::Game>("Game").PauseAction();
}

bool Editor::EditorMgr::KeyPressed( const InputSystem::KeyInfo& ke )
{
	OC_UNUSED(ke);
	return false;
}

bool Editor::EditorMgr::KeyReleased( const InputSystem::KeyInfo& ke )
{
	OC_UNUSED(ke);
	return false;
}

bool Editor::EditorMgr::MouseMoved( const InputSystem::MouseInfo& mi )
{
	OC_UNUSED(mi);

	if (mEditToolWorking)
	{
		///@TODO add other tools here
		Vector2 worldCursorPos;
		if (gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mi.x, mi.y), worldCursorPos, mEditorGUI->GetEditorViewport()->GetRenderTarget()))
		{
			OC_ASSERT(GetCurrentEntity());
			GetCurrentEntity().GetProperty("Position").SetValue<Vector2>(worldCursorPos - mEditToolRelativeBodyPosition);
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

		if (gInputMgr.IsKeyDown(InputSystem::KC_RSHIFT) || gInputMgr.IsKeyDown(InputSystem::KC_RSHIFT))
		{
			mMultiselectStarted = true;
			mSelectionCursorPosition = worldCursorPos;
			return true;
		}
		else
		{
			mSelectedEntities.clear();
			if (mHoveredEntity.IsValid()) mSelectedEntities.push_back(mHoveredEntity);
			SetCurrentEntity(GetSelectedEntity());

			if (GetCurrentEntity().IsValid())
			{
				// we've got an entity to edit, so let's init the right tool
				mEditToolWorking = true;
				///@TODO add other tools here
				mEditToolCursorPosition = worldCursorPos;
				mEditToolRelativeBodyPosition = worldCursorPos - GetCurrentEntity().GetProperty("Position").GetValue<Vector2>();
			}
		}
	}

	return false;
}

bool Editor::EditorMgr::MouseButtonReleased( const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn )
{
	OC_UNUSED(mi);
	OC_UNUSED(btn);

	mEditToolWorking = false;

	if (mMultiselectStarted)
	{
		mMultiselectStarted = false;

		Vector2 worldCursorPos;
		if (!GetWorldCursorPos(worldCursorPos))
		{
			// we're not in the corrent viewport
			return false;
		}

		GfxSystem::RenderTargetID rt = mEditorGUI->GetEditorViewport()->GetRenderTarget();

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
			if (picker.PickMultipleEntities(worldCursorPos, cameraRotation, mSelectedEntities) == 1)
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

	return false;
}

bool Editor::EditorMgr::GetWorldCursorPos(Vector2& worldCursorPos) const
{
	InputSystem::MouseState& ms = gInputMgr.GetMouseState();
	GfxSystem::RenderTargetID rt = mEditorGUI->GetEditorViewport()->GetRenderTarget();
	return gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(ms.x, ms.y), worldCursorPos, rt);
}
