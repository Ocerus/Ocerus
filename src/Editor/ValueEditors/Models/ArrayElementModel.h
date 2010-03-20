/// @file
/// Declares a model that allows to use ValueEditors to edit entity attributes.

#ifndef _ARRAYELEMENTMODEL_H_
#define _ARRAYELEMENTMODEL_H_

#include "Base.h"
#include "IValueEditorModel.h"
#include "Utils/StringConverter.h"



namespace Editor
{
	template<class ElementType>
	class ArrayEditor;

	/// The ArrayElementModel class is a model for ValueEditors that allows to
	/// view/edit entity attributes, such as entity ID and entity name.
	template<class ElementType>
	class ArrayElementModel: public ITypedValueEditorModel<ElementType>
	{
	public:
		/// 
		ArrayElementModel(ArrayEditor<ElementType>* parentEditor, uint32 index): mParentEditor(parentEditor), mIndex(index) {}

		virtual string GetName() const { return Utils::StringConverter::ToString(mIndex) + ":"; }

		virtual string GetTooltip() const { return ""; }

		virtual bool IsValid() const { return true; }

		virtual bool IsReadOnly() const { return false; }

		virtual bool IsListElement() const { return true; }

		virtual bool IsRemovable() const { return !IsReadOnly(); }

		virtual ElementType GetValue() const;

		virtual void SetValue(const ElementType& newValue);

		virtual void Remove();

	private:
		ArrayEditor<ElementType>* mParentEditor;
		uint32 mIndex;
	};

	template<class ElementType>
	class ArrayStringElementModel: public ITypedValueEditorModel<string>
	{
	public:

		///
		ArrayStringElementModel(ArrayEditor<ElementType>* parentEditor, uint32 index): mParentEditor(parentEditor), mIndex(index) {}

		virtual string GetName() const { return Utils::StringConverter::ToString(mIndex + 1) + ":"; }

		virtual string GetTooltip() const { return ""; }

		virtual bool IsValid() const { return true; }

		virtual bool IsReadOnly() const { return false; }

		virtual bool IsListElement() const { return true; }

		virtual bool IsRemovable() const { return !IsReadOnly(); }

		virtual string GetValue() const;

		virtual void SetValue(const string& newValue);

		virtual void Remove();

	private:
		ArrayEditor<ElementType>* mParentEditor;
		uint32 mIndex;
	};

}

#endif // _ARRAYELEMENTMODEL_H_