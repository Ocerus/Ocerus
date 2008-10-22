#ifndef _COMPONENTMGR_H_
#define _COMPONENTMGR_H_

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
		ComponentMgr(void);
		~ComponentMgr(void);
		
		typedef vector<Component*> ComponentsList;

		ComponentsList& GetEntityComponents(EntityHandle h);

		void LoadResourcesForActiveComponents(void); 
		void ReleaseResourcesForAllComponents(void);

	private:
		friend EntityComponentsIterator;

		typedef map<EntityID, ComponentsList> EntityComponentsMap;

		// static component data
		ComponentCreationMethod mComponentCreationMethod[NUM_COMPONENT_TYPES];

		// dynamic component data
		EntityComponentsMap mEntityComponentsMap;
	};
}

#endif