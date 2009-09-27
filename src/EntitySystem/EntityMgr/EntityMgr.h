#ifndef _ENTITYMGR_H_
#define _ENTITYMGR_H_

#include "EntityHandle.h"
#include "EntityMessage.h"
#include "EntityEnums.h"
#include "../../Utility/Properties/PropertyHolderMediator.h"
#include "../../ResourceSystem/Resource.h"
#include "Singleton.h"

/// @name Macro for easier use.
#define gEntityMgr EntitySystem::EntityMgr::GetSingleton()

/// @name Forward declarations.
class PropertyList;

namespace EntitySystem
{

	/// @name Forward declarations.
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

		/// @name Creates new entity accorindgly to its description and returns its handle.
		EntityHandle CreateEntity(const EntityDescription& desc, PropertyList& out);
		/// @name Destroys a specified entity if it exists.
		void DestroyEntity(const EntityHandle h);
		/// @name Loads all entities from an XML resource.
		bool LoadFromResource(ResourceSystem::ResourcePtr res);
		/// @name Posts a message to an entity.
		inline EntityMessage::eResult PostMessage(EntityHandle h, const EntityMessage& msg) { return PostMessage(h.GetID(), msg); }
		/// @name Sends a message to all entities.
		void BroadcastMessage(const EntityMessage& msg);
		/// @name Returns the type of a specified entity.
		eEntityType GetEntityType(const EntityHandle h) const;
		/// @name Returns team this entity belongs to.
		TeamID GetEntityTeam(const EntityHandle h) const;
		/// @name Returns true if the entity exists.
		bool EntityExists(const EntityHandle h) const;
		/// @name Changes team this entity belongs to.
		//@{
		void SetEntityTeam(const EntityHandle h, const EntityHandle teamOwner);
		void SetEntityTeam(const EntityHandle h, const TeamID team);
		//@}
		/// @name Returns true if the entity was fully initialized.
		bool IsEntityInited(const EntityHandle h) const;
		/// @name Retrieves properties of an entity. A filter related to properties' flags can be specified.
		bool GetEntityProperties(const EntityHandle h, PropertyList& out, const PropertyAccessFlags flagMask = FULL_PROPERTY_ACCESS_FLAGS);
		/// @name Retrieves a property of an entity. A filter related to properties' flags can be specified.
		PropertyHolderMediator GetEntityProperty(const EntityHandle h, const StringKey key, const PropertyAccessFlags flagMask = FULL_PROPERTY_ACCESS_FLAGS) const;
		/// @name Returns EntityHandle to an entity of a specified ID. If there are more of them, then returns the first one.
		EntityHandle FindFirstEntity(const string& ID);
		/// @name Actually destroyes all entities marked for destruction.
		void ProcessDestroyQueue(void);
		/// @name Destroys all entities in the manager.
		void DestroyAllEntities(void);	
		/// @name Fills the output vector with entities matching specified type/team.
		//TODO tohle je tu zbytecny; az budou hotovy query, odstranit!
		void EnumerateEntities(vector<EntityHandle>& out, const eEntityType desiredType = NUM_ENTITY_TYPES, const TeamID team = 0);

	private:
		/// @name This struct holds info about an instance of an entity in the system.
		struct EntityInfo
		{
			EntityInfo(void): mType(ET_UNKNOWN), mTeam(0), mFullyInited(false) {}
			EntityInfo(const eEntityType _type, const string& _ID = ""): mTeam(0), mFullyInited(false), mType(_type), mID(_ID) {}
			EntityInfo(const bool _fullyInited, const TeamID _team, const eEntityType _type, const string& _ID = ""): mFullyInited(_fullyInited), mTeam(_team), mType(_type), mID(_ID) {}

			eEntityType mType;
			TeamID mTeam; // currently corresponds to an entity ID of the owning ship. Zero if no such exists.
			bool mFullyInited;
			string mID;
		};
		//TODO predelat zpet na hashmapu, az bude hotova struktura pro vykreslovani entit i podle Z-orderu
		//typedef stdext::hash_map<EntityID, EntityInfo*> EntityMap;
		typedef map<EntityID, EntityInfo*> EntityMap;
		typedef vector<EntityID> EntityQueue;

		ComponentMgr* mComponentMgr;
		EntityMap mEntities;
		EntityQueue mEntityDestroyQueue;

		/// @name Posts a message to an entity. It is the only way entities can communicate with each other.
		EntityMessage::eResult PostMessage(EntityID id, const EntityMessage& msg);

		/// @name Internal method.
		void DestroyEntity(EntityMap::const_iterator it);

	};
}

#endif