#include "Common.h"
#include "EditorMgr.h"
#include "EditorGUI.h"
#include "GUISystem/GUIMgr.h"
#include "GUISystem/ViewportWindow.h"

#include "CEGUI.h"

using namespace Editor;

const float32 SELECTION_MIN_DISTANCE = 10.0f; ///< Minimum distance of the cursor position for the selection to be considered as multi-selection. The distance is given in pixels!


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
	mSelectionStarted = false;
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

	// draw the multi-selection stuff
	if (mSelectionStarted)
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
	return false;
}

bool Editor::EditorMgr::MouseButtonPressed( const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn )
{
	if (btn == InputSystem::MBTN_LEFT)
	{
		mSelectionStarted = gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(mi.x, mi.y), mSelectionCursorPosition, mEditorGUI->GetEditorViewport()->GetRenderTarget());
		return true;
	}

	return false;
}

bool Editor::EditorMgr::MouseButtonReleased( const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn )
{
	OC_UNUSED(mi);
	OC_UNUSED(btn);

	if (!mSelectionStarted)
	{
		// the mouse was not previously pressed probably
		return false;
	}
	mSelectionStarted = false;

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
			SetCurrentEntity(mSelectedEntities[0]);
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
		SetCurrentEntity(mHoveredEntity);
	}
	else
	{
		// nothing is selected
		mSelectedEntities.clear();
		SetCurrentEntity(EntityHandle::Null);
	}

	return true;
}

bool Editor::EditorMgr::GetWorldCursorPos(Vector2& worldCursorPos) const
{
	InputSystem::MouseState& ms = gInputMgr.GetMouseState();
	GfxSystem::RenderTargetID rt = mEditorGUI->GetEditorViewport()->GetRenderTarget();
	return gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(ms.x, ms.y), worldCursorPos, rt);
}