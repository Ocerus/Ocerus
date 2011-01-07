#include "Common.h"
#include "BoolEditor.h"
#include "GUISystem/CEGUICommon.h"
#include "Editor/EditorMgr.h"

using namespace Editor;

const eValueEditorType Editor::BoolEditor::Type(VET_PT_BOOL);

BoolEditor::~BoolEditor()
{
	DestroyModel();
	DeinitWidget();
}

CEGUI::Window* BoolEditor::GetWidget()
{
	return mEditorWidget;
}

void BoolEditor::SetModel(BoolEditor::Model* newModel)
{
	if (mEditorWidget == 0)
		InitWidget();

	mModel = newModel;
	SetupWidget(mModel);
}

void BoolEditor::DestroyModel()
{
	delete mModel;
	mModel = 0;
}

void BoolEditor::Submit()
{
	OC_DASSERT(mCheckboxWidget != 0);
	bool newValue = mCheckboxWidget->isSelected();
	mModel->SetValue(newValue);
}

void BoolEditor::Update()
{
	OC_DASSERT(mCheckboxWidget != 0);
	if (UpdatesLocked()) return;
	mCheckboxWidget->setSelected(mModel->IsValid() ? mModel->GetValue() : false);
}

void BoolEditor::InitWidget()
{
	static unsigned int editorCounter = 0;
	OC_DASSERT(mEditorWidget == 0);

	CEGUI::String editorName = "Editor/EntityWindow/ValueEditors/BoolEditor" + Utils::StringConverter::ToString(editorCounter++);
	
	// Create main editor widget
	mEditorWidget = gGUIMgr.CreateWindow("DefaultWindow", editorName);
	mEditorWidget->setHeight(CEGUI::UDim(0, GetEditboxHeight()));

	// Create label widget
	CEGUI::Window* labelWidget = CreateLabelWidget(editorName + "/Label");
	mEditorWidget->addChildWindow(labelWidget);

	// Create editbox widget
	mCheckboxWidget = static_cast<CEGUI::Checkbox*>(gGUIMgr.CreateWindow("Editor/Checkbox", editorName + "/Checkbox"));
	mCheckboxWidget->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&BoolEditor::OnEventCheckStateChanged, this));
	mEditorWidget->addChildWindow(mCheckboxWidget);

	// Create remove button widget
	CEGUI::Window* removeButton = CreateRemoveButtonWidget(editorName + "/RemoveButton");
	removeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&BoolEditor::OnRemoveButtonClicked, this));
	mEditorWidget->addChildWindow(removeButton);

	// Create is-shared checkbox
	CEGUI::Checkbox* isSharedCheckbox = CreateIsSharedCheckboxWidget(editorName + "/IsSharedCheckbox");
	isSharedCheckbox->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
	isSharedCheckbox->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&BoolEditor::OnEventIsSharedCheckboxChanged, this));
	mEditorWidget->addChildWindow(isSharedCheckbox);
	
	// Create is-locked image
	CEGUI::Window* isLockedWidget = CreateIsLockedImageWidget(editorName + "/IsLockedWidget");
	mEditorWidget->addChildWindow(isLockedWidget);
}

void BoolEditor::DeinitWidget()
{
	gGUIMgr.DestroyWindow(mEditorWidget);
	mEditorWidget = 0;
}

void BoolEditor::SetupWidget(BoolEditor::Model* model)
{
	// Set dimensions
	CEGUI::UDim dimLeft = model->IsShareable() ? cegui_absdim(16) : cegui_absdim(0);
	CEGUI::UDim dimMiddle = model->IsListElement() ? cegui_absdim(32) : cegui_reldim(0.5f);
	CEGUI::UDim dimRight = model->IsRemovable() ? CEGUI::UDim(1, - 24 - 2) : cegui_reldim(1);
	
	// Setup label widget
	CEGUI::Window* labelWidget = mEditorWidget->getChildAtIdx(0);
	labelWidget->setArea(CEGUI::URect(dimLeft, cegui_absdim(0), dimMiddle + cegui_absdim(-2), cegui_reldim(1)));
	labelWidget->setProperty("HorzFormatting", model->IsListElement() ? "RightAligned" : "LeftAligned");
	labelWidget->setText(model->GetName());
	labelWidget->setTooltipText(model->GetTooltip());

	// Setup checkbox widget
	mCheckboxWidget->setArea(CEGUI::URect(dimMiddle + CEGUI::UDim(0, 6), CEGUI::UDim(0, 0), dimMiddle + CEGUI::UDim(0, 16), CEGUI::UDim(0, GetEditboxHeight())));
	mCheckboxWidget->setEnabled(!model->IsReadOnly());

	// Setup remove button widget
	CEGUI::Window* removeButton = mEditorWidget->getChildAtIdx(2);
	if (mModel->IsRemovable())
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
	if (mModel->IsShareable())
	{
		isSharedCheckbox->setVisible(true);
		isSharedCheckbox->setPosition(CEGUI::UVector2(cegui_absdim(0), cegui_absdim(0)));
		isSharedCheckbox->setSelected(mModel->IsShared());
	}
	else
	{
		isSharedCheckbox->setVisible(false);
	}

	// Create is-locked image
	CEGUI::Window* isLockedWidget = mEditorWidget->getChildAtIdx(4);
	if (mModel->IsLocked())
	{
		isLockedWidget->setVisible(true);
		isLockedWidget->setPosition(CEGUI::UVector2(dimMiddle + cegui_absdim(20), CEGUI::UDim(0.5f, -6)));
	}
	else
	{
		isLockedWidget->setVisible(false);
	}
}

bool BoolEditor::OnRemoveButtonClicked(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	mModel->Remove();
	return true;
}

bool BoolEditor::OnEventCheckStateChanged(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	OC_DASSERT(mCheckboxWidget != 0);
	this->Submit();
	return true;
}

bool Editor::BoolEditor::OnEventIsSharedCheckboxChanged(const CEGUI::EventArgs& args)
{
	const CEGUI::WindowEventArgs& winArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
	bool isSelected = static_cast<CEGUI::Checkbox*>(winArgs.window)->isSelected();
	if (mModel->IsShared() != isSelected)
	{
		mModel->SetShared(isSelected);
	}
	return true;
}
