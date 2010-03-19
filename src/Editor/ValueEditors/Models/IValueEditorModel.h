/// @file
/// Declares an interface for value editor models.

#ifndef _IVALUEEDITORMODEL_H_
#define _IVALUEEDITORMODEL_H_

#include "Base.h"

namespace Editor
{
	class IValueEditorModel
	{
	public:

		virtual string GetName() const = 0;

		virtual string GetTooltip() const = 0;

		virtual bool IsValid() const = 0;

		virtual bool IsReadOnly() const = 0;

		virtual bool IsListElement() const = 0;
	};

	template<class T>
	class ITypedValueEditorModel: public IValueEditorModel
	{
	public:

		virtual T GetValue() const = 0;

		virtual void SetValue(const T& newValue) = 0;
	};
}

#endif // _IVALUEEDITORMODEL_H_