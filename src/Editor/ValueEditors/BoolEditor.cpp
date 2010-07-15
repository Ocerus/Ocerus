#include "Common.h"
#include "BoolEditor.h"
#include "GUISystem/CEGUITools.h"
#include "Editor/EditorMgr.h"

using namespace Editor;

BoolEditor::~BoolEditor()
{
	delete mModel;
}

CEGUI::Window* BoolEditor::CreateWidget(const CEGUI::String& namePrefix)
{
	PROFILE_FNC();

	OC_DASSERT(mEditorWidget == 0);
	OC_DASSERT(mCheckboxWidget == 0);

	/// Create main editor widget
	mEditorWidget = gCEGUIWM.createWindow("DefaultWindow", namePrefix);
	mEditorWidget->setHeight(CEGUI::UDim(0, GetEditboxHeight()));

	CEGUI::UDim dimMiddle = mModel->IsListElement() ? CEGUI::UDim(0, 32) : CEGUI::UDim(0.5f, 0);
	CEGUI::UDim dimRight = mModel->IsRemovable() ? CEGUI::UDim(1, - GetEditboxHeight() - 2) : CEGUI::UDim(1, 0);
	
	/// Create label widget of the editor
	CEGUI::Window* labelWidget = this->CreateEditorLabelWidget(namePrefix + "/Label", mModel);
	labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), dimMiddle + CEGUI::UDim(0, -2), CEGUI::UDim(1, 0)));
	mEditorWidget->addChildWindow(labelWidget);

	/// Create checkbox widget of the editor
	mCheckboxWidget = static_cast<CEGUI::Checkbox*>(gCEGUIWM.createWindow("Editor/Checkbox", namePrefix + "/Checkbox"));
	mCheckboxWidget->setArea(CEGUI::URect(dimMiddle + CEGUI::UDim(0, 6), CEGUI::UDim(0, 0), dimRight, CEGUI::UDim(0, GetEditboxHeight())));
	mCheckboxWidget->setEnabled(!mModel->IsReadOnly());
	mEditorWidget->addChildWindow(mCheckboxWidget);

	/// Create remove button, if needed
	if (mModel->IsRemovable())
	{
		CEGUI::Window* removeButton = CreateRemoveElementButtonWidget(namePrefix + "/RemoveButton");
		removeButton->setPosition(CEGUI::UVector2(dimRight + CEGUI::UDim(0, 2), CEGUI::UDim(0, 0)));
		removeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&BoolEditor::OnEventButtonRemovePressed, this));

		mEditorWidget->addChildWindow(removeButton);
	}
	
	/// Subscribe to checkbox events
	mCheckboxWidget->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&BoolEditor::OnEventCheckStateChanged, this));

	/// Update editor and return main widget
	Update();
	return mEditorWidget;
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
bool BoolEditor::OnEventButtonRemovePressed(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	mModel->Remove();
	return true;
}

bool BoolEditor::OnEventCheckStateChanged(const CEGUI::EventArgs& args)
{
	OC_DASSERT(mCheckboxWidget != 0);
	this->Submit();
	gEditorMgr.PropertyValueChanged();
	return true;
}