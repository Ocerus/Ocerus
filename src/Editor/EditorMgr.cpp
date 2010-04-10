#include "Common.h"
#include "EditorMgr.h"
#include "EditorGUI.h"
#include "GUISystem/GUIMgr.h"
#include "GUISystem/ViewportWindow.h"

#include "CEGUI.h"



using namespace Editor;

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
	mEditorGUI->Update(delta);
}

void Editor::EditorMgr::Draw(float32 delta)
{
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
	return false;
}

bool Editor::EditorMgr::KeyReleased( const InputSystem::KeyInfo& ke )
{
	return false;
}

bool Editor::EditorMgr::MouseMoved( const InputSystem::MouseInfo& mi )
{
	return false;
}

bool Editor::EditorMgr::MouseButtonPressed( const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn )
{
	return false;
}

bool Editor::EditorMgr::MouseButtonReleased( const InputSystem::MouseInfo& mi, const InputSystem::eMouseButton btn )
{
	return false;
}

bool Editor::EditorMgr::GetWorldCursorPos(Vector2& worldCursorPos) const
{
	InputSystem::MouseState& ms = gInputMgr.GetMouseState();
	GfxSystem::RenderTargetID rt = mEditorGUI->GetEditorViewport()->GetRenderTarget();
	return gGfxRenderer.ConvertScreenToWorldCoords(GfxSystem::Point(ms.x, ms.y), worldCursorPos, rt);
}