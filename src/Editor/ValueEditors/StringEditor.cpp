#include "Common.h"
#include "StringEditor.h"
#include "GUISystem/CEGUITools.h"
#include "Editor/EditorMgr.h"

using namespace Editor;

StringEditor::~StringEditor()
{
	delete mModel;
}

CEGUI::Window* StringEditor::CreateWidget(const CEGUI::String& namePrefix)
{
	PROFILE_FNC();

	OC_DASSERT(mEditorWidget == 0);
	OC_DASSERT(mEditboxWidget == 0);

	/// Create main editor widget
	mEditorWidget = gGUIMgr.CreateWindow("DefaultWindow", true);
	OC_ASSERT(this);
	OC_ASSERT(mEditorWidget);
	mEditorWidget->setHeight(CEGUI::UDim(0, GetEditboxHeight()));

	CEGUI::UDim dimMiddle = mModel->IsListElement() ? CEGUI::UDim(0, 32) : CEGUI::UDim(0.5f, 0);
	CEGUI::UDim dimRight = mModel->IsRemovable() ? CEGUI::UDim(1, - GetEditboxHeight() - 2) : CEGUI::UDim(1, 0);
	
	/// Create label widget of the editor
	CEGUI::Window* labelWidget = this->CreateEditorLabelWidget(namePrefix + "/Label", mModel);
	labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), dimMiddle + CEGUI::UDim(0, -2), CEGUI::UDim(1, 0)));
	mEditorWidget->addChildWindow(labelWidget);

	/// Create editbox widget of the editor
	mEditboxWidget = static_cast<CEGUI::Editbox*>(gGUIMgr.CreateWindow("Editor/Editbox", true));
	mEditboxWidget->setArea(CEGUI::URect(dimMiddle + CEGUI::UDim(0, 2), CEGUI::UDim(0, 0), dimRight, CEGUI::UDim(0, GetEditboxHeight())));
	mEditboxWidget->setProperty("ReadOnly", mModel->IsReadOnly() ? "True" : "False");
	AddWidgetToTabNavigation(mEditboxWidget);
	mEditorWidget->addChildWindow(mEditboxWidget);

	/// Create remove button, if needed
	if (mModel->IsRemovable())
	{
		CEGUI::Window* removeButton = CreateRemoveElementButtonWidget(namePrefix + "/RemoveButton");
		removeButton->setPosition(CEGUI::UVector2(dimRight + CEGUI::UDim(0, 2), CEGUI::UDim(0, 0)));
		removeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&StringEditor::OnEventButtonRemovePressed, this));

		mEditorWidget->addChildWindow(removeButton);
	}

	/// Create isShared checkbox, if needed
	if (mModel->IsShareable())
	{
		labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 16), CEGUI::UDim(0, 0), dimMiddle + CEGUI::UDim(0, -2), CEGUI::UDim(1, 0)));
		CEGUI::Checkbox* isSharedCheckbox = CreateIsSharedCheckboxWidget(namePrefix + "/IsSharedCheckbox");
		isSharedCheckbox->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
		isSharedCheckbox->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&StringEditor::OnEventIsSharedCheckboxChanged, this));
		isSharedCheckbox->setSelected(mModel->IsShared());
		mEditorWidget->addChildWindow(isSharedCheckbox);
	}
	
	/// Subscribe to editbox events
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&StringEditor::OnEventActivated, this));
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&StringEditor::OnEventDeactivated, this));
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&StringEditor::OnEventKeyDown, this));

	/// Update editor and return main widget
	Update();
	return mEditorWidget;
}

void StringEditor::Submit()
{
	OC_DASSERT(mEditboxWidget != 0);
	string newValue = mEditboxWidget->getText().c_str();
	mModel->SetValue(newValue);
}

void StringEditor::Update()
{
	OC_DASSERT(mEditboxWidget != 0);
	if (UpdatesLocked()) return;
	mEditboxWidget->setText(mModel->IsValid() ? this->mModel->GetValue() : "");
}

bool StringEditor::OnEventIsSharedCheckboxChanged(const CEGUI::EventArgs& args)
{
	const CEGUI::WindowEventArgs& winArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
	mModel->SetShared(static_cast<CEGUI::Checkbox*>(winArgs.window)->isSelected());
	return true;
}

bool StringEditor::OnEventButtonRemovePressed(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	mModel->Remove();
	return true;
}

bool StringEditor::OnEventKeyDown(const CEGUI::EventArgs& args)
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
			gEditorMgr.PropertyValueChanged();
			return true;
		default:
			break;
	}
	return false;
}