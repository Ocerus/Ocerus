#include "Common.h"
#include "TwoDimEditor.h"
#include "Editor/EditorMgr.h"
#include "GUISystem/CEGUICommon.h"
#include <boost/concept_check.hpp>

using namespace Editor;

const eValueEditorType PointEditor::Type(VET_PT_POINT);
const eValueEditorType Vector2Editor::Type(VET_PT_VECTOR2);

TwoDimEditor::~TwoDimEditor()
{
	DeinitWidget();
}

CEGUI::Window* TwoDimEditor::GetWidget()
{
	return mEditorWidget;
}

void TwoDimEditor::Submit()
{
	float32 x = Utils::StringConverter::FromString<float32>(GetEditbox1Widget()->getText().c_str());
	float32 y = Utils::StringConverter::FromString<float32>(GetEditbox2Widget()->getText().c_str());
	SetValue(x, y);
}

void TwoDimEditor::Update()
{
	if (UpdatesLocked()) return;
	float32 x, y;
	GetValue(x, y);
	GetEditbox1Widget()->setText(Utils::StringConverter::ToString(x));
	GetEditbox1Widget()->setTooltipText(Utils::StringConverter::ToString(x));
	GetEditbox2Widget()->setText(Utils::StringConverter::ToString(y));
	GetEditbox2Widget()->setTooltipText(Utils::StringConverter::ToString(y));
}

bool TwoDimEditor::OnEventDeactivated(const CEGUI::EventArgs&)
{
	if (GetEditbox1Widget()->isActive() || GetEditbox2Widget()->isActive())
		return false;
	this->UnlockUpdates();
	return true;
}

bool TwoDimEditor::OnRemoveButtonClicked(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	GetModel()->Remove();
	return true;
}

bool TwoDimEditor::OnEventKeyDown(const CEGUI::EventArgs& args)
{
	OC_DASSERT(GetEditbox1Widget() != 0);
	OC_DASSERT(GetEditbox2Widget() != 0);
	const CEGUI::KeyEventArgs& keyArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
	switch (keyArgs.scancode)
	{
		case CEGUI::Key::Escape:
			GetEditbox1Widget()->deactivate();
			GetEditbox2Widget()->deactivate();
			return true;
		case CEGUI::Key::Return:
		case CEGUI::Key::NumpadEnter:
			this->Submit();
			GetEditbox1Widget()->deactivate();
			GetEditbox2Widget()->deactivate();
			gEditorMgr.PropertyValueChanged();
			return true;
		default:
			break;
	}
	return false;
}

bool Editor::TwoDimEditor::OnEventIsSharedCheckboxChanged(const CEGUI::EventArgs& args)
{
	const CEGUI::WindowEventArgs& winArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
	bool isSelected = static_cast<CEGUI::Checkbox*>(winArgs.window)->isSelected();
	if (GetModel()->IsShared() != isSelected)
	{
		GetModel()->SetShared(isSelected);
		gEditorMgr.PropertyValueChanged();
	}
	return true;
}

void Editor::TwoDimEditor::InitWidget()
{
	static unsigned int editorCounter = 0;
	OC_DASSERT(mEditorWidget == 0);

	CEGUI::String editorName = "Editor/EntityWindow/ValueEditors/TwoDimEditor" + Utils::StringConverter::ToString(editorCounter++);

	// Create main editor widget
	mEditorWidget = gGUIMgr.CreateWindow("DefaultWindow", editorName);
	mEditorWidget->setHeight(cegui_absdim(2 * GetEditboxHeight() + 5));

	// Create label widget
	CEGUI::Window* labelWidget = CreateLabelWidget(editorName + "/Label");
	mEditorWidget->addChildWindow(labelWidget);

	CEGUI::Window* labelWidgetX = CreateLabelWidget(editorName + "/LabelX");
	labelWidgetX->setText("x:");
	labelWidgetX->setTooltipText("The x coordinate.");
	mEditorWidget->addChildWindow(labelWidgetX);

	CEGUI::Window* labelWidgetY = CreateLabelWidget(editorName + "/LabelY");
	labelWidgetY->setText("y:");
	labelWidgetY->setTooltipText("The y coordinate.");
	mEditorWidget->addChildWindow(labelWidgetY);

	// Create editbox widget
	CEGUI::Window* editbox1Widget = gGUIMgr.CreateWindow("Editor/Editbox", editorName + "/Editbox1");
	mEditorWidget->addChildWindow(editbox1Widget);
	CEGUI::Window* editbox2Widget = gGUIMgr.CreateWindow("Editor/Editbox", editorName + "/Editbox2");
	mEditorWidget->addChildWindow(editbox2Widget);

	editbox1Widget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&TwoDimEditor::OnEventActivated, this));
	editbox2Widget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&TwoDimEditor::OnEventActivated, this));
	editbox1Widget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&TwoDimEditor::OnEventDeactivated, this));
	editbox2Widget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&TwoDimEditor::OnEventDeactivated, this));
	editbox1Widget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&TwoDimEditor::OnEventKeyDown, this));
	editbox2Widget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&TwoDimEditor::OnEventKeyDown, this));

	// Create remove button widget
	CEGUI::Window* removeButton = CreateRemoveButtonWidget(editorName + "/RemoveButton");
	removeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&TwoDimEditor::OnRemoveButtonClicked, this));
	mEditorWidget->addChildWindow(removeButton);

	// Create is-shared checkbox
	CEGUI::Window* isSharedCheckbox = CreateIsSharedCheckboxWidget(editorName + "/IsSharedCheckbox");
	isSharedCheckbox->setPosition(CEGUI::UVector2(cegui_absdim(0), cegui_absdim(0)));
	isSharedCheckbox->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&TwoDimEditor::OnEventIsSharedCheckboxChanged, this));
	mEditorWidget->addChildWindow(isSharedCheckbox);
	
	// Create is-locked images
	GetEditbox1Widget()->addChildWindow(CreateIsLockedImageWidget(editorName + "/IsLockedWidget1"));
	GetEditbox2Widget()->addChildWindow(CreateIsLockedImageWidget(editorName + "/IsLockedWidget2"));
}

void Editor::TwoDimEditor::DeinitWidget()
{
	gGUIMgr.DestroyWindow(mEditorWidget);
	mEditorWidget = 0;
}

bool TwoDimEditor::IsWidgetInited() const
{
	return mEditorWidget != 0;
}

void TwoDimEditor::SetupWidget(IValueEditorModel* model)
{
	if (mEditorWidget == 0)
		InitWidget();

	// Set dimensions
	CEGUI::UDim dimLeft = model->IsShareable() ? cegui_absdim(16) : cegui_absdim(0);
	CEGUI::UDim dimMiddle = model->IsListElement() ? cegui_absdim(32) : cegui_reldim(0.5f);
	CEGUI::UDim dimRight = model->IsRemovable() ? CEGUI::UDim(1, - GetEditboxHeight() - 2) : cegui_reldim(1);
	
	// Setup label widget
	CEGUI::Window* labelWidget = mEditorWidget->getChildAtIdx(0);
	labelWidget->setArea(CEGUI::URect(dimLeft, cegui_absdim(0), dimMiddle + cegui_absdim(-2), cegui_reldim(1)));
	labelWidget->setText(model->GetName());
	labelWidget->setTooltipText(model->GetTooltip());

	CEGUI::Window* labelXWidget = mEditorWidget->getChildAtIdx(1);
	labelXWidget->setArea(CEGUI::URect(dimMiddle + cegui_absdim(2), cegui_absdim(0), dimMiddle + cegui_absdim(10), cegui_absdim(GetEditboxHeight())));
	CEGUI::Window* labelYWidget = mEditorWidget->getChildAtIdx(2);
	labelYWidget->setArea(CEGUI::URect(dimMiddle + cegui_absdim(2), CEGUI::UDim(1, -GetEditboxHeight()), dimMiddle + cegui_absdim(10), CEGUI::UDim(1, 0)));

	// Setup editbox widget
	CEGUI::Window* editbox1Widget = mEditorWidget->getChildAtIdx(3);
	editbox1Widget->setProperty("ReadOnly", model->IsReadOnly() ? "True" : "False");
	editbox1Widget->setArea(CEGUI::URect(dimMiddle + cegui_absdim(16), cegui_absdim(0), dimRight, cegui_absdim(GetEditboxHeight())));
	AddWidgetToTabNavigation(editbox1Widget);

	CEGUI::Window* editbox2Widget = mEditorWidget->getChildAtIdx(4);
	editbox2Widget->setProperty("ReadOnly", model->IsReadOnly() ? "True" : "False");
	editbox2Widget->setArea(CEGUI::URect(dimMiddle + cegui_absdim(16), CEGUI::UDim(1, -GetEditboxHeight()), dimRight, CEGUI::UDim(1, 0)));
	AddWidgetToTabNavigation(editbox2Widget);

	// Setup remove button
	CEGUI::Window* removeButton = mEditorWidget->getChildAtIdx(5);
	if (model->IsRemovable())
	{
		removeButton->setVisible(true);
		removeButton->setPosition(CEGUI::UVector2(dimRight + cegui_absdim(2), cegui_absdim(GetEditboxHeight() / 2)));
	}
	else
	{
		removeButton->setVisible(false);
	}

	// Setup is-shared checkbox
	CEGUI::Checkbox* isSharedCheckbox = static_cast<CEGUI::Checkbox*>(mEditorWidget->getChildAtIdx(6));
	if (model->IsShareable())
	{
		isSharedCheckbox->setVisible(true);
		isSharedCheckbox->setPosition(CEGUI::UVector2(cegui_absdim(0), cegui_absdim(GetEditboxHeight() - 12)));
		isSharedCheckbox->setSelected(model->IsShared());
		mEditorWidget->addChildWindow(isSharedCheckbox);
	}
	else
	{
		isSharedCheckbox->setVisible(false);
	}

	// Setup is-locked widget
	CEGUI::Window* isLockedWidget1 = GetEditbox1Widget()->getChildAtIdx(0);
	CEGUI::Window* isLockedWidget2 = GetEditbox2Widget()->getChildAtIdx(0);
	isLockedWidget1->setVisible(model->IsLocked());
	isLockedWidget2->setVisible(model->IsLocked());
}

inline CEGUI::Window* TwoDimEditor::GetEditbox1Widget()
{
	OC_DASSERT(mEditorWidget != 0);
	return mEditorWidget->getChildAtIdx(3);
}

inline CEGUI::Window* TwoDimEditor::GetEditbox2Widget()
{
	OC_DASSERT(mEditorWidget != 0);
	return mEditorWidget->getChildAtIdx(4);
}

