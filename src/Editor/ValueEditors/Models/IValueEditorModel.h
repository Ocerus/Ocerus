/// @file
/// Declares an interface for value editor models.

#ifndef _IVALUEEDITORMODEL_H_
#define _IVALUEEDITORMODEL_H_

#include "Base.h"

namespace Editor
{
	/// The IValueEditorModel class is the basic interface for value editor models.
	class IValueEditorModel
	{
	public:
		/// Returns the name of the edited variable.
		virtual string GetName() const = 0;

		/// Returns the tool-tip for the edited variable.
		virtual string GetTooltip() const = 0;

		/// Returns whether the variable is valid.
		virtual bool IsValid() const = 0;

		/// Returns whether the variable is read only.
		virtual bool IsReadOnly() const = 0;

		/// Returns whether the variable is list element.
		virtual bool IsListElement() const = 0;

		/// Returns whether the variable is removable.
		virtual bool IsRemovable() const = 0;

		/// Removes the variable.
		virtual void Remove() = 0;
	};

	/// The ITypedValueEditorModel template class adds value-manipulation methods to
	/// IValueEditorModel according to its template parameter.
	template<class T>
	class ITypedValueEditorModel: public IValueEditorModel
	{
	public:
		/// Returns the value of the variable.
		virtual T GetValue() const = 0;

		/// Sets the value of the variable.
		virtual void SetValue(const T& newValue) = 0;
	};
}

#endif // _IVALUEEDITORMODEL_H_