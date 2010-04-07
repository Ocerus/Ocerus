#include "Common.h"
#include "EditorMgr.h"
#include "EditorGUI.h"
#include "GUISystem/GUIMgr.h"

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
}

void Editor::EditorMgr::UnloadEditor()
{
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
