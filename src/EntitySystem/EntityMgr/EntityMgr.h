/// @file
/// Entry point to the Entity system.

#ifndef _ENTITYMGR_H_
#define _ENTITYMGR_H_

#include "Base.h"
#include "Singleton.h"
#include "EntityHandle.h"
#include "EntityEnums.h"
#include "Properties/PropertyAccess.h"

/// Macro for easier use.
#define gEntityMgr EntitySystem::EntityMgr::GetSingleton()

namespace EntitySystem
{
	/// This class manages all game entities like weapons, enemy ships, projectiles, etc. Manipulation of entities is
	///	done via entity handles.
	class EntityMgr : public Singleton<EntityMgr>
	{
	public:

		EntityMgr(void);
		~EntityMgr(void);

		/// Creates new entity accorindgly to its description and returns its handle.
		EntityHandle CreateEntity(EntityDescription& desc, PropertyList& out);

		/// Destroys a specified entity if it exists.
		void DestroyEntity(const EntityHandle h);

		/// Loads all entities from an XML resource.
		bool LoadFromResource(ResourceSystem::ResourcePtr res);

		/// Posts a message to an entity.
		inline EntityMessage::eResult PostMessage(EntityHandle h, const EntityMessage& msg) { return PostMessage(h.GetID(), msg); }

		/// Sends a message to all entities.
		void BroadcastMessage(const EntityMessage& msg);

		/// Returns the type of a specified entity.
		eEntityType GetEntityType(const EntityHandle h) const;

		/// Returns true if the entity exists.
		bool EntityExists(const EntityHandle h) const;

		/// Changes the team this entity belongs to.
		void SetEntityTeam(const EntityHandle h, const EntityHandle teamOwner);

		/// Returns true if the entity was fully initialized.
		bool IsEntityInited(const EntityHandle h) const;

		/// Retrieves properties of an entity. A filter related to properties' flags can be specified.
		bool GetEntityProperties(const EntityHandle h, PropertyList& out, const PropertyAccessFlags flagMask = PA_FULL_ACCESS);

		/// Retrieves a property of an entity. A filter related to properties' flags can be specified.
		PropertyHolder GetEntityProperty(const EntityHandle h, const StringKey key, const PropertyAccessFlags flagMask = PA_FULL_ACCESS) const;

		/// Returns EntityHandle to an entity of a specified ID. If there are more of them, then returns the first one.
		EntityHandle FindFirstEntity(const string& ID);

		/// Actually destroyes all entities marked for destruction.
		void ProcessDestroyQueue(void);

		/// Destroys all entities in the manager.
		void DestroyAllEntities(void);

	private:

		/// This struct holds info about an instance of an entity in the system.
		struct EntityInfo
		{
			EntityInfo(void);
			EntityInfo(const eEntityType _type, const string& _ID = ""): mType(_type), mFullyInited(false), mID(_ID) {}
			EntityInfo(const bool _fullyInited, const eEntityType _type, const string& _ID = ""): mType(_type), mFullyInited(_fullyInited), mID(_ID) {}

			/// User type of the entity.
			eEntityType mType;
			/// True if this entity was fully inited in the system by the user.
			bool mFullyInited;
			/// Human readable user ID.
			string mID;
		};

		typedef hash_map<EntityID, EntityInfo*> EntityMap;
		typedef vector<EntityID> EntityQueue;

		ComponentMgr* mComponentMgr;
		EntityMap mEntities;
		EntityQueue mEntityDestroyQueue;

		/// Posts a message to an entity. It is the only way entities can communicate with each other apart from the properties.
		EntityMessage::eResult PostMessage(EntityID id, const EntityMessage& msg);

		/// Destructs an entity completely and removes it from the system.
		void DestroyEntity(EntityMap::const_iterator it);

	};
}

#endif