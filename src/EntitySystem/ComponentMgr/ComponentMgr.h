#ifndef _COMPONENTMGR_H_
#define _COMPONENTMGR_H_

#include "ComponentEnums.h"
#include "Component.h"
#include "ComponentDescription.h"
#include "ComponentIterators.h"
#include "../EntityMgr/EntityHandle.h"
#include "../../Utility/Singleton.h"

#define gComponentMgr EntitySystem::ComponentMgr::GetSingleton()

namespace EntitySystem
{
	typedef Component* (*ComponentCreationMethod)(void);

	class ComponentMgr : public Singleton<ComponentMgr>
	{
	public:
		ComponentMgr(void);
		~ComponentMgr(void);
		
		bool CreateComponent(EntityHandle h, const ComponentDescription& desc);
		inline void DestroyEntityComponents(EntityHandle h) { DestroyEntityComponents(h.mEntityID); }
		void DestroyEntityComponents(EntityID id);
		EntityComponentsIterator GetEntityComponents(EntityHandle h) { return GetEntityComponents(h.mEntityID); }
		EntityComponentsIterator GetEntityComponents(EntityID id);

	private:
		typedef map<EntityID, ComponentsList> EntityComponentsMap;

		// static component data
		ComponentCreationMethod mComponentCreationMethod[NUM_COMPONENT_TYPES];

		// dynamic component data
		EntityComponentsMap mEntityComponentsMap;

	};
}

#endif