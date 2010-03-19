#include "Common.h"
#include "StringEditor.h"
#include "GUISystem/CEGUITools.h"

using namespace Editor;

Editor::StringEditor::~StringEditor()
{
	delete mModel;
}

CEGUI::Window* Editor::StringEditor::CreateWidget(const CEGUI::String& namePrefix)
{
	OC_ASSERT(mEditboxWidget == 0);

	/// Create main editor widget
	CEGUI::Window* widget = gCEGUIWM.createWindow("DefaultWindow", namePrefix);
	widget->setHeight(CEGUI::UDim(0, GetEditboxHeight()));

	/// Create label widget of the editor
	CEGUI::Window* labelWidget = this->CreateEditorLabelWidget(namePrefix + "/Label", mModel);
	if (mModel->IsListElement())
	{
		labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(0, 32), CEGUI::UDim(1, 0)));
	} else {
		labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(0.5f, -2), CEGUI::UDim(1, 0)));
	}
	widget->addChildWindow(labelWidget);

	/// Create editbox widget of the editor
	mEditboxWidget = static_cast<CEGUI::Editbox*>(gCEGUIWM.createWindow("Editor/Editbox", namePrefix + "/Editbox"));
	if (mModel->IsListElement())
	{
		mEditboxWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 36), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	} else {
		mEditboxWidget->setArea(CEGUI::URect(CEGUI::UDim(0.5f, 2), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	}
	mEditboxWidget->setProperty("ReadOnly", mModel->IsReadOnly() ? "True" : "False");
	widget->addChildWindow(mEditboxWidget);

	/// Subscribe to editbox events
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&Editor::StringEditor::OnEventActivated, this));
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&Editor::StringEditor::OnEventDeactivated, this));
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&Editor::StringEditor::OnEventKeyDown, this));

	/// Update editor and return main widget
	Update();
	return widget;
}

void Editor::StringEditor::Submit()
{
	OC_DASSERT(mEditboxWidget != 0);
	string newValue = mEditboxWidget->getText().c_str();
	mModel->SetValue(newValue);
}

void Editor::StringEditor::Update()
{
	OC_DASSERT(mEditboxWidget != 0);
	if (UpdatesLocked()) return;
	mEditboxWidget->setText(mModel->IsValid() ? this->mModel->GetValue() : "");
}

bool Editor::StringEditor::OnEventKeyDown(const CEGUI::EventArgs& args)
{
	OC_DASSERT(mEditboxWidget != 0);
	const CEGUI::KeyEventArgs& keyArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
	switch (keyArgs.scancode)
	{
		case CEGUI::Key::Escape:
			mEditboxWidget->deactivate();
			return true;
		case CEGUI::Key::Return:
		case CEGUI::Key::NumpadEnter:
			this->Submit();
			mEditboxWidget->deactivate();
			return true;
		default:
			break;
	}
	return false;
}