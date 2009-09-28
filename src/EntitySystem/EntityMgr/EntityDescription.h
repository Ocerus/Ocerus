/// @file
/// Static information about an entity to be created.

#ifndef _ENTITYDESCRIPTION_H_
#define _ENTITYDESCRIPTION_H_

#include "Settings.h"
#include "EntityEnums.h"
#include "../ComponentMgr/ComponentEnums.h"

namespace EntitySystem
{

	/// @brief This class contains all info needed to create one instance of an entity. It is basically a collection of
	///	component descriptions.
	class EntityDescription
	{
	public:

		EntityDescription(void);
		~EntityDescription(void);

		/// Call this before each subsequent filling of the description.
		void Init(const eEntityType type, const string& ID = "");

		/// Addd new component specified by its type.
		void AddComponent(const eComponentType type);

		/// For internal use by the ComponentMgr. Advances to the next component in an internal list.
		eComponentType _GetNextComponent(void);

	private:

		friend class EntityMgr;

		typedef vector<eComponentType> ComponentDescriptionsList;

		/// List of descriptions of invidivual components this entity will consist of.
		ComponentDescriptionsList mComponents;

		/// Index of current component description.
		uint32 mIndex;

		/// Type of this entity.
		eEntityType mType;

		/// Custom identifier (not neccessarily unique) of this entity.
		string mID;

		/// Clears everything
		void Clear(void);
	};
}

#endif