/// @file
/// Template method implementations of ArrayEditor.

#ifndef _ARRAYEDITORIMPL_H_
#define _ARRAYEDITORIMPL_H_

#include "ArrayEditor.h"
#include "Models/ArrayElementModel.h"
#include "StringEditor.h"
#include "GUISystem/VerticalLayout.h"
#include "Properties/PropertyEnums.h"

namespace Editor
{

	template<class ElementType>
	CEGUI::Window* ArrayEditor<ElementType>::CreateWidget(const CEGUI::String& namePrefix)
	{
		OC_ASSERT(mWidget == 0);
		mNamePrefix = namePrefix;

		/// Create main editor widget
		mWidget = gCEGUIWM.createWindow("DefaultWindow", namePrefix);
		mLayout = new GUISystem::VerticalLayout(mWidget, 0, true);


		/// Create header widget
		mHeaderWidget = gCEGUIWM.createWindow("DefaultWindow", namePrefix + "Controls");
		mHeaderWidget->setHeight(CEGUI::UDim(0, GetEditboxHeight()));
		mLayout->AddChildWindow(mHeaderWidget);

		/// Create label widget of the editor
		CEGUI::Window* labelWidget = this->CreateEditorLabelWidget(mNamePrefix + "/Label", mModel);
		labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(0.5f, -2), CEGUI::UDim(0, GetEditboxHeight())));
		mHeaderWidget->addChildWindow(labelWidget);
		
		/// Create add element button
		mButtonAddElement = static_cast<CEGUI::PushButton*>(gCEGUIWM.createWindow("Editor/Button", namePrefix + "Controls/ButtonAddElement"));
		mButtonAddElement->setText("Add");
		mButtonAddElement->setArea(CEGUI::URect(CEGUI::UDim(0.5f, 1), CEGUI::UDim(0, 0), CEGUI::UDim(0.6666f, -1), CEGUI::UDim(0, GetEditboxHeight())));
		mButtonAddElement->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::ArrayEditor<ElementType>::OnEventButtonAddPressed, this));

		
		mHeaderWidget->addChildWindow(mButtonAddElement);
		
		/// Create revert button
		mButtonRevert = static_cast<CEGUI::PushButton*>(gCEGUIWM.createWindow("Editor/Button", namePrefix + "Controls/ButtonRevert"));
		mButtonRevert->setText("Rvrt");
		mButtonRevert->setArea(CEGUI::URect(CEGUI::UDim(0.6666f, 1), CEGUI::UDim(0, 0), CEGUI::UDim(0.8333f, -1), CEGUI::UDim(0, GetEditboxHeight())));
		mButtonRevert->setEnabled(false);
		mButtonRevert->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::ArrayEditor<ElementType>::OnEventButtonRevertPressed, this));

		mHeaderWidget->addChildWindow(mButtonRevert);

		/// Create save button
		mButtonSave = static_cast<CEGUI::PushButton*>(gCEGUIWM.createWindow("Editor/Button", namePrefix + "Controls/ButtonSave"));
		mButtonSave->setText("Save");
		mButtonSave->setArea(CEGUI::URect(CEGUI::UDim(0.8333f, 1), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(0, GetEditboxHeight())));
		mButtonSave->setEnabled(false);
		mButtonSave->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Editor::ArrayEditor<ElementType>::OnEventButtonSavePressed, this));
		mHeaderWidget->addChildWindow(mButtonSave);

		/// Update editor and return main widget
		Update();
		mLayout->UpdateLayout();
		return mWidget;
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::Submit()
	{
		SubmitEditors();
		SubmitInternalArray();
		UnlockUpdates();
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::Update()
	{
		/// If one of element editors is locked, consider the whole editor as locked.
		for (typename vector<AbstractValueEditor*>::iterator it = mElementEditors.begin(); it != mElementEditors.end(); ++it)
		{
			if ((*it)->UpdatesLocked())
				LockUpdates();
		}

		if (UpdatesLocked()) return;

		UpdateInternalArray();
		UpdateEditors();
	}

	template<class ElementType>
	bool ArrayEditor<ElementType>::OnEventButtonAddPressed(const CEGUI::EventArgs&)
	{
		LockUpdates();
		mArray.push_back(new ElementType(PropertyTypes::GetDefaultValue<ElementType>()));
		UpdateEditors();
		return true;
	}

	template<class ElementType>
	bool ArrayEditor<ElementType>::OnEventButtonRevertPressed(const CEGUI::EventArgs& arg)
	{
		OC_UNUSED(arg);
		UnlockUpdates();
		return true;
	}

	template<class ElementType>
	bool ArrayEditor<ElementType>::OnEventButtonSavePressed(const CEGUI::EventArgs& arg)
	{
		OC_UNUSED(arg);
		Submit();
		return true;
	}

	template<class ElementType>
	const ElementType& ArrayEditor<ElementType>::GetElement(uint32 index) const
	{
		OC_DASSERT(index < mArray.size());
		return *mArray[index];
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::SetElement(uint32 index, const ElementType& newElement)
	{
		OC_DASSERT(index < mArray.size());
		*mArray[index] = newElement;
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::RemoveElement(uint32 index)
	{
		OC_DASSERT(index < mArray.size());
		LockUpdates();
		SubmitEditors();

		/// Remove given element from internal array.
		delete mArray[index];
		mArray.erase(mArray.begin() + index);

		UpdateEditors();
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::SubmitEditors()
	{
		for (typename vector<AbstractValueEditor*>::iterator it = mElementEditors.begin(); it != mElementEditors.end(); ++it)
		{
			(*it)->Submit();
		}
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::UpdateEditors()
	{
		if (mArray.size() != mElementEditors.size())
		{
			mLayout->LockUpdates();
			for (uint32 i = mArray.size(); i < mElementEditors.size(); ++i)
			{
				delete mElementEditors.back();
				mElementEditors.pop_back();
			}

			for (uint32 i = mElementEditors.size(); i < mArray.size(); ++i)
			{
				/// @todo Choose the right editor
				AbstractValueEditor* editor = new Editor::StringEditor(new ArrayStringElementModel<ElementType>(this, i));
				mElementEditors.push_back(editor);
				mLayout->AddChildWindow(editor->CreateWidget(mNamePrefix + "/Editor" + Utils::StringConverter::ToString(i)));
			}
			mLayout->UnlockUpdates();
			mLayout->UpdateLayout();
		}

		for (typename vector<AbstractValueEditor*>::iterator it = mElementEditors.begin(); it != mElementEditors.end(); ++it)
		{
			(*it)->Update();
		}
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::DeleteEditors()
	{
		for (typename vector<AbstractValueEditor*>::iterator it = mElementEditors.begin(); it != mElementEditors.end(); ++it)
		{
			delete (*it);
		}
		mElementEditors.clear();
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::SubmitInternalArray()
	{
		ArrayType newArray(mArray.size());
		uint32 i = 0;
		for (typename InternalArray::iterator it = mArray.begin(); it != mArray.end(); ++i, ++it)
		{
			newArray[i] = **it;
		}
		mModel->SetValue(&newArray);
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::UpdateInternalArray()
	{
		const ArrayType& array = *mModel->GetValue();
		uint32 oldSize = mArray.size();
		uint32 newSize = array.GetSize();
		uint32 greaterSize = oldSize > newSize ? oldSize : newSize;

		for (uint32 i = 0; i < greaterSize; ++i)
		{
			if (i < oldSize && i < newSize)
			{
				*mArray[i] = array[i];
			}
			else if (i >= newSize)
			{
				delete mArray.back();
				mArray.pop_back();
			}
			else /* (i >= oldSize) */
			{
				mArray.push_back(new ElementType(array[i]));
			}
		}
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::DeleteInternalArray()
	{
		for (typename InternalArray::iterator it = mArray.begin(); it != mArray.end(); ++it)
		{
			delete (*it);
		}
		mArray.clear();
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::LockUpdates()
	{
		AbstractValueEditor::LockUpdates();
		mButtonRevert->setEnabled(true);
		mButtonSave->setEnabled(true);
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::UnlockUpdates()
	{
		AbstractValueEditor::UnlockUpdates();

		mButtonRevert->setEnabled(false);
		mButtonSave->setEnabled(false);
	}
}

#endif // _ARRAYEDITORIMPL_H_