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
	mCurrentEntity = new EntitySystem::EntityHandle(*newCurrentEntity);
	mEditorGUI->SetCurrentEntity(mCurrentEntity);
}
