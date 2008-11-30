#ifndef _ENTITYDESCRIPTION_H_
#define _ENTITYDESCRIPTION_H_

#include <vector>
#include "../../Utility/Settings.h"
#include "EntityEnums.h"

namespace EntitySystem
{

	/// Forward declarations
	//@{
	class ComponentDescription;
	//@}

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

		/// Add new component descriptions using this method.
		void AddComponentDescription(ComponentDescription& desc);

		/// For internal use by the ComponentMgr.
		ComponentDescription* GetNextComponentDescription(void);
	private:
		friend class EntityMgr;

		typedef std::vector<ComponentDescription*> ComponentDescriptionsList;

		/// List of descriptions of invidivual components this entity will consist of.
		ComponentDescriptionsList mComponentDescriptions;

		/// Index of current component description.
		uint32 mIndex;

		/// Type of this entity.
		eEntityType mType;

		/// Clears everything
		void Clear(void);
	};
}

#endif