/// @file
/// Template method implementations of ArrayElementModel.
/// Those implementations need to be in separate file to avoid header cyclic dependency with ArrayEditor.h

#ifndef _ARRAYELEMENTMODELIMPL_H_
#define _ARRAYELEMENTMODELIMPL_H_

#include "ArrayElementModel.h"
#include "../ArrayEditor.h"

namespace Editor
{

	template<class ElementType>
	ElementType ArrayElementModel<ElementType>::GetValue() const
	{
		return mParentEditor->GetElement(mIndex);
	}

	template<class ElementType>
	void ArrayElementModel<ElementType>::SetValue(const ElementType& newValue)
	{
		mParentEditor->SetElement(mIndex, newValue);
	}

	template<class ElementType>
	void ArrayElementModel<ElementType>::Remove()
	{
		mParentEditor->RemoveElement(mIndex);
	}

	template<class ElementType>
	string ArrayStringElementModel<ElementType>::GetValue() const
	{
		return Utils::StringConverter::ToString(mParentEditor->GetElement(mIndex));
	}

	template<class ElementType>
	void ArrayStringElementModel<ElementType>::SetValue(const string& newValue)
	{
		mParentEditor->SetElement(mIndex, Utils::StringConverter::FromString<ElementType>(newValue));
	}

	template<class ElementType>
	void ArrayStringElementModel<ElementType>::Remove()
	{
		mParentEditor->RemoveElement(mIndex);
	}
}

#endif // _ARRAYELEMENTMODELIMPL_H_