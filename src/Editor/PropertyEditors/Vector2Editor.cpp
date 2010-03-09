#include "Common.h"
#include "Vector2Editor.h"
#include "GUISystem/CEGUITools.h"
#include "EntityAttributeModel.h"

#include "CEGUI.h"

using namespace Editor;

template<class Model>
CEGUI::Window* Editor::Vector2Editor<Model>::CreateWidget(const CEGUI::String& namePrefix)
{
	OC_ASSERT(mEditbox1Widget == 0);
	OC_ASSERT(mEditbox2Widget == 0);
	float32 editboxHeight = gCEGUIWM.getWindow("EditorRoot")->getFont(true)->getLineSpacing(1.1f) + 10;
	CEGUI::Window* widget = gCEGUIWM.createWindow("DefaultWindow", namePrefix);
	widget->setHeight(CEGUI::UDim(0, 2 * editboxHeight + 5));

	/// labelWidget
	{
		CEGUI::Window* labelWidget = gCEGUIWM.createWindow("Editor/StaticText", namePrefix + "/EditorLabel");
		labelWidget->setText(this->mModel.GetName());
		labelWidget->setProperty("FrameEnabled", "False");
		labelWidget->setProperty("BackgroundEnabled", "False");
		labelWidget->setTooltipText(this->mModel.GetComment());
		labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(0.5f, -2), CEGUI::UDim(0, editboxHeight)));
		widget->addChildWindow(labelWidget);
	}

	/// labelWidgets x, y
	{
		CEGUI::Window* labelWidgetX = gCEGUIWM.createWindow("Editor/StaticText", namePrefix + "/EditorLabelX");
		labelWidgetX->setText("x:");
		labelWidgetX->setProperty("FrameEnabled", "False");
		labelWidgetX->setProperty("BackgroundEnabled", "False");
		labelWidgetX->setTooltipText("The x coordinate.");
		labelWidgetX->setArea(CEGUI::URect(CEGUI::UDim(0.5f, 5), CEGUI::UDim(0, 0), CEGUI::UDim(0.5f, 18), CEGUI::UDim(0, editboxHeight)));
		widget->addChildWindow(labelWidgetX);

		CEGUI::Window* labelWidgetY = gCEGUIWM.createWindow("Editor/StaticText", namePrefix + "/EditorLabelY");
		labelWidgetY->setText("y:");
		labelWidgetY->setProperty("FrameEnabled", "False");
		labelWidgetY->setProperty("BackgroundEnabled", "False");
		labelWidgetY->setTooltipText("The y coordinate.");
		labelWidgetY->setArea(CEGUI::URect(CEGUI::UDim(0.5f, 5), CEGUI::UDim(1, -editboxHeight), CEGUI::UDim(0.5f, 18), CEGUI::UDim(1, 0)));
		widget->addChildWindow(labelWidgetY);
	}
	
	mEditbox1Widget = static_cast<CEGUI::Editbox*>(gCEGUIWM.createWindow("Editor/Editbox", namePrefix + "/Editbox1"));
	mEditbox1Widget->setArea(CEGUI::URect(CEGUI::UDim(0.5f, 20), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(0, editboxHeight)));
	widget->addChildWindow(mEditbox1Widget);

	mEditbox1Widget->setProperty("ReadOnly", this->mModel.GetAccessFlags() & Reflection::PA_EDIT_WRITE ? "False" : "True");

	mEditbox1Widget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&Editor::Vector2Editor<Model>::OnEventActivated, this));
	mEditbox1Widget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&Editor::Vector2Editor<Model>::OnEventDeactivated, this));
	mEditbox1Widget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&Editor::Vector2Editor<Model>::OnEventKeyDown, this));

	mEditbox2Widget = static_cast<CEGUI::Editbox*>(gCEGUIWM.createWindow("Editor/Editbox", namePrefix + "/Editbox2"));
	mEditbox2Widget->setArea(CEGUI::URect(CEGUI::UDim(0.5f, 20), CEGUI::UDim(1, -editboxHeight), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	widget->addChildWindow(mEditbox2Widget);

	mEditbox2Widget->setProperty("ReadOnly", this->mModel.GetAccessFlags() & Reflection::PA_EDIT_WRITE ? "False" : "True");

	mEditbox2Widget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&Editor::Vector2Editor<Model>::OnEventActivated, this));
	mEditbox2Widget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&Editor::Vector2Editor<Model>::OnEventDeactivated, this));
	mEditbox2Widget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&Editor::Vector2Editor<Model>::OnEventKeyDown, this));

	Update();
	return widget;
}

template<class Model>
void Editor::Vector2Editor<Model>::Submit()
{
	OC_DASSERT(mEditbox1Widget != 0);
	OC_DASSERT(mEditbox2Widget != 0);
	Vector2 newValue;
	newValue.x = Utils::StringConverter::FromString<float32>(mEditbox1Widget->getText().c_str());
	newValue.y = Utils::StringConverter::FromString<float32>(mEditbox2Widget->getText().c_str());
	this->mModel.SetValue(newValue);
}

template<class Model>
void Editor::Vector2Editor<Model>::Update()
{
	OC_DASSERT(mEditbox1Widget != 0);
	OC_DASSERT(mEditbox2Widget != 0);
	if (this->UpdatesLocked()) return;
	if (!this->mModel.IsValid())
	{
		mEditbox1Widget->setText("");
		mEditbox2Widget->setText("");
		return;
	}
	Vector2 value = this->mModel.template GetValue<Vector2>();
	mEditbox1Widget->setText(Utils::StringConverter::ToString(value.x));
	mEditbox2Widget->setText(Utils::StringConverter::ToString(value.y));
}


template<class Model>
bool Editor::Vector2Editor<Model>::OnEventDeactivated(const CEGUI::EventArgs&)
{
	if (mEditbox1Widget->isActive() || mEditbox2Widget->isActive())
		return false;
	this->UnlockUpdates();
	this->Update();
	return true;
}

template<class Model>
bool Editor::Vector2Editor<Model>::OnEventKeyDown(const CEGUI::EventArgs& args)
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

/// Explicit Instantiation
template class Editor::Vector2Editor<PropertyHolder>;
