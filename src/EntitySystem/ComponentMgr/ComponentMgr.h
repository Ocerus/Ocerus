/// @file
/// Manager of all entity components in the system.

#ifndef _COMPONENTMGR_H_
#define _COMPONENTMGR_H_

#include "Base.h"
#include "Singleton.h"
#include "ComponentIterators.h"

/// @brief Entity system manages game entities and their parts. 
/// @remarks However, it takes care of their logical representation and
/// leaves optimizing of other tasks to other system. For example, the rendering system can use an optimization data
/// structure for faster visibility culling of entities.
namespace EntitySystem
{
	/// Type used for a registration function of individual components.
	typedef Component* (*ComponentCreationMethod)(void);

	/// @brief This class manages instances of all entity components in the system. Every entity consist only of these
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
		Component* CreateComponent(const EntityHandle h, const eComponentType type);

		/// Destroys all components of a specified entity.
		inline void DestroyEntityComponents(const EntityHandle h) { DestroyEntityComponents(h.GetID()); }

		/// Destroys all components of a specified entity.
		void DestroyEntityComponents(const EntityID id);

		/// Returns a collection of components (represented by an iterator) of a specified entity.
		inline EntityComponentsIterator GetEntityComponents(const EntityHandle h) const { return GetEntityComponents(h.GetID()); }

		/// Returns a collection of components (represented by an iterator) of a specified entity.
		EntityComponentsIterator GetEntityComponents(const EntityID id) const;

		/// Retrieves properties of an entity. A filter related to access flags can be specified.
		inline bool GetEntityProperties(const EntityHandle h, PropertyList& out, const PropertyAccessFlags flagMask = FULL_PROPERTY_ACCESS_FLAGS) { return GetEntityProperties(h.GetID(), out, flagMask); }

		/// Retrieves properties of an entity. A filter related to access flags can be specified.
		bool GetEntityProperties(const EntityID id, PropertyList& out, const PropertyAccessFlags flagMask = FULL_PROPERTY_ACCESS_FLAGS);

		/// Retrieves properties of an entity. A filter related to access flags can be specified.
		PropertyHolderMediator GetEntityProperty(const EntityHandle h, const StringKey key, const PropertyAccessFlags mask = FULL_PROPERTY_ACCESS_FLAGS) const;

	private:

		typedef stdext::hash_map<EntityID, ComponentsList*> EntityComponentsMap;

		ComponentCreationMethod mComponentCreationMethod[NUM_COMPONENT_TYPES];
		EntityComponentsMap mEntityComponentsMap;

	};
}

#endif