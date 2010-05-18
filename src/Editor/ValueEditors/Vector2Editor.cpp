#include "Common.h"
#include "Vector2Editor.h"
#include "Editor/EditorMgr.h"
#include "GUISystem/CEGUITools.h"

using namespace Editor;

Editor::Vector2Editor::~Vector2Editor()
{
	delete mModel;
}

CEGUI::Window* Editor::Vector2Editor::CreateWidget(const CEGUI::String& namePrefix)
{
	PROFILE_FNC();

	OC_ASSERT(mEditbox1Widget == 0);
	OC_ASSERT(mEditbox2Widget == 0);

	/// Create main editor widget
	mEditorWidget = gCEGUIWM.createWindow("DefaultWindow", namePrefix);
	mEditorWidget->setHeight(CEGUI::UDim(0, 2 * GetEditboxHeight() + 5));

	CEGUI::UDim dimMiddle = mModel->IsListElement() ? CEGUI::UDim(0, 32) : CEGUI::UDim(0.5f, 0);
	CEGUI::UDim dimRight = mModel->IsRemovable() ? CEGUI::UDim(1, - GetEditboxHeight() - 2) : CEGUI::UDim(1, 0);

	/// Create label widget of the editor
	CEGUI::Window* labelWidget = CreateEditorLabelWidget(namePrefix + "/Label", mModel);
	//labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), dimMiddle + CEGUI::UDim(0, -2), CEGUI::UDim(0, GetEditboxHeight())));
	labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), dimMiddle + CEGUI::UDim(0, -2), CEGUI::UDim(1, 0)));
	mEditorWidget->addChildWindow(labelWidget);

	/// Create label widget for x coordinate
	CEGUI::Window* labelWidgetX = CreateStaticTextWidget(namePrefix + "/LabelX", "x:", "The x coordinate.");
	labelWidgetX->setArea(CEGUI::URect(dimMiddle + CEGUI::UDim(0, 2), CEGUI::UDim(0, 0), dimMiddle + CEGUI::UDim(0, 10), CEGUI::UDim(0, GetEditboxHeight())));
	mEditorWidget->addChildWindow(labelWidgetX);

	/// Create label widget for y coordinate
	CEGUI::Window* labelWidgetY = CreateStaticTextWidget(namePrefix + "/LabelY", "y:", "The y coordinate.");
	labelWidgetY->setArea(CEGUI::URect(dimMiddle + CEGUI::UDim(0, 2), CEGUI::UDim(1, -GetEditboxHeight()), dimMiddle + CEGUI::UDim(0, 10), CEGUI::UDim(1, 0)));
	mEditorWidget->addChildWindow(labelWidgetY);

	/// Create editbox widget for x coordinate
	mEditbox1Widget = static_cast<CEGUI::Editbox*>(gCEGUIWM.createWindow("Editor/Editbox", namePrefix + "/Editbox1"));
	mEditbox1Widget->setProperty("ReadOnly", mModel->IsReadOnly() ? "True" : "False");
	mEditbox1Widget->setArea(CEGUI::URect(dimMiddle + CEGUI::UDim(0, 16), CEGUI::UDim(0, 0), dimRight, CEGUI::UDim(0, GetEditboxHeight())));
	mEditorWidget->addChildWindow(mEditbox1Widget);

	/// Subscribe to editbox1 events
	mEditbox1Widget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&Editor::Vector2Editor::OnEventActivated, this));
	mEditbox1Widget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&Editor::Vector2Editor::OnEventDeactivated, this));
	mEditbox1Widget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&Editor::Vector2Editor::OnEventKeyDown, this));

	/// Create editbox widget for y coordinate
	mEditbox2Widget = static_cast<CEGUI::Editbox*>(gCEGUIWM.createWindow("Editor/Editbox", namePrefix + "/Editbox2"));
	mEditbox2Widget->setProperty("ReadOnly", mModel->IsReadOnly() ? "True" : "False");
	mEditbox2Widget->setArea(CEGUI::URect(dimMiddle + CEGUI::UDim(0, 16), CEGUI::UDim(1, -GetEditboxHeight()), dimRight, CEGUI::UDim(1, 0)));
	mEditorWidget->addChildWindow(mEditbox2Widget);

	/// Subscribe to editbox2 events
	mEditbox2Widget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&Editor::Vector2Editor::OnEventActivated, this));
	mEditbox2Widget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&Editor::Vector2Editor::OnEventDeactivated, this));
	mEditbox2Widget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&Editor::Vector2Editor::OnEventKeyDown, this));

	/// Create remove button, if needed
	if (mModel->IsRemovable())
	{
		CEGUI::Window* removeButton = CreateRemoveElementButtonWidget(namePrefix + "/RemoveButton");
		removeButton->setPosition(CEGUI::UVector2(dimRight + CEGUI::UDim(0, 2), CEGUI::UDim(0, GetEditboxHeight() / 2)));
		removeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::Vector2Editor::OnEventButtonRemovePressed, this));
		mEditorWidget->addChildWindow(removeButton);
	}

	Update();
	return mEditorWidget;
}

void Editor::Vector2Editor::Submit()
{
	OC_DASSERT(mEditbox1Widget != 0);
	OC_DASSERT(mEditbox2Widget != 0);
	Vector2 newValue;
	newValue.x = Utils::StringConverter::FromString<float32>(mEditbox1Widget->getText().c_str());
	newValue.y = Utils::StringConverter::FromString<float32>(mEditbox2Widget->getText().c_str());
	mModel->SetValue(newValue);
}

void Editor::Vector2Editor::Update()
{
	OC_DASSERT(mEditbox1Widget != 0);
	OC_DASSERT(mEditbox2Widget != 0);

	if (UpdatesLocked()) return;

	if (!mModel->IsValid())
	{
		mEditbox1Widget->setText("");
		mEditbox2Widget->setText("");
		return;
	}
	Vector2 value = mModel->GetValue();
	mEditbox1Widget->setText(Utils::StringConverter::ToString(value.x));
	mEditbox2Widget->setText(Utils::StringConverter::ToString(value.y));
}


bool Editor::Vector2Editor::OnEventDeactivated(const CEGUI::EventArgs&)
{
	if (mEditbox1Widget->isActive() || mEditbox2Widget->isActive())
		return false;
	this->UnlockUpdates();
	return true;
}

bool Editor::Vector2Editor::OnEventButtonRemovePressed(const CEGUI::EventArgs& args)
{
	OC_UNUSED(args);
	mModel->Remove();
	return true;
}

bool Editor::Vector2Editor::OnEventKeyDown(const CEGUI::EventArgs& args)
{
	OC_DASSERT(mEditbox1Widget != 0);
	OC_DASSERT(mEditbox2Widget != 0);
	const CEGUI::KeyEventArgs& keyArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
	switch (keyArgs.scancode)
	{
		case CEGUI::Key::Escape:
			mEditbox1Widget->deactivate();
			mEditbox2Widget->deactivate();
			return true;
		case CEGUI::Key::Return:
		case CEGUI::Key::NumpadEnter:
			this->Submit();
			mEditbox1Widget->deactivate();
			mEditbox2Widget->deactivate();
			gEditorMgr.PropertyValueChanged();
			return true;
		default:
			break;
	}
	return false;
}
