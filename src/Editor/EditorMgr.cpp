#include "Common.h"
#include "EditorMgr.h"
#include "EditorGUI.h"
#include "GUISystem/GUIMgr.h"
#include "GUISystem/ViewportWindow.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"
#include "Core/Game.h"
#include "CEGUI.h"

#include <Box2D.h>

using namespace Editor;

const float32 SELECTION_MIN_DISTANCE = 0.2f; ///< Minimum distance of the cursor position for the selection to be considered as multi-selection. The distance is given in pixels!
const float32 EDIT_TOOL_ANGLE_CHANGE_RATIO = 0.3f; ///< How fast the edit tool will change the angle.
const float32 EDIT_TOOL_SCALE_CHANGE_RATIO = 0.1f; ///< How fast the edit tool will change the scale.


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
	mEditTool = ET_MOVE;

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
	if (ke.keyCode == InputSystem::KC_M)
	{
		mEditToolWorking = false;
		mEditTool = ET_MOVE;
		return true;
	}
	else if (ke.keyCode == InputSystem::KC_R)
	{
		mEditToolWorking = false;
		mEditTool = ET_ROTATE;
		return true;
	}
	else if (ke.keyCode == InputSystem::KC_Z)
	{
		mEditToolWorking = false;
		mEditTool = ET_ROTATE_Z;
		return true;
	}
	else if (ke.keyCode == InputSystem::KC_S)
	{
		mEditToolWorking = false;
		mEditTool = ET_SCALE;
		return true;
	}

	Vector2 worldCursorPos;
	if (GetWorldCursorPos(worldCursorPos))
	{
		if (ke.keyCode == InputSystem::KC_LEFT)
		{
			///@TODO camera movement
			return true;
		}
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
			OC_ASSERT(GetCurrentEntity());
			Vector2 delta = worldCursorPos - mEditToolCursorPosition;
			delta.y = -delta.y; // so that the editing tool seems more intuitive
			float32 scalarDelta = delta.x;
			switch (mEditTool)
			{
			case ET_MOVE:
				GetCurrentEntity().GetProperty("Position").SetValue<Vector2>(worldCursorPos - mEditToolRelativeBodyPosition);
				break;
			case ET_ROTATE:
				GetCurrentEntity().GetProperty("Angle").SetValue<float32>(mEditToolBodyAngle + EDIT_TOOL_ANGLE_CHANGE_RATIO * scalarDelta);
				break;
			case ET_ROTATE_Z:
				GetCurrentEntity().GetProperty("ZAngle").SetValue<float32>(mEditToolBodyAngle + EDIT_TOOL_ANGLE_CHANGE_RATIO * scalarDelta);
				break;
			case ET_SCALE:
				GetCurrentEntity().GetProperty("Scale").SetValue<Vector2>(mEditToolBodyScale + EDIT_TOOL_SCALE_CHANGE_RATIO * delta);
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
				mEditToolCursorPosition = worldCursorPos;
				switch (mEditTool)
				{
				case ET_MOVE:
					mEditToolRelativeBodyPosition = worldCursorPos - GetCurrentEntity().GetProperty("Position").GetValue<Vector2>();
					break;
				case ET_ROTATE:
					mEditToolBodyAngle = GetCurrentEntity().GetProperty("Angle").GetValue<float32>();
					break;
				case ET_ROTATE_Z:
					if (gEntityMgr.HasEntityComponentOfType(GetCurrentEntity(), EntitySystem::CT_Model))
					{
						mEditToolBodyAngle = GetCurrentEntity().GetProperty("ZAngle").GetValue<float32>();
					}
					else
					{
						mEditToolWorking = false;
					}
					break;
				case ET_SCALE:
					mEditToolBodyScale = GetCurrentEntity().GetProperty("Scale").GetValue<Vector2>();
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