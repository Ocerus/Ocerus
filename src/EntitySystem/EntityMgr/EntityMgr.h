#ifndef _ENTITYMGR_H_
#define _ENTITYMGR_H_

#include <map>
#include "EntityHandle.h"
#include "EntityMessage.h"
#include "EntityEnums.h"
#include "../../Utility/Properties/PropertyList.h"

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
		EntityHandle CreateEntity(const EntityDescription& desc, PropertyList& out);
		/// Destroys a specified entity if it exists.
		void DestroyEntity(const EntityHandle h);
		/// Posts a message to an entity. It is the only way entities can communicate with each other.
		inline EntityMessage::eResult PostMessage(EntityHandle h, const EntityMessage& msg) { return PostMessage(h.GetID(), msg); }
		/// Sends a message to all entities.
		void BroadcastMessage(const EntityMessage& msg);
		/// Returns the type of a specified entity.
		eEntityType GetEntityType(const EntityHandle h) const;
		/// Returns true if the entity was fully initialized.
		bool IsEntityInited(const EntityHandle h) const;
		/// Retrieves properties of an entity. A filter related to properties' flags can be specified.
		bool GetEntityProperties(const EntityHandle h, PropertyList& out, const uint8 flagMask);
		/// Destroys all entities in the manager.
		void DestroyAllEntities(void);	


	private:
		/// This struct holds info about an instance of an entity in the system.
		struct EntityInfo
		{
			EntityInfo(void): type(ET_UNKNOWN), fullyInited(false) {}
			EntityInfo(const eEntityType _type): fullyInited(false), type(_type) {}
			EntityInfo(const bool _fullyInited, const eEntityType _type): fullyInited(_fullyInited), type(_type)	{}

			eEntityType type;
			bool fullyInited;
		};
		typedef std::map<EntityID, EntityInfo> EntityMap;

		ComponentMgr* mComponentMgr;
		EntityMap mEntities;

		/// Posts a message to an entity. It is the only way entities can communicate with each other.
		EntityMessage::eResult PostMessage(EntityID id, const EntityMessage& msg);
	};
}

#endif