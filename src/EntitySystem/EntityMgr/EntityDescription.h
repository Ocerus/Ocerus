#ifndef _ENTITYDESCRIPTION_H_
#define _ENTITYDESCRIPTION_H_

#include "Settings.h"
#include "EntityEnums.h"
#include "../ComponentMgr/ComponentEnums.h"

namespace EntitySystem
{

	/** This class contains all info needed to create one instance of an entity. It is basically a collection of
		component descriptions.
	*/
	class EntityDescription
	{
	public:
		EntityDescription(void);
		~EntityDescription(void);

		/// @name Call this before each subsequent filling of the description.
		void Init(const eEntityType type, const string& ID = "");

		/// @name Add new component specified by its type.
		void AddComponent(const eComponentType type);

		/// @name For internal use by the ComponentMgr.
		eComponentType GetNextComponent(void);
	private:
		friend class EntityMgr;

		typedef vector<eComponentType> ComponentDescriptionsList;

		/// @name List of descriptions of invidivual components this entity will consist of.
		ComponentDescriptionsList mComponents;

		/// @name Index of current component description.
		uint32 mIndex;

		/// @name Type of this entity.
		eEntityType mType;

		/// @name Custom identifier (not neccessarily unique) of this entity.
		string mID;

		/// @name Clears everything
		void Clear(void);
	};
}

#endif