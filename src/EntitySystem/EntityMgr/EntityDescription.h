#ifndef _ENTITYDESCRIPTION_H_
#define _ENTITYDESCRIPTION_H_

#include <vector>
#include "../../Utility/Settings.h"

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

		/// Add new component descriptions using this method.
		void AddComponentDescription(ComponentDescription* desc);

		/// For internal use by the ComponentMgr.
		ComponentDescription* GetNextComponentDescription(void);
	private:
		friend class EntityMgr;

		typedef std::vector<ComponentDescription*> ComponentDescriptionsList;

		ComponentDescriptionsList mComponentDescriptions;
		/// Index of current component description.
		uint32 mIndex;
	};
}

#endif