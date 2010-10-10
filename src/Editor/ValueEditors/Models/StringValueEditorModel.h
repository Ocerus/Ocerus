/// @file
/// Value editor model wrapper that converts values of specified type to strings

#ifndef _EDITOR_STRINGVALUEEDITORMODEL_H_
#define _EDITOR_STRINGVALUEEDITORMODEL_H_

#include "Base.h"
#include "IValueEditorModel.h"
#include "Utils/StringConverter.h"

namespace Editor
{
	template<class T>
	class StringValueEditorModel: public ITypedValueEditorModel<string>
	{
	public:
		typedef ITypedValueEditorModel<T> WrappedModel;
		
		StringValueEditorModel(WrappedModel* wrappedModel): mWrappedModel(wrappedModel) {}
		virtual ~StringValueEditorModel() { delete mWrappedModel; }

		/// Returns the name of the edited variable.
		virtual string GetName() const { return mWrappedModel->GetName(); }

		/// Returns the tool-tip for the edited variable.
		virtual string GetTooltip() const { return mWrappedModel->GetTooltip(); }

		/// Returns whether the variable is valid.
		virtual bool IsValid() const { return mWrappedModel->IsValid(); }

		/// Returns whether the variable is read only.
		virtual bool IsReadOnly() const { return mWrappedModel->IsReadOnly(); }

		/// Returns whether the variable is locked.
		virtual bool IsLocked() const { return mWrappedModel->IsLocked(); }

		/// Returns whether the variable is list element.
		virtual bool IsListElement() const { return mWrappedModel->IsListElement(); }

		/// Returns whether the variable is removable.
		virtual bool IsRemovable() const { return mWrappedModel->IsRemovable(); }

		/// Returns whether the variable is shareable.
		virtual bool IsShareable() const { return mWrappedModel->IsShareable(); }

		/// Returns whether the variable is shared.
		virtual bool IsShared() const { return mWrappedModel->IsShared(); }

		/// Removes the variable.
		virtual void Remove() { return mWrappedModel->Remove(); }

		/// Sets whether the variable is shared. This is used for prototype properties.
		virtual void SetShared(bool isShared) { return mWrappedModel->SetShared(isShared); }

		/// Returns the value of the variable.
		virtual string GetValue() const
		{
			return Utils::StringConverter::ToString(mWrappedModel->GetValue());
		}

		/// Sets the value of the variable.
		virtual void SetValue(const string& newValue)
		{
			mWrappedModel->SetValue(Utils::StringConverter::FromString<T>(newValue));
		}
		
		WrappedModel* GetWrappedModel() const { return mWrappedModel; }

	private:
		WrappedModel* mWrappedModel;
	};

       template<>
       string StringValueEditorModel<ResourceSystem::ResourcePtr>::GetValue() const;
}

#endif // _EDITOR_STRINGVALUEEDITORMODEL_H_