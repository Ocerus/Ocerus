/// @file
/// Defines property editors for different property types.

#ifndef _PROPERTYEDITORCREATOR_H_
#define _PROPERTYEDITORCREATOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"

namespace Editor
{
//	template<class Model>
//	IValueEditor* CreateValueEditor(const Model& model, const PropertyHolder&);

	/// Creates a property editor for given property.
	AbstractValueEditor* CreatePropertyEditor(const PropertyHolder&);

	/// Creates an editor for EntityID.
	AbstractValueEditor* CreateEntityIDEditor(const EntitySystem::EntityHandle&);

	/// Creates an editor for EntityName.
	AbstractValueEditor* CreateEntityNameEditor(const EntitySystem::EntityHandle&);
}

#endif // _PROPERTYEDITORCREATOR_H_
