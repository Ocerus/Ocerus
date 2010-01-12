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


/// DOCASNE
bool EditorMgr::EntityPickerEvent(const CEGUI::EventArgs& e)
{
	string entityName = CEGUI::WindowManager::getSingleton().getWindow("picker/edit")->getText().c_str();
	EntitySystem::EntityHandle entity = gEntityMgr.FindFirstEntity(entityName);
	SetCurrentEntity(&entity);
	return true;
}

void EditorMgr::LoadEditor()
{
	gGUIMgr.LoadRootLayout("Editor.layout");

	/// DOCASNE
	CEGUI::FrameWindow* picker = (CEGUI::FrameWindow*)CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/FrameWindow", "picker");
	picker->setText("EntityPicker");
	picker->setSize(CEGUI::UVector2(CEGUI::UDim(0, 200), CEGUI::UDim(0, 50)));
	picker->setPosition(CEGUI::UVector2(CEGUI::UDim(1, -210), CEGUI::UDim(1, -80)));
	CEGUI::WindowManager::getSingleton().getWindow("root")->addChildWindow(picker);

	CEGUI::Editbox* pickerInput = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Editbox", "picker/edit");
	picker->addChildWindow(pickerInput);
	pickerInput->setArea(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0));

	pickerInput->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&EditorMgr::EntityPickerEvent, this));
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
