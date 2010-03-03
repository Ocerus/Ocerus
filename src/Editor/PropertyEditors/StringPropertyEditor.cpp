#include "Common.h"
#include "StringPropertyEditor.h"
#include "GUISystem/CEGUITools.h"

#include "CEGUI.h"

using namespace Editor;

Editor::StringPropertyEditor::StringPropertyEditor(const PropertyHolder& property): AbstractPropertyEditor(property), mEditboxWidget(0)
{
}

CEGUI::Window* Editor::StringPropertyEditor::CreateEditorWidget(CEGUI::Window* parent)
{
	OC_ASSERT(mEditboxWidget == 0);
	mEditboxWidget = static_cast<CEGUI::Editbox*>(gCEGUIWM.createWindow("Editor/Editbox", parent->getName() + "/" + mProperty.GetKey() + "Editbox"));
	mEditboxWidget->setProperty("ReadOnly", mProperty.GetAccessFlags() & Reflection::PA_EDIT_WRITE ? "False" : "True");
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&Editor::StringPropertyEditor::OnEditboxSubmit, this));
	parent->addChildWindow(mEditboxWidget);
	UpdateEditorWidget();
	return mEditboxWidget;
}

void Editor::StringPropertyEditor::SubmitEditorWidget()
{
	string newValue = mEditboxWidget->getText().c_str();
	mProperty.SetValueFromString(newValue);
}

void Editor::StringPropertyEditor::UpdateEditorWidget()
{
	OC_ASSERT(mEditboxWidget != 0);
	mEditboxWidget->setText(mProperty.GetValueString());
	AbstractPropertyEditor::UpdateEditorWidget();
}

uint32 Editor::StringPropertyEditor::GetEditorHeight() const
{
	return (int32)gCEGUIWM.getWindow("EditorRoot")->getFont(true)->getLineSpacing(1.1f) + 10;
}

