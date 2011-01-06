/// @file
/// Declares an interface for value editor models.

#ifndef _EDITOR_PROPERTYMODEL_H_
#define _EDITOR_PROPERTYMODEL_H_

#include "Base.h"
#include "IValueEditorModel.h"
#include "Editor/EditorMgr.h"
#include "Utils/Properties/PropertyHolder.h"
#include "Utils/ResourcePointers.h"
#include "EntitySystem/EntityMgr/EntityMgr.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"

namespace Editor
{
	/// The PropertyModel template class is a ValueEditorModel that manages a property of given template parameter type.
	template<class T>
	class PropertyModel: public ITypedValueEditorModel<T>
	{
	public:
		/// Constructs a new ProperyModel that manages given property.
		PropertyModel(const PropertyHolder& property, const EntitySystem::EntityHandle& entity = EntitySystem::EntityHandle()): mProperty(property), mEntity(entity) { PROFILE_FNC(); }

		/// Returns the name of the property.
		virtual string GetName() const { return mProperty.GetName(); }

		/// Returns the comment of the property.
		virtual string GetTooltip() const { return mProperty.GetComment(); }

		/// Returns whether the property is valid.
		virtual bool IsValid() const { return mProperty.IsValid(); }

		/// Returns whether the property is read-only.
		virtual bool IsReadOnly() const
		{
			return !(mProperty.GetAccessFlags() & Reflection::PA_EDIT_WRITE) || IsLocked();
		}

		/// Returns whether the variable is locked.
		virtual bool IsLocked() const
		{
			return gEntityMgr.IsEntityLinkedToPrototype(mEntity) &&
				gEntityMgr.IsPrototypePropertyShared(gEntityMgr.GetEntityPrototype(mEntity), mProperty.GetKey());
		}

		/// Returns false.
		virtual bool IsListElement() const { return false; }

		/// Returns false.
		virtual bool IsRemovable() const { return false; }

		/// Returns whether the property belongs to a prototype and thus is shareable.
		virtual bool IsShareable() const
		{ 
			bool shareable = mEntity.IsValid();
			shareable = shareable && mEntity.Exists();
			shareable = shareable && gEntityMgr.IsEntityPrototype(mEntity);
			shareable = shareable && (mProperty.GetAccessFlags()&PA_INIT)!=0;
			return shareable;
		}

		/// Returns whether the property is shared.
		virtual bool IsShared() const { return IsShareable() && gEntityMgr.IsPrototypePropertyShared(mEntity, mProperty.GetKey()); }

		/// You must not call this method, as removing of properties is not implemented.
		virtual void Remove() { OC_FAIL("PropertyModel does not support Remove() operation."); }

		virtual void SetShared(bool isShared)
		{
			OC_ASSERT(IsShareable());
			if (isShared)
			{
				gEntityMgr.SetPrototypePropertyShared(mEntity, mProperty.GetKey());
			}
			else
			{
				gEntityMgr.SetPrototypePropertyNonShared(mEntity, mProperty.GetKey());
			}
			gEditorMgr.PropertyValueChanged();
		}

		/// Returns the value of the property.
		virtual T GetValue() const { return mProperty.GetValue<T>(); }

		/// Sets the newValue to the property.
		virtual void SetValue(const T& newValue)\
		{
			mProperty.SetValue<T>(newValue);
			gEditorMgr.PropertyValueChanged();
		}

	protected:
		PropertyHolder mProperty;
		EntitySystem::EntityHandle mEntity;
	};


	/// The StringPropertyModel class is a ValueEditorModel that manages a string property. This class converts property
	/// values to strings, so it works with editors that manages strings.
	class StringPropertyModel: public PropertyModel<string>
	{
	public:
		/// Constructs a new StringProperyModel that manages given property.
		StringPropertyModel(const PropertyHolder& property, const EntitySystem::EntityHandle& entity = EntitySystem::EntityHandle()): PropertyModel<string>(property, entity) {}

		/// Returns the string representation of the property value.
		virtual string GetValue() const { return mProperty.GetValueString(); }

		/// Sets the property value from string representation in newValue.
		virtual void SetValue(const string& newValue)
		{
			mProperty.SetValueFromString(newValue);
			gEditorMgr.PropertyValueChanged();
		}
	};


	/// The LayerPropertyModel template class is a ValueEditorModel that manages a layer property of an entity.
	class LayerPropertyModel: public PropertyModel<string>
	{
	public:
	
		/// Constructs the model from a property corresponding to the entity layer and the entity itself. The entity can be null.
		LayerPropertyModel(const PropertyHolder& property, const EntitySystem::EntityHandle& entity = EntitySystem::EntityHandle()): PropertyModel<string>(property, entity)
		{
			OC_DASSERT(property.GetType() == PT_INT32);
		}

		/// Returns the string representation of the property value.
		virtual string GetValue() const
		{ 
			EntitySystem::LayerID layerID = mProperty.GetValue<int32>();
			if (gLayerMgr.ExistsLayer(layerID))
				return gLayerMgr.GetLayerName(layerID);
			return "";
		}

		virtual void SetValue(const string&) {}

		virtual bool IsReadOnly() const { return true; }

		virtual bool IsShareable() const { return false; }
	};
}

#endif // _EDITOR_PROPERTYMODEL_H_