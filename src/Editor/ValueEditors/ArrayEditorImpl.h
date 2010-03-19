/// @file
/// Template method implementations of ArrayEditor.

#ifndef _ARRAYEDITORIMPL_H_
#define _ARRAYEDITORIMPL_H_

#include "Models/ArrayElementModel.h"
#include "StringEditor.h"
#include "GUISystem/VerticalLayout.h"

namespace Editor
{

	template<class ElementType>
	CEGUI::Window* ArrayEditor<ElementType>::CreateWidget(const CEGUI::String& namePrefix)
	{
		OC_ASSERT(mWidget == 0);
		mNamePrefix = namePrefix;

		/// Create main editor widget
		mWidget = gCEGUIWM.createWindow("DefaultWindow", namePrefix);
		mWidget->setHeight(CEGUI::UDim(0, 400));

		/// Update editor and return main widget
		Update();
		return mWidget;
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::Update()
	{
		UpdateArray();

		/// Clear all the content of Entity Editor
		for (int i = (mWidget->getChildCount() - 1); i >= 0; --i)
		{
			gCEGUIWM.destroyWindow(mWidget->getChildAtIdx(i));
		}

		GUISystem::VerticalLayout* layout = new GUISystem::VerticalLayout(mWidget, 0, true);

		/// Create label widget of the editor
		CEGUI::Window* labelWidget = this->CreateEditorLabelWidget(mNamePrefix + "/Label", mModel);
		labelWidget->setArea(CEGUI::URect(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1, 0), CEGUI::UDim(0, GetEditboxHeight())));
		layout->AddChildWindow(labelWidget);

		uint32 i = 0;
		for (typename InternalArray::iterator it = mArray.begin(); it != mArray.end(); ++it, ++i)
		{
			AbstractValueEditor* editor = new Editor::StringEditor(new ArrayStringElementModel<ElementType>(*it, i));
			mElementEditors.push_back(editor);
			layout->AddChildWindow(editor->CreateWidget(mNamePrefix + "/Editor" + Utils::StringConverter::ToString(i)));
		}
		layout->UpdateLayout();
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::UpdateArray()
	{
		ClearArray();
		const ArrayType& array = *mModel->GetValue();
		for (int32 i = 0; i < array.GetSize(); ++i)
		{
			mArray.push_back(new ElementType(array[i]));
		}
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::SubmitArray()
	{
		ArrayType newArray(mArray.size());
		int32 i = 0;
		for (typename InternalArray::iterator it = mArray.begin(); it != mArray.end(); ++i, ++it)
		{
			newArray[i] = **it;
		}
		mModel->SetValue(&newArray);
	}

	template<class ElementType>
	void ArrayEditor<ElementType>::ClearArray()
	//void ArrayEditor::ClearArray()
	{
		for (typename InternalArray::iterator it = mArray.begin(); it != mArray.end(); ++it)
		{
			delete (*it);
		}
		mArray.clear();
	}
}

#endif // _ARRAYEDITORIMPL_H_