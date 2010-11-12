/// @file
/// Declares a model that allows to use ValueEditors to edit array elements.

#ifndef _EDITOR_ARRAYELEMENTMODEL_H_
#define _EDITOR_ARRAYELEMENTMODEL_H_

#include "Base.h"
#include "../ArrayEditor.h"
#include "IValueEditorModel.h"
#include "Utils/StringConverter.h"

namespace Editor
{
	template<class ElementType>
	class ArrayEditor;

	/// The ArrayElementModel class is a model for ValueEditors that allows to
	/// view/edit elements of arrays.
	template<class ElementType>
	class ArrayElementModel: public ITypedValueEditorModel<ElementType>
	{
	public:

		/// Constructs the model from the parrent array editor and index of the item it is modelling.
		ArrayElementModel(ArrayEditor<ElementType>* parentEditor, uint32 index): mParentEditor(parentEditor), mIndex(index) {}

		virtual string GetName() const { return Utils::StringConverter::ToString(mIndex) + ":"; }

		virtual string GetTooltip() const { return ""; }

		virtual bool IsValid() const { return true; }

		virtual bool IsReadOnly() const { return mParentEditor->IsReadOnly(); }

		virtual bool IsLocked() const { return mParentEditor->IsLocked(); }

		virtual bool IsListElement() const { return true; }

		virtual bool IsRemovable() const { return !IsReadOnly(); }

		virtual bool IsShareable() const { return false; }

		virtual bool IsShared() const { return false; }

		virtual ElementType GetValue() const { return mParentEditor->GetElement(mIndex); }

		virtual void SetValue(const ElementType& newValue) { mParentEditor->SetElement(mIndex, newValue); }

		virtual void Remove() { mParentEditor->RemoveElement(mIndex); }
		
		virtual void SetShared(bool isShared) { OC_UNUSED(isShared); OC_FAIL("ArrayElementModel does not support SetShared() operation."); }

	protected:
		ArrayEditor<ElementType>* mParentEditor;
		uint32 mIndex;
	};
}

#endif // _EDITOR_ARRAYELEMENTMODEL_H_