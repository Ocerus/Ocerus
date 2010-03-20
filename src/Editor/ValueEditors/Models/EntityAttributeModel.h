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
			TYPE_NAME
		};

		/// Constructs a model for given entity and type of attribute.
		EntityAttributeModel(const EntitySystem::EntityHandle& entity, eAttributeType type);

		virtual string GetName() const;

		virtual string GetTooltip() const;

		virtual bool IsValid() const;

		virtual bool IsReadOnly() const;

		virtual bool IsListElement() const { return false; }

		virtual bool IsRemovable() const { return false; }

		virtual void Remove() { OC_FAIL("EnitityAttributeModel does not support Remove() operation."); }

		virtual string GetValue() const;

		virtual void SetValue(const string& newValue);

	private:
		EntitySystem::EntityHandle mEntity;
		eAttributeType mType;
	};
}

#endif // _ENTITYATTRIBUTEMODEL_H_