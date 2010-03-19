#include "Common.h"
#include "Vector2Editor.h"
#include "GUISystem/CEGUITools.h"

using namespace Editor;

Editor::Vector2Editor::~Vector2Editor()
{
	delete mModel;
}

CEGUI::Window* Editor::Vector2Editor::CreateWidget(const CEGUI::String& namePrefix)
{
	OC_ASSERT(mEditbox1Widget == 0);
	OC_ASSERT(mEditbox2Widget == 0);

	/// Create main editor widget
	CEGUI::Window* widget = gCEGUIWM.createWindow("DefaultWindow", namePrefix);
	widget->setHeight(CEGUI::UDim(0, 2 * GetEditboxHeight() + 5));

	/// Create label widget of the editor
	CEGUI::Window* labelWidget = CreateEditorLabelWidget(namePrefix + "/Label", mModel);
	labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(0.5f, -2), CEGUI::UDim(0, GetEditboxHeight())));
	widget->addChildWindow(labelWidget);

	/// Create label widget for x coordinate
	CEGUI::Window* labelWidgetX = CreateStaticTextWidget(namePrefix + "/LabelX", "x:", "The x coordinate.");
	labelWidgetX->setArea(CEGUI::URect(CEGUI::UDim(0.5f, 5), CEGUI::UDim(0, 0), CEGUI::UDim(0.5f, 18), CEGUI::UDim(0, GetEditboxHeight())));
	widget->addChildWindow(labelWidgetX);

	/// Create label widget for y coordinate
	CEGUI::Window* labelWidgetY = CreateStaticTextWidget(namePrefix + "/LabelY", "y:", "The y coordinate.");
	labelWidgetY->setArea(CEGUI::URect(CEGUI::UDim(0.5f, 5), CEGUI::UDim(1, -GetEditboxHeight()), CEGUI::UDim(0.5f, 18), CEGUI::UDim(1, 0)));
	widget->addChildWindow(labelWidgetY);

	
	/// Create editbox widget for x coordinate
	mEditbox1Widget = static_cast<CEGUI::Editbox*>(gCEGUIWM.createWindow("Editor/Editbox", namePrefix + "/Editbox1"));
	mEditbox1Widget->setProperty("ReadOnly", mModel->IsReadOnly() ? "True" : "False");
	mEditbox1Widget->setArea(CEGUI::URect(CEGUI::UDim(0.5f, 20), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(0, GetEditboxHeight())));
	widget->addChildWindow(mEditbox1Widget);

	/// Subscribe to editbox1 events
	mEditbox1Widget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&Editor::Vector2Editor::OnEventActivated, this));
	mEditbox1Widget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&Editor::Vector2Editor::OnEventDeactivated, this));
	mEditbox1Widget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&Editor::Vector2Editor::OnEventKeyDown, this));

	/// Create editbox widget for y coordinate
	mEditbox2Widget = static_cast<CEGUI::Editbox*>(gCEGUIWM.createWindow("Editor/Editbox", namePrefix + "/Editbox2"));
	mEditbox2Widget->setProperty("ReadOnly", mModel->IsReadOnly() ? "True" : "False");
	mEditbox2Widget->setArea(CEGUI::URect(CEGUI::UDim(0.5f, 20), CEGUI::UDim(1, -GetEditboxHeight()), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	widget->addChildWindow(mEditbox2Widget);

	/// Subscribe to editbox2 events
	mEditbox2Widget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&Editor::Vector2Editor::OnEventActivated, this));
	mEditbox2Widget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&Editor::Vector2Editor::OnEventDeactivated, this));
	mEditbox2Widget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&Editor::Vector2Editor::OnEventKeyDown, this));

	Update();
	return widget;
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
	this->Update();
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
			return true;
		default:
			break;
	}
	return false;
}
