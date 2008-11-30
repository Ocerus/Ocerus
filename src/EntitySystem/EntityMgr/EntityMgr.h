#ifndef _ENTITYMGR_H_
#define _ENTITYMGR_H_

#include <map>
#include "EntityHandle.h"
#include "EntityMessage.h"
#include "EntityEnums.h"

/// Macro for easier use.
#define gEntityMgr EntitySystem::EntityMgr::GetSingleton()

namespace EntitySystem
{

	/// Forward declarations.
	//@{
	class ComponentMgr;
	class EntityDescription;
	struct EntityMessage;
	//@}

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
		inline EntityMessage::eResult PostMessage(EntityHandle h, const EntityMessage& msg) { return PostMessage(h.GetID(), msg); }
		/// Sends a message to all entities.
		void BroadcastMessage(const EntityMessage& msg);
		/// Returns the type of a specified entity.
		eEntityType GetEntityType(EntityHandle h) const;
		/// Destroys all entities in the manager.
		void DestroyAllEntities(void);	


	private:
		typedef std::map<EntityID, eEntityType> EntityMap;

		ComponentMgr* mComponentMgr;
		EntityMap mEntities;

		/// Posts a message to an entity. It is the only way entities can communicate with each other.
		EntityMessage::eResult PostMessage(EntityID id, const EntityMessage& msg);
	};
}

#endif