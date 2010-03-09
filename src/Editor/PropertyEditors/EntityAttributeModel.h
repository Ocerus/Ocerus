/// @file
/// Declares a model that allows to use ValueEditors to edit entity attributes.

#ifndef _ENTITYATTRIBUTEMODEL_H_
#define _ENTITYATTRIBUTEMODEL_H_

#include "Base.h"
#include "Utils/Properties/PropertyAccess.h"

namespace Editor
{
	/// The EntityAttributeModel class is a model for ValueEditors that allows to
	/// view/edit entity attributes, such as entity ID and entity name.
	class EntityAttributeModel
	{
	public:
		enum eAttrType
		{
			TYPE_ID,
			TYPE_NAME
		};

		/// Constructs a model for given entity and type of attribute.
		EntityAttributeModel(const EntitySystem::EntityHandle& entity, eAttrType type);

		/// Returns the access flags to this attribute.
		Reflection::PropertyAccessFlags GetAccessFlags() const;

		/// Returns the key representing this attribute.
		//StringKey GetKey() const;

		/// Returns the attribute value.
		string GetValueString();

		/// Sets the attribute value.
		void SetValueFromString(const string& str);

		/// Returns the name of this attribute.
		const char* GetName() const;

		/// Returns the comment of this attribute.
		string GetComment() const;

		/// Returns whether this attribute is valid.
		bool IsValid() const;

	private:
		EntitySystem::EntityHandle mEntity;
		eAttrType mType;
	};
}

#endif // _ENTITYATTRIBUTEMODEL_H_