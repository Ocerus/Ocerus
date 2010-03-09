#include "Common.h"
#include "StringEditor.h"
#include "GUISystem/CEGUITools.h"
#include "EntityAttributeModel.h"

#include "CEGUI.h"

using namespace Editor;

template<class Model>
CEGUI::Window* Editor::StringEditor<Model>::CreateWidget(const CEGUI::String& namePrefix)
{
	OC_ASSERT(mEditboxWidget == 0);
	CEGUI::Window* widget = gCEGUIWM.createWindow("DefaultWindow", namePrefix);
	widget->setHeight(CEGUI::UDim(0, gCEGUIWM.getWindow("EditorRoot")->getFont(true)->getLineSpacing(1.1f) + 10));

	CEGUI::Window* labelWidget = this->CreateLabelWidget(namePrefix);
	labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(0.5f, -2), CEGUI::UDim(1, 0)));
	widget->addChildWindow(labelWidget);

	mEditboxWidget = static_cast<CEGUI::Editbox*>(gCEGUIWM.createWindow("Editor/Editbox", namePrefix + "/Editbox"));
	mEditboxWidget->setArea(CEGUI::URect(CEGUI::UDim(0.5f, 2), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
	widget->addChildWindow(mEditboxWidget);
	
	mEditboxWidget->setProperty("ReadOnly", this->mModel.GetAccessFlags() & Reflection::PA_EDIT_WRITE ? "False" : "True");

	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&Editor::StringEditor<Model>::OnEventActivated, this));
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&Editor::StringEditor<Model>::OnEventDeactivated, this));
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&Editor::StringEditor<Model>::OnEventKeyDown, this));

	Update();
	return widget;
}

template<class Model>
void Editor::StringEditor<Model>::Submit()
{
	OC_DASSERT(mEditboxWidget != 0);
	string newValue = mEditboxWidget->getText().c_str();
	this->mModel.SetValueFromString(newValue);
}

template<class Model>
void Editor::StringEditor<Model>::Update()
{
	OC_DASSERT(mEditboxWidget != 0);
	if (this->UpdatesLocked()) return;
	mEditboxWidget->setText(this->mModel.IsValid() ? this->mModel.GetValueString() : "");
}

template<class Model>
bool Editor::StringEditor<Model>::OnEventKeyDown(const CEGUI::EventArgs& args)
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

/// Explicit Instantiation
template class Editor::StringEditor<PropertyHolder>;
template class Editor::StringEditor<EntityAttributeModel>;
