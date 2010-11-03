#include "Common.h"
#include "PrototypeEditor.h"
#include "Editor/PrototypeWindow.h"
#include "Editor/EditorMgr.h"
#include "GUISystem/CEGUICommon.h"

using namespace Editor;

const eValueEditorType Editor::PrototypeEditor::Type(VET_PT_PROTOTYPE);

void PrototypeEditor::SetModel(PrototypeEditor::Model* newModel)
{
	if (mEditorWidget == 0)
		InitWidget();

	StringEditor::SetModel(newModel);
	mEditboxWidget->setProperty("ReadOnly", "True");
}

void PrototypeEditor::InitWidget()
{
	StringEditor::InitWidget();
	mEditboxWidget->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&Editor::PrototypeEditor::OnEventDragDropItemDropped, this));
}

bool Editor::PrototypeEditor::OnEventDragDropItemDropped(const CEGUI::EventArgs& e)
{
	const CEGUI::DragDropEventArgs& args = static_cast<const CEGUI::DragDropEventArgs&>(e);
	if (args.dragDropItem->getUserString("DragDataType") != "Prototype") { return false; }

	EntitySystem::EntityID prototypeID = args.dragDropItem->getID();
	string prototypeIDString = Utils::StringConverter::ToString(prototypeID);
	mModel->SetValue(prototypeIDString);
	Update();
	return true;
}
