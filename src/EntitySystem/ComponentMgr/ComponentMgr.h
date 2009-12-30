/// @file
/// Manager of all entity components in the system.

#ifndef _COMPONENTMGR_H_
#define _COMPONENTMGR_H_

#include "Base.h"
#include "Singleton.h"
#include "ComponentIterators.h"
#include "ComponentID.h"

namespace EntitySystem
{
	/// Type used for a registration function of individual components.
	typedef Component* (*ComponentCreationMethod)(void);

	/// This class manages instances of all entity components in the system. Every entity consist only of these
	/// components plus some minor attributes.
	/// @remarks
	/// Components are registered automatically by taking their definitions from the ComponentTypes.h file.
	class ComponentMgr : public Singleton<ComponentMgr>
	{
	public:

		/// Constructor.
		ComponentMgr(void);

		/// Destructor.
		~ComponentMgr(void);
		
		/// Creates a component and attaches it to an entity.
		ComponentID CreateComponent(const EntityID id, const eComponentType type);

		/// Destroys a component of the entity.
		void DestroyComponent(const EntityID id, const ComponentID componentToDestroy);

		/// Destroys all components of a specified entity.
		void DestroyEntityComponents(const EntityID id);

		/// Returns a collection of components (represented by an iterator) of a specified entity.
		EntityComponentsIterator GetEntityComponents(const EntityID id) const;

		/// Returns a component of the given entity.
		Component* GetEntityComponent(const EntityID id, const ComponentID cmpID) const;

		/// Returns the number of components of an entity.
		int32 GetNumberOfEntityComponents(const EntityID id) const;

	private:

		typedef hash_map<EntityID, ComponentsList*> EntityComponentsMap;

		ComponentCreationMethod mComponentCreationMethod[NUM_COMPONENT_TYPES];
		EntityComponentsMap mEntityComponentsMap;

	};
}

#endif
