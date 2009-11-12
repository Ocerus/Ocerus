/// @file
/// Manager of all entity components in the system.

#ifndef _COMPONENTMGR_H_
#define _COMPONENTMGR_H_

#include "Base.h"
#include "Singleton.h"
#include "ComponentIterators.h"
#include "ComponentID.h"

/// Entity system manages game entities and their parts. 
/// However, it takes care of their logical representation and
/// leaves optimizing of other tasks to other system. For example, the rendering system can use an optimization data
/// structure for faster visibility culling of entities.
namespace EntitySystem
{
	/// Type used for a registration function of individual components.
	typedef Component* (*ComponentCreationMethod)(void);

	/// This class manages instances of all entity components in the system. Every entity consist only of these
	/// components plus some minor attributes.
	/// @remarks
	/// Components are registered automatically by taking their definitions from the ComponentTypes.h file. Apart
	/// from that you can register them manually by calling appropriate methods.
	class ComponentMgr : public Singleton<ComponentMgr>
	{
	public:

		ComponentMgr(void);
		~ComponentMgr(void);
		
		/// Creates a component and attaches it to an entity.
		inline ComponentID CreateComponent(const EntityHandle h, const eComponentType type) { return CreateComponent(h.GetID(), type); }

		/// Creates a component and attaches it to an entity.
		ComponentID CreateComponent(const EntityID id, const eComponentType type);

		/// Destroys a component of the entity.
		void DestroyComponent(const EntityID id, const ComponentID componentToDestroy);

		/// Destroys all components of a specified entity.
		inline void DestroyEntityComponents(const EntityHandle h) { DestroyEntityComponents(h.GetID()); }

		/// Destroys all components of a specified entity.
		void DestroyEntityComponents(const EntityID id);

		/// Returns a collection of components (represented by an iterator) of a specified entity.
		inline EntityComponentsIterator GetEntityComponents(const EntityHandle h) const { return GetEntityComponents(h.GetID()); }

		/// Returns a collection of components (represented by an iterator) of a specified entity.
		EntityComponentsIterator GetEntityComponents(const EntityID id) const;

		/// Returns a component of the given entity.
		inline Component* GetEntityComponent(const EntityHandle h, const ComponentID cmpID) const { return GetEntityComponent(h.GetID(), cmpID); }

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
