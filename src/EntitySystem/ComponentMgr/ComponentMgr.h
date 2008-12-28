#ifndef _COMPONENTMGR_H_
#define _COMPONENTMGR_H_

#include <map>
#include "../EntityMgr/EntityHandle.h"
#include "../../Utility/Singleton.h"
#include "../../Utility/Properties/PropertyHolderMediator.h"
#include "ComponentIterators.h"
#include "ComponentEnums.h"

namespace EntitySystem
{

	/// @name Forward declarations
	//@{
	class Component;
	//#}

	/// @name Type used for registration function individual components.
	typedef Component* (*ComponentCreationMethod)(void);

	/** This class manages instances of all entity components in the system.
	*/
	class ComponentMgr : public Singleton<ComponentMgr>
	{
	public:
		/// @name All components must be registered in this constructor, so don't forget to add new types there!
		ComponentMgr(void);
		~ComponentMgr(void);
		
		//TODO doplnit const pred EntityHandle

		/// @name Creates a component and attaches it to an entity.
		Component* CreateComponent(EntityHandle h, const eComponentType type);
		/// @name Destroys all components of a specified entity.
		//@{
		inline void DestroyEntityComponents(EntityHandle h) { DestroyEntityComponents(h.GetID()); }
		void DestroyEntityComponents(EntityID id);
		//@}
		/// @name Returns a collection of components (represented by an iterator) of a specified entity.
		//@{
		inline EntityComponentsIterator GetEntityComponents(EntityHandle h) { return GetEntityComponents(h.GetID()); }
		EntityComponentsIterator GetEntityComponents(EntityID id);
		//@}

		/// @name Retrieves properties of an entity. A filter related to access flags can be specified.
		//@{
		inline bool GetEntityProperties(const EntityHandle h, PropertyList& out, const PropertyAccessFlags flagMask = FULL_PROPERTY_ACCESS_FLAGS) { return GetEntityProperties(h.GetID(), out, flagMask); }
		bool GetEntityProperties(const EntityID id, PropertyList& out, const PropertyAccessFlags flagMask = FULL_PROPERTY_ACCESS_FLAGS);
		PropertyHolderMediator GetEntityProperty(const EntityHandle h, const StringKey key, const PropertyAccessFlags mask = FULL_PROPERTY_ACCESS_FLAGS);
		//@}

	private:
		typedef std::map<EntityID, ComponentsList*> EntityComponentsMap;

		/// @name Static component data.
		ComponentCreationMethod mComponentCreationMethod[NUM_COMPONENT_TYPES];

		/// @name Dynamic component data.
		EntityComponentsMap mEntityComponentsMap;

	};
}

#endif