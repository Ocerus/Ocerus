/// @file
/// Declares an interface for value editor models.

#ifndef _PROPERTYMODEL_H_
#define _PROPERTYMODEL_H_

#include "Base.h"
#include "IValueEditorModel.h"
#include "Utils/Properties/PropertyHolder.h"

namespace Editor
{
	/// The PropertyModel template class is a ValueEditorModel that manages a property of given template parameter type.
	template<class T>
	class PropertyModel: public ITypedValueEditorModel<T>
	{
	public:
		/// Constructs a new ProperyModel that manages given property.
		PropertyModel(const PropertyHolder& property): mProperty(property) { PROFILE_FNC(); }

		/// Returns the name of the property.
		virtual string GetName() const { return mProperty.GetName(); }

		/// Returns the comment of the property.
		virtual string GetTooltip() const { return mProperty.GetComment(); }

		/// Returns whether the property is valid.
		virtual bool IsValid() const { return mProperty.IsValid(); }

		/// Returns whether the property is read-only.
		virtual bool IsReadOnly() const { return !(mProperty.GetAccessFlags() & Reflection::PA_EDIT_WRITE); }

		/// Returns false.
		virtual bool IsListElement() const { return false; }

		/// Returns false.
		virtual bool IsRemovable() const { return false; }

		/// You must not call this method, as removing of properties is not implemented.
		virtual void Remove() { OC_FAIL("PropertyModel does not support Remove() operation."); }

		/// Returns the value of the property.
		virtual T GetValue() const { return mProperty.GetValue<T>(); }

		/// Sets the newValue to the property.
		virtual void SetValue(const T& newValue) { return mProperty.SetValue<T>(newValue); }

	private:
		PropertyHolder mProperty;
	};

	/// The StringPropertyModel class is a ValueEditorModel that manages a property. This class converts property
	/// values to strings, so it works with editors that manages strings.
	class StringPropertyModel: public ITypedValueEditorModel<string>
	{
	public:
		/// Constructs a new StringProperyModel that manages given property.
		StringPropertyModel(const PropertyHolder& property): mProperty(property) {}

		/// Returns the name of the property.
		virtual string GetName() const { return mProperty.GetName(); }

		/// Returns the comment of the property.
		virtual string GetTooltip() const { return mProperty.GetComment(); }

		/// Returns whether the property is valid.
		virtual bool IsValid() const { return mProperty.IsValid(); }

		/// Returns whether the property is read-only.
		virtual bool IsReadOnly() const { return !(mProperty.GetAccessFlags() & Reflection::PA_EDIT_WRITE); }

		/// Returns false.
		virtual bool IsListElement() const { return false; }

		/// Returns false.
		virtual bool IsRemovable() const { return false; }

		/// You must not call this method, as removing of properties is not implemented.
		virtual void Remove() { OC_FAIL("StringPropertyModel does not support Remove() operation."); }

		/// Returns the string representation of the property value.
		virtual string GetValue() const { return mProperty.GetValueString(); }

		/// Sets the property value from string representation in newValue.
		virtual void SetValue(const string& newValue) { return mProperty.SetValueFromString(newValue); }

	private:
		PropertyHolder mProperty;
	};
}

#endif // 