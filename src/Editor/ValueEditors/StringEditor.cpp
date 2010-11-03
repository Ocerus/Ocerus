#include "Common.h"
#include "StringEditor.h"
#include "GUISystem/CEGUICommon.h"
#include "Editor/EditorMgr.h"

using namespace Editor;

const eValueEditorType StringEditor::Type(VET_PT_STRING);

StringEditor::~StringEditor()
{
	DestroyModel();
	DeinitWidget();
}

void StringEditor::SetModel(StringEditor::Model* newModel)
{
	if (mEditorWidget == 0)
		InitWidget();

	mModel = newModel;
	SetupWidget(mModel);
}

void StringEditor::DestroyModel()
{
	delete mModel;
	mModel = 0;
}

CEGUI::Window* StringEditor::GetWidget()
{
	OC_DASSERT(mEditorWidget != 0);
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
	mEditboxWidget->setTooltipText(mModel->IsValid() ? this->mModel->GetValue() : "");
}

bool StringEditor::OnEventIsSharedCheckboxChanged(const CEGUI::EventArgs& args)
{
	const CEGUI::WindowEventArgs& winArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
	bool isSelected = static_cast<CEGUI::Checkbox*>(winArgs.window)->isSelected();
	if (mModel->IsShared() != isSelected)
	{
		mModel->SetShared(isSelected);
	}
	return true;
}

bool StringEditor::OnRemoveButtonClicked(const CEGUI::EventArgs& args)
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
			return true;
		default:
			break;
	}
	return false;
}

void Editor::StringEditor::InitWidget()
{
	static unsigned int editorCounter = 0;
	OC_DASSERT(mEditorWidget == 0);
	OC_DASSERT(mEditboxWidget == 0);

	CEGUI::String editorName = "Editor/EntityWindow/ValueEditors/StringEditor" + Utils::StringConverter::ToString(editorCounter++);
	
	// Create main editor widget
	mEditorWidget = gGUIMgr.CreateWindow("DefaultWindow", editorName);
	mEditorWidget->setHeight(CEGUI::UDim(0, GetEditboxHeight()));

	// Create label widget
	CEGUI::Window* labelWidget = CreateLabelWidget(editorName + "/Label");
	mEditorWidget->addChildWindow(labelWidget);

	// Create editbox widget
	mEditboxWidget = gGUIMgr.CreateWindow("Editor/Editbox", editorName + "/Editbox");
	mEditorWidget->addChildWindow(mEditboxWidget);

	// Create remove button widget
	CEGUI::Window* removeButton = gGUIMgr.CreateWindow("Editor/Button", editorName + "/RemoveButton");
	removeButton->setText(TR("entity_editor_remove"));
	removeButton->setSize(CEGUI::UVector2(CEGUI::UDim(0, GetEditboxHeight()), CEGUI::UDim(0, GetEditboxHeight())));
	removeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&StringEditor::OnRemoveButtonClicked, this));
	mEditorWidget->addChildWindow(removeButton);

	// Create is-shared checkbox
	CEGUI::Checkbox* isSharedCheckbox = CreateIsSharedCheckboxWidget(editorName + "/IsSharedCheckbox");
	isSharedCheckbox->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
	isSharedCheckbox->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&StringEditor::OnEventIsSharedCheckboxChanged, this));
	mEditorWidget->addChildWindow(isSharedCheckbox);

	// Create is-locked image
	CEGUI::Window* isLockedWidget = CreateIsLockedImageWidget(editorName + "/IsLockedWidget");
	mEditboxWidget->addChildWindow(isLockedWidget);

	/// Subscribe to editbox events
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&StringEditor::OnEventActivated, this));
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&StringEditor::OnEventDeactivated, this));
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&StringEditor::OnEventKeyDown, this));
}

void Editor::StringEditor::DeinitWidget()
{
	gGUIMgr.DestroyWindow(mEditorWidget);
	mEditorWidget = 0;
}

void StringEditor::SetupWidget(StringEditor::Model* model)
{
	// Set dimensions
	CEGUI::UDim dimLeft = model->IsShareable() ? cegui_absdim(16) : cegui_absdim(0);
	CEGUI::UDim dimMiddle = model->IsListElement() ? cegui_absdim(32) : cegui_reldim(0.5f);
	CEGUI::UDim dimRight = model->IsRemovable() ? CEGUI::UDim(1, - GetEditboxHeight() - 2) : cegui_reldim(1);
	
	// Setup label widget
	CEGUI::Window* labelWidget = mEditorWidget->getChildAtIdx(0);
	labelWidget->setArea(CEGUI::URect(dimLeft, cegui_absdim(0), dimMiddle + cegui_absdim(-2), cegui_reldim(1)));
	labelWidget->setProperty("HorzFormatting", model->IsListElement() ? "RightAligned" : "LeftAligned");
	labelWidget->setText(model->GetName());
	labelWidget->setTooltipText(model->GetTooltip());

	// Setup editbox widget
	mEditboxWidget->setArea(CEGUI::URect(dimMiddle + cegui_absdim(2), cegui_absdim(0), dimRight, cegui_absdim(GetEditboxHeight())));
	mEditboxWidget->setProperty("ReadOnly", model->IsReadOnly() ? "True" : "False");
	AddWidgetToTabNavigation(mEditboxWidget);

	// Setup remove button widget
	CEGUI::Window* removeButton = mEditorWidget->getChildAtIdx(2);
	if (model->IsRemovable())
	{
		removeButton->setVisible(true);
		removeButton->setPosition(CEGUI::UVector2(dimRight + cegui_absdim(2), cegui_absdim(0)));
	}
	else
	{
		removeButton->setVisible(false);
	}

	// Setup is-shared checkbox
	CEGUI::Checkbox* isSharedCheckbox = static_cast<CEGUI::Checkbox*>(mEditorWidget->getChildAtIdx(3));
	if (model->IsShareable())
	{
		isSharedCheckbox->setVisible(true);
		isSharedCheckbox->setPosition(CEGUI::UVector2(cegui_absdim(0), cegui_absdim(0)));
		isSharedCheckbox->setSelected(model->IsShared());
	}
	else
	{
		isSharedCheckbox->setVisible(false);
	}

	// Setup is-locked widget
	CEGUI::Window* isLockedWidget = mEditboxWidget->getChildAtIdx(0);
	isLockedWidget->setVisible(model->IsLocked());
}

CEGUI::Window* Editor::StringEditor::GetEditboxWidget()
{
	OC_DASSERT(mEditboxWidget != 0);
	return mEditboxWidget;
}
