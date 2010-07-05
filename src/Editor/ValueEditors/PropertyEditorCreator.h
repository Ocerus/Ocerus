/// @file
/// Defines property editors for different property types.

#ifndef _PROPERTYEDITORCREATOR_H_
#define _PROPERTYEDITORCREATOR_H_

#include "Base.h"
#include "AbstractValueEditor.h"

namespace Editor
{
	/// Creates a property editor for given property.
	AbstractValueEditor* CreatePropertyEditor(const PropertyHolder&);

	/// Creates an editor for EntityID.
	AbstractValueEditor* CreateEntityIDEditor(const EntitySystem::EntityHandle&);

	/// Creates an editor for the name of the entity.
	AbstractValueEditor* CreateEntityNameEditor(const EntitySystem::EntityHandle&);

	/// Creates an editor for the user tag of the entity.
	AbstractValueEditor* CreateEntityTagEditor(const EntitySystem::EntityHandle&);
}

#endif // _PROPERTYEDITORCREATOR_H_
