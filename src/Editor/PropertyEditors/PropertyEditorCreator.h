/// @file
/// Defines property editors for different property types.

#ifndef _PROPERTYEDITORCREATOR_H_
#define _PROPERTYEDITORCREATOR_H_

#include "Base.h"
#include "IValueEditor.h"

namespace Editor
{
	/// Creates a property editor for given property.
	IValueEditor* CreatePropertyEditor(const PropertyHolder&);

	/// Creates an editor for EntityID.
	IValueEditor* CreateEntityIDEditor(const EntitySystem::EntityHandle&);

	/// Creates an editor for EntityName.
	IValueEditor* CreateEntityNameEditor(const EntitySystem::EntityHandle&);
}

#endif // _PROPERTYEDITORCREATOR_H_
