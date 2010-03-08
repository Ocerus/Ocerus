/// @file
/// Defines template member functions for ArrayStringEditor.

#include "ArrayStringEditor.h"

#include "GUISystem/CEGUITools.h"
#include "EntityAttributeModel.h"
#include "CEGUI.h"
#include "GUISystem/GUIMgr.h"
#include "Array.h"
#include "StringConverter.h"


namespace Editor
{
	template<class Model, class ElementType>
	CEGUI::Window* Editor::ArrayStringEditor<Model, ElementType>::CreateEditorWidget(CEGUI::Window* parent)
	{
		OC_ASSERT(mEditboxWidget == 0);
		mNamePrefix = string(parent->getName().c_str()) + "/" + this->mModel.GetKey().ToString() + "EditGroup";
		CEGUI::Window* editContainer = gCEGUIWM.createWindow("DefaultWindow", mNamePrefix);
		parent->addChildWindow(editContainer);


		mEditboxWidget = static_cast<CEGUI::Editbox*>(gCEGUIWM.createWindow("Editor/Editbox", mNamePrefix + "/Editbox"));
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

	template<class Model, class ElementType>
	bool ArrayStringEditor<Model, ElementType>::OnEventActivated(const CEGUI::EventArgs&)
	//bool ArrayStringEditor::OnEventActivated(const CEGUI::EventArgs&)
	{
		if (this->mArrayEditorWidget == 0)
		{
			mArrayEditorWidget = GUISystem::LoadWindowLayout("Editor-ArrayEditor.layout", mNamePrefix + "/");
			gGUIMgr.GetRootLayout()->addChildWindow(mArrayEditorWidget);
			mArrayEditorWidget->setArea(CEGUI::URect(CEGUI::UDim(0.35f, 0), CEGUI::UDim(0.5f, -75), CEGUI::UDim(0.65f, 0), CEGUI::UDim(0.5f, 75)));

			mArrayEditorListWidget = static_cast<CEGUI::Listbox*>(mArrayEditorWidget->getChild(mArrayEditorWidget->getName() + "/ValueList"));

			ArrayType array = AbstractValueEditor<Model>::mModel.template GetValue<ArrayType>();
			for (int32 i = 0; i < array->GetSize(); ++i)
			{
				mArrayEditorListWidget->addItem(new CEGUI::ListboxTextItem(Utils::StringConverter::ToString((*array)[i])));
			}
			
			OC_ASSERT(mArrayEditorListWidget != 0);
		}
		
		
		return true;
	}

}
