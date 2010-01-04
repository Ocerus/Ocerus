/// @file
/// Static information about an entity to be created.

#ifndef _ENTITYDESCRIPTION_H_
#define _ENTITYDESCRIPTION_H_

#include "Base.h"
#include "../ComponentMgr/ComponentEnums.h"

namespace EntitySystem
{
	/// This class contains all info needed to create one instance of an entity. It is basically a collection of
	///	component descriptions.
	class EntityDescription
	{
	public:

		/// Kind of the entity.
		enum eKind { EK_ENTITY, EK_PROTOTYPE, EK_PROTOTYPE_COPY };

		EntityDescription(void);
		~EntityDescription(void);

		/// Clears everything.
		/// Call this before each subsequent filling of the description.
		void Reset(void);

		/// Add a new component specified by its type.
		void AddComponent(const eComponentType type);

		/// Sets a custom name for this entity.
		inline void SetName(const string& toSet) { mName = toSet; }

		/// Sets this entity to be an ordinary entity or a prototype.
		inline void SetKind(const eKind kind) { mKind = kind; }

		/// Sets the prototype the entity is to be linked to.
		inline void SetPrototype(const EntityHandle toSet) { SetPrototype(toSet.GetID()); }

		/// Sets the prototype the entity is to be linked to.
		inline void SetPrototype(const EntityID toSet) { mPrototype = toSet; }

		/// Sets a desired ID for this entity. It doesn't have to be used by EntityMgr.
		inline void SetDesiredID(const EntityID toSet) { mID = toSet; }

	private:

		friend class EntityMgr;

		typedef vector<eComponentType> ComponentDescriptionsList;

		/// List of descriptions of invidivual components this entity will consist of.
		ComponentDescriptionsList mComponents;

		/// Custom identifier (not neccessarily unique) of this entity.
		string mName;

		/// Kind of the entity to be created (prototype, entity, ...).
		eKind mKind;

		/// ID of a prototype of this entity (can be invalid).
		EntityID mPrototype;

		/// ID of this entity (can be invalid to use autogenerated ID).
		EntityID mID;

	};
}

#endif