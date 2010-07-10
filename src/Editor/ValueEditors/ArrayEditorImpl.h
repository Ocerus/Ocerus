/// @file
/// Template method implementations of ArrayEditor.

#ifndef _ARRAYEDITORIMPL_H_
#define _ARRAYEDITORIMPL_H_

#include "ArrayEditor.h"
#include "Models/ArrayElementModel.h"
#include "StringEditor.h"
#include "BoolEditor.h"
#include "Vector2Editor.h"
#include "PointEditor.h"
#include "ResourceEditor.h"
#include "GUISystem/VerticalLayout.h"
#include "Properties/PropertyEnums.h"

namespace Editor
{
	/// Creates a value editor for given ElementType. Default implementation creates a string editor.
	/// @todo It would be nice to join this code with election mechanism in PropertyEditorCreator.cpp,
	///       that is used for PropertyEditors, although it might require some template hacking.
	template<class ElementType>
	AbstractValueEditor* CreateArrayElementEditor(ArrayEditor<ElementType>* parentEditor, uint32 index)
	{
		return new Editor::StringEditor(new ArrayStringElementModel<ElementType>(parentEditor, index));
	}

	template<>
	AbstractValueEditor* CreateArrayElementEditor<bool>(ArrayEditor<bool>* parentEditor, uint32 index)
	{
		return new Editor::BoolEditor(new ArrayElementModel<bool>(parentEditor, index));
	}
	
	template<>
	AbstractValueEditor* CreateArrayElementEditor<Vector2>(ArrayEditor<Vector2>* parentEditor, uint32 index)
	{
		return new Editor::Vector2Editor(new ArrayElementModel<Vector2>(parentEditor, index));
	}
	
	template<>
	AbstractValueEditor* CreateArrayElementEditor<GfxSystem::Point>(ArrayEditor<GfxSystem::Point>* parentEditor, uint32 index)
	{
		return new Editor::PointEditor(new ArrayElementModel<GfxSystem::Point>(parentEditor, index));
	}
	
	template<>
	AbstractValueEditor* CreateArrayElementEditor<ResourceSystem::ResourcePtr>(ArrayEditor<ResourceSystem::ResourcePtr>* parentEditor, uint32 index)
	{
		return new Editor::ResourceEditor(new ArrayElementModel<ResourceSystem::ResourcePtr>(parentEditor, index));
	}
	
	template<class ElementType>
	CEGUI::Window* ArrayEditor<ElementType>::CreateWidget(const CEGUI::String& namePrefix)
	{
		PROFILE_FNC();

		OC_ASSERT(mEditorWidget == 0);
		mNamePrefix = namePrefix;

		/// Create main editor widget
		mEditorWidget = gCEGUIWM.createWindow("DefaultWindow", namePrefix);
		mLayout = new GUISystem::VerticalLayout(mEditorWidget, 0, true);


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
		return mEditorWidget;
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
		gEditorMgr.PropertyValueChanged();
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
			while (mArray.size() < mElementEditors.size())
			{
				delete mElementEditors.back();
				mElementEditors.pop_back();
			}

			for (uint32 i = mElementEditors.size(); i < mArray.size(); ++i)
			{
				AbstractValueEditor* editor = CreateArrayElementEditor(this, i);
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