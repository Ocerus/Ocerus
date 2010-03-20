/// @file
/// Declares an interface for value editor models.

#ifndef _PROPERTYMODEL_H_
#define _PROPERTYMODEL_H_

#include "Base.h"
#include "IValueEditorModel.h"
#include "Utils/Properties/PropertyHolder.h"

namespace Editor
{
	template<class T>
	class PropertyModel: public ITypedValueEditorModel<T>
	{
	public:
		PropertyModel(const PropertyHolder& property): mProperty(property) {}
		
		virtual string GetName() const { return mProperty.GetName(); }

		virtual string GetTooltip() const { return mProperty.GetComment(); }

		virtual bool IsValid() const { return mProperty.IsValid(); }

		virtual bool IsReadOnly() const { return !(mProperty.GetAccessFlags() & Reflection::PA_EDIT_WRITE); }

		virtual bool IsListElement() const { return false; }

		virtual bool IsRemovable() const { return false; }

		virtual void Remove() { OC_FAIL("PropertyModel does not support Remove() operation."); }

		virtual T GetValue() const { return mProperty.GetValue<T>(); }

		virtual void SetValue(const T& newValue) { return mProperty.SetValue<T>(newValue); }

	private:
		PropertyHolder mProperty;
	};

	class StringPropertyModel: public ITypedValueEditorModel<string>
	{
	public:
		StringPropertyModel(const PropertyHolder& property): mProperty(property) {}

		virtual string GetName() const { return mProperty.GetName(); }

		virtual string GetTooltip() const { return mProperty.GetComment(); }

		virtual bool IsValid() const { return mProperty.IsValid(); }

		virtual bool IsReadOnly() const { return !(mProperty.GetAccessFlags() & Reflection::PA_EDIT_WRITE); }

		virtual bool IsListElement() const { return false; }

		virtual bool IsRemovable() const { return false; }

		virtual void Remove() { OC_FAIL("StringPropertyModel does not support Remove() operation."); }

		virtual string GetValue() const { return mProperty.GetValueString(); }

		virtual void SetValue(const string& newValue) { return mProperty.SetValueFromString(newValue); }

	private:
		PropertyHolder mProperty;
	};
}

#endif // 