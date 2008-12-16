#ifndef _ENTITYDESCRIPTION_H_
#define _ENTITYDESCRIPTION_H_

#include <vector>
#include "../../Utility/Settings.h"
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

		/// Call this before each subsequent filling of the description.
		void Init(const eEntityType type);

		/// Add new component specified by its type.
		void AddComponent(const eComponentType type);

		/// For internal use by the ComponentMgr.
		eComponentType GetNextComponent(void);
	private:
		friend class EntityMgr;

		typedef std::vector<eComponentType> ComponentDescriptionsList;

		/// List of descriptions of invidivual components this entity will consist of.
		ComponentDescriptionsList mComponents;

		/// Index of current component description.
		uint32 mIndex;

		/// Type of this entity.
		eEntityType mType;

		/// Clears everything
		void Clear(void);
	};
}

#endif