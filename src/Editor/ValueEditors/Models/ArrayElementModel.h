/// @file
/// Declares a model that allows to use ValueEditors to edit entity attributes.

#ifndef _ARRAYELEMENTMODEL_H_
#define _ARRAYELEMENTMODEL_H_

#include "Base.h"
#include "IValueEditorModel.h"
#include "Utils/StringConverter.h"

namespace Editor
{
	/// The ArrayElementModel class is a model for ValueEditors that allows to
	/// view/edit entity attributes, such as entity ID and entity name.
	template<class ElementType>
	class ArrayElementModel: public ITypedValueEditorModel<ElementType>
	{
	public:

		/// 
		ArrayElementModel(ElementType* element, int index): mElement(element), mIndex(index) {}

		virtual string GetName() const { return Utils::StringConverter::ToString(mIndex) + ":"; }

		virtual string GetTooltip() const { return ""; }

		virtual bool IsValid() const { return true; }

		virtual bool IsReadOnly() const { return false; }

		virtual bool IsListElement() const { return true; }

		virtual ElementType GetValue() const { return *mElement; }

		virtual void SetValue(const ElementType& newValue) { *mElement = newValue; }

	private:
		ElementType* mElement;
		int mIndex;
	};

	template<class ElementType>
	class ArrayStringElementModel: public ITypedValueEditorModel<string>
	{
	public:

		///
		ArrayStringElementModel(ElementType* element, int index): mElement(element), mIndex(index) {}

		virtual string GetName() const { return Utils::StringConverter::ToString(mIndex + 1) + ":"; }

		virtual string GetTooltip() const { return ""; }

		virtual bool IsValid() const { return true; }

		virtual bool IsReadOnly() const { return false; }

		virtual bool IsListElement() const { return true; }

		virtual string GetValue() const { return Utils::StringConverter::ToString(*mElement); }

		virtual void SetValue(const string& newValue) { *mElement = Utils::StringConverter::FromString<ElementType>(newValue); }

	private:
		ElementType* mElement;
		int mIndex;
	};

}

#endif // _ARRAYELEMENTMODEL_H_