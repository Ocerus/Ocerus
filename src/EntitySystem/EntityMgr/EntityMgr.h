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
		inline EntityMessage::eResult PostMessage(EntityHandle h, const EntityMessage& msg) { return PostMessage(h.mEntityID, msg); }
		void BroadcastMessage(const EntityMessage& msg);
		void DestroyAllEntities(void);	


	private:
		typedef set<EntityID> EntitySet;

		ComponentMgr* mComponentMgr;
		EntitySet mEntitySet;

		EntityMessage::eResult PostMessage(EntityID id, const EntityMessage& msg);
	};
}

#endif