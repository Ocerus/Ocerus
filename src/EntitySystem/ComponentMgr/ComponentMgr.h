#ifndef _COMPONENTMGR_H_
#define _COMPONENTMGR_H_

#include <map>
#include "ComponentEnums.h"
#include "Component.h"
#include "../EntityMgr/EntityHandle.h"
#include "../../Utility/Singleton.h"

#define gComponentMgr EntitySystem::ComponentMgr::GetSingleton()

namespace EntitySystem
{
	class EntityComponentsIterator;
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
		map<EntityID, Component*> mEntityComponentMap[NUM_COMPONENT_TYPES];
	};
}

#endif