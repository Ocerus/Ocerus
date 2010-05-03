#include "Common.h"
#include "StringEditor.h"
#include "GUISystem/CEGUITools.h"
#include "Editor/EditorMgr.h"

using namespace Editor;

Editor::StringEditor::~StringEditor()
{
	delete mModel;
}

CEGUI::Window* Editor::StringEditor::CreateWidget(const CEGUI::String& namePrefix)
{
	PROFILE_FNC();

	OC_DASSERT(mEditorWidget == 0);
	OC_DASSERT(mEditboxWidget == 0);

	/// Create main editor widget
	mEditorWidget = gCEGUIWM.createWindow("DefaultWindow", namePrefix);
	mEditorWidget->setHeight(CEGUI::UDim(0, GetEditboxHeight()));

	CEGUI::UDim dimMiddle = mModel->IsListElement() ? CEGUI::UDim(0, 32) : CEGUI::UDim(0.5f, 0);
	CEGUI::UDim dimRight = mModel->IsRemovable() ? CEGUI::UDim(1, - GetEditboxHeight() - 2) : CEGUI::UDim(1, 0);
	
	/// Create label widget of the editor
	CEGUI::Window* labelWidget = this->CreateEditorLabelWidget(namePrefix + "/Label", mModel);
	labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), dimMiddle + CEGUI::UDim(0, -2), CEGUI::UDim(1, 0)));
	mEditorWidget->addChildWindow(labelWidget);

	/// Create editbox widget of the editor
	mEditboxWidget = static_cast<CEGUI::Editbox*>(gCEGUIWM.createWindow("Editor/Editbox", namePrefix + "/Editbox"));
	mEditboxWidget->setArea(CEGUI::URect(dimMiddle + CEGUI::UDim(0, 2), CEGUI::UDim(0, 0), dimRight, CEGUI::UDim(0, GetEditboxHeight())));
	mEditboxWidget->setProperty("ReadOnly", mModel->IsReadOnly() ? "True" : "False");
	mEditorWidget->addChildWindow(mEditboxWidget);

	/// Create remove button, if needed
	if (mModel->IsRemovable())
	{
		CEGUI::Window* removeButton = CreateRemoveElementButtonWidget(namePrefix + "/RemoveButton");
		removeButton->setPosition(CEGUI::UVector2(dimRight + CEGUI::UDim(0, 2), CEGUI::UDim(0, 0)));
		removeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::StringEditor::OnEventButtonRemovePressed, this));

		mEditorWidget->addChildWindow(removeButton);
	}
	
	/// Subscribe to editbox events
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&Editor::StringEditor::OnEventActivated, this));
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&Editor::StringEditor::OnEventDeactivated, this));
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&Editor::StringEditor::OnEventKeyDown, this));

	/// Update editor and return main widget
	Update();
	return mEditorWidget;
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
bool Editor::StringEditor::OnEventButtonRemovePressed(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	mModel->Remove();
	return true;
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
			if (gEditorMgr.IsEditingPrototype()) gEntityMgr.SavePrototypes();
			return true;
		default:
			break;
	}
	return false;
}