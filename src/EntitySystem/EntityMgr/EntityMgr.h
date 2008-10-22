#ifndef _ENTITYMGR_H_
#define _ENTITYMGR_H_

#include "EntityDescription.h"
#include "EntityHandle.h"
#include "EntityMessage.h"
#include "../ComponentMgr/ComponentMgr.h"

#define gEntityMgr EntitySystem::EntityMgr::GetSingleton()

namespace EntitySystem
{
	class EntityMgr : public Singleton<EntityMgr>
	{
	public:
		EntityMgr(void);
		~EntityMgr(void);

		EntityHandle CreateEntity(const EntityDescription& desc);
		void DestroyEntity(EntityHandle h);
		void PostMessage(EntityHandle h, const EntityMessage& msg);
		void BroadcastMessage(const EntityMessage& msg);
		void RemoveAllEntities(void);	


	private:
		ComponentMgr* mComponentMgr;

	};
}

#endif