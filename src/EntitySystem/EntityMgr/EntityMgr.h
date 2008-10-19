#ifndef _ENTITYMGR_H_
#define _ENTITYMGR_H_

#include "EntityHandle.h"
#include "EntityNode.h"
#include "EntityMessage.h"
#include "../ComponentMgr/ComponentMgr.h"

namespace EntitySystem
{
	class EntityMgr : public Singleton<EntityMgr>
	{
	public:
		EntityMgr();
		~EntityMgr();

		MessageHandle CreateEntity(const EntityDescription& desc);
		void DestroyEntity(MessageHandle h);
		void PostMessage(EntityHandle h, const EntityMessage& msg);
		void BroadcastMessage(const EntityMessage& msg);
		void RemoveAllEntities();	


	private:
		ComponentMgr* mComponentMgr;


}

#endif