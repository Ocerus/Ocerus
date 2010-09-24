/// @file
/// Declares a model that allows to use ValueEditors to edit entity attributes.

#ifndef _ENTITYATTRIBUTEMODEL_H_
#define _ENTITYATTRIBUTEMODEL_H_


#include "Base.h"
#include "IValueEditorModel.h"
#include "EntitySystem/EntityMgr/EntityHandle.h"

namespace Editor
{
	/// The EntityAttributeModel class is a model for ValueEditors that allows to
	/// view/edit entity attributes, such as entity ID and entity name.
	class EntityAttributeModel: public ITypedValueEditorModel<string>
	{
	public:
		enum eAttributeType
		{
			TYPE_ID,
			TYPE_NAME,
			TYPE_TAG,
			TYPE_PROTOTYPE
		};

		/// Constructs a model for given entity and type of attribute.
		EntityAttributeModel(const EntitySystem::EntityHandle& entity, eAttributeType type);

		virtual string GetName() const;

		virtual string GetTooltip() const;

		virtual bool IsValid() const;

		virtual bool IsReadOnly() const;

		virtual bool IsLocked() const { return false; }

		virtual bool IsListElement() const { return false; }

		virtual bool IsRemovable() const;

		virtual bool IsShareable() const { return false; }

		virtual bool IsShared() const { return false; }

		virtual void Remove();

		virtual void SetShared(bool isShared) { OC_UNUSED(isShared); OC_FAIL("EntityAttributeModel does not support SetShared() operation."); }

		virtual string GetValue() const;

		virtual void SetValue(const string& newValue);

	private:
		EntitySystem::EntityHandle mEntity;
		eAttributeType mType;
	};
}

#endif // _ENTITYATTRIBUTEMODEL_H_