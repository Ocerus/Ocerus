#include "Common.h"
#include "StringEditor.h"
#include "GUISystem/CEGUITools.h"
#include "EntityAttributeModel.h"

#include "CEGUI.h"

using namespace Editor;

template<class Model>
CEGUI::Window* Editor::StringEditor<Model>::CreateEditorWidget(CEGUI::Window* parent)
{
	OC_ASSERT(mEditboxWidget == 0);
	mEditboxWidget = static_cast<CEGUI::Editbox*>(gCEGUIWM.createWindow("Editor/Editbox", parent->getName() + "/" + this->mModel.GetKey() + "Editbox"));
	mEditboxWidget->setProperty("ReadOnly", this->mModel.GetAccessFlags() & Reflection::PA_EDIT_WRITE ? "False" : "True");

	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&Editor::StringEditor<Model>::OnEventActivated, this));
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&Editor::StringEditor<Model>::OnEventDeactivated, this));
	mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&Editor::StringEditor<Model>::OnEventKeyDown, this));

	parent->addChildWindow(mEditboxWidget);
	Update();
	return mEditboxWidget;
}

template<class Model>
void Editor::StringEditor<Model>::Submit()
{
	string newValue = mEditboxWidget->getText().c_str();
	this->mModel.SetValueFromString(newValue);
}

template<class Model>
void Editor::StringEditor<Model>::Update()
{
	OC_ASSERT(mEditboxWidget != 0);
	if (this->UpdatesLocked()) return;
	mEditboxWidget->setText(this->mModel.IsValid() ? this->mModel.GetValueString() : "");
}

template<class Model>
uint32 Editor::StringEditor<Model>::GetEditorHeightHint() const
{
	return (int32)gCEGUIWM.getWindow("EditorRoot")->getFont(true)->getLineSpacing(1.1f) + 10;
}

template<class Model>
bool Editor::StringEditor<Model>::OnEventKeyDown(const CEGUI::EventArgs& args)
{
	const CEGUI::KeyEventArgs& keyArgs = static_cast<const CEGUI::KeyEventArgs&>(args);
	switch (keyArgs.scancode)
	{
		case CEGUI::Key::Escape:
			mEditboxWidget->deactivate();
			return true;
		case CEGUI::Key::Return:
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
