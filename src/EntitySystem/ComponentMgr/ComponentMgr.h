#ifndef _COMPONENTMGR_H_
#define _COMPONENTMGR_H_

#include <map>
#include "ComponentEnums.h"
#include "../EntityMgr/EntityHandle.h"

namespace EntitySystem
{

	class EntityComponentsIterator;
	class Component;
	typedef Component* (*ComponentCreationMethod)(void);

	class ComponentMgr : public Singleton<ComponentMgr>
	{
	public:
		ComponentMgr();
		~ComponentMgr();
		
		EntityComponentsIterator GetEntityComponents(EntityHandle h);
		void LoadResourcesForActiveComponents(); // calls LoadResource of all instantiated components
		void ReleaseResourcesForAllComponents();

	private:
		friend EntityComponentsIterator;

		// static component data
		ComponentCreationMethod mComponentCreationMethod[NUM_COMPONENT_TYPES];

		// dynamic component data
		std::map<EntityID, Component*> mEntityComponentMap[NUM_COMPONENT_TYPES];
	}
}

#endif