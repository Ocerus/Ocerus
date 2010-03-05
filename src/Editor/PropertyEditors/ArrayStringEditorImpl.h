/// @file
/// Defines template member functions for ArrayStringEditor.

#include "ArrayStringEditor.h"

#include "GUISystem/CEGUITools.h"
#include "EntityAttributeModel.h"
#include "CEGUI.h"


namespace Editor
{
	template<class Model, class ElementType>
	CEGUI::Window* Editor::ArrayStringEditor<Model, ElementType>::CreateEditorWidget(CEGUI::Window* parent)
	{
		OC_ASSERT(mEditboxWidget == 0);
		const CEGUI::String namePrefix = parent->getName() + "/" + this->mModel.GetKey() + "EditGroup";
		CEGUI::Window* editContainer = gCEGUIWM.createWindow("DefaultWindow", namePrefix);
		parent->addChildWindow(editContainer);


		mEditboxWidget = static_cast<CEGUI::Editbox*>(gCEGUIWM.createWindow("Editor/Editbox", namePrefix + "/Editbox"));
		mEditboxWidget->setProperty("ReadOnly", this->mModel.GetAccessFlags() & Reflection::PA_EDIT_WRITE ? "False" : "True");

		mEditboxWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, -32), CEGUI::UDim(1, 0)));

		editContainer->addChildWindow(mEditboxWidget);

		mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventActivated, CEGUI::Event::Subscriber(&Editor::ArrayStringEditor<Model, ElementType>::OnEventActivated, this));
		mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventDeactivated, CEGUI::Event::Subscriber(&Editor::ArrayStringEditor<Model, ElementType>::OnEventDeactivated, this));
		mEditboxWidget->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&Editor::ArrayStringEditor<Model, ElementType>::OnEventKeyDown, this));


		Update();
		return editContainer;
	}

	template<class Model, class ElementType>
	void Editor::ArrayStringEditor<Model, ElementType>::Submit()
	{
		string newValue = mEditboxWidget->getText().c_str();
		this->mModel.SetValueFromString(newValue);
	}

	template<class Model, class ElementType>
	void Editor::ArrayStringEditor<Model, ElementType>::Update()
	{
		OC_ASSERT(mEditboxWidget != 0);
		if (this->UpdatesLocked()) return;
		mEditboxWidget->setText(this->mModel.IsValid() ? this->mModel.GetValueString() : "");
	}

	template<class Model, class ElementType>
	uint32 Editor::ArrayStringEditor<Model, ElementType>::GetEditorHeightHint() const
	{
		return (int32)gCEGUIWM.getWindow("EditorRoot")->getFont(true)->getLineSpacing(1.1f) + 10;
	}

	template<class Model, class ElementType>
	bool Editor::ArrayStringEditor<Model, ElementType>::OnEventKeyDown(const CEGUI::EventArgs& args)
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
}
