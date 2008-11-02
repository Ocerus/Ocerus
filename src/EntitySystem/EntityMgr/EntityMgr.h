#ifndef _ENTITYMGR_H_
#define _ENTITYMGR_H_

#include <set>
#include "EntityDescription.h"
#include "EntityHandle.h"
#include "EntityMessage.h"
#include "../ComponentMgr/ComponentMgr.h"

/// Macro for easier use.
#define gEntityMgr EntitySystem::EntityMgr::GetSingleton()

namespace EntitySystem
{
	/** This class manages all game entities like weapons, enemy ships, projectiles, etc. Manipulation of entities is
		done via entity handles.
	*/
	class EntityMgr : public Singleton<EntityMgr>
	{
	public:
		EntityMgr(void);
		~EntityMgr(void);

		/// Creates new entity accorindgly to its description and returns its handle.
		EntityHandle CreateEntity(const EntityDescription& desc);
		/// Destroys a specified entity if it exists.
		void DestroyEntity(EntityHandle h);
		/// Posts a message to an entity. It is the only way entities can communicate with each other.
		inline EntityMessage::eResult PostMessage(EntityHandle h, const EntityMessage& msg) { return PostMessage(h.mEntityID, msg); }
		/// Sends a message to all entities.
		void BroadcastMessage(const EntityMessage& msg);
		/// Destroys all entities in the manager.
		void DestroyAllEntities(void);	


	private:
		typedef std::set<EntityID> EntitySet;

		ComponentMgr* mComponentMgr;
		EntitySet mEntitySet;

		/// Posts a message to an entity. It is the only way entities can communicate with each other.
		EntityMessage::eResult PostMessage(EntityID id, const EntityMessage& msg);
	};
}

#endif