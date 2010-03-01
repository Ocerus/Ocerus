#include "Common.h"
#include "EditorMgr.h"
#include "EditorGUI.h"
#include "GUISystem/GUIMgr.h"

#include "CEGUI.h"



using namespace Editor;

EditorMgr::EditorMgr():
	mEditorGUI(0),
	mCurrentEntity(0)
{
	ocInfo << "*** EditorMgr init ***";
	mEditorGUI = new EditorGUI();
}

EditorMgr::~EditorMgr()
{
	delete mCurrentEntity;
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

void Editor::EditorMgr::SetCurrentEntity(const EntitySystem::EntityHandle* newCurrentEntity)
{
	delete mCurrentEntity;

	if (newCurrentEntity && newCurrentEntity->IsValid())
	{
		mCurrentEntity = new EntitySystem::EntityHandle(*newCurrentEntity);
	}
	else
	{
		mCurrentEntity = 0;
	}

	mEditorGUI->UpdateEntityEditorWindow();
}

void Editor::EditorMgr::UpdateCurrentEntityName(const string& newName)
{
	if (!mCurrentEntity) return;
	gEntityMgr.SetEntityName(*mCurrentEntity, newName);
}

void Editor::EditorMgr::UpdateCurrentEntityProperty(const EntitySystem::ComponentID& componentId, const StringKey propertyKey, const string& newValue)
{
	if (!mCurrentEntity) return;

	PropertyHolder property = gEntityMgr.GetEntityComponentProperty(*mCurrentEntity, componentId, propertyKey);
	property.SetValueFromString(newValue);
}
