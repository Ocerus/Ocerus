#ifndef _COMPONENTMGR_H_
#define _COMPONENTMGR_H_

#include <map>
#include "ComponentEnums.h"
#include "Component.h"
#include "ComponentDescription.h"
#include "ComponentIterators.h"
#include "../EntityMgr/EntityHandle.h"
#include "../../Utility/Singleton.h"

namespace EntitySystem
{
	typedef Component* (*ComponentCreationMethod)(void);

	/** This class manages instances of all entity components in the system.
	*/
	class ComponentMgr : public Singleton<ComponentMgr>
	{
	public:
		/// All components must be registered in this constructor, so don't forget to add new types there!
		ComponentMgr(void);
		~ComponentMgr(void);
		
		/// Creates a component and attaches it to an entity.
		bool CreateComponent(EntityHandle h, const ComponentDescription& desc);
		/// Destroys all components of a specified entity.
		//@{
		inline void DestroyEntityComponents(EntityHandle h) { DestroyEntityComponents(h.mEntityID); }
		void DestroyEntityComponents(EntityID id);
		//@}
		/// Returns a collection of components (represented by an iterator) of a specified entity.
		//@{
		EntityComponentsIterator GetEntityComponents(EntityHandle h) { return GetEntityComponents(h.mEntityID); }
		EntityComponentsIterator GetEntityComponents(EntityID id);
		//@}

	private:
		typedef std::map<EntityID, ComponentsList> EntityComponentsMap;

		/// Static component data.
		ComponentCreationMethod mComponentCreationMethod[NUM_COMPONENT_TYPES];

		/// Dynamic component data.
		EntityComponentsMap mEntityComponentsMap;

	};
}

#endif