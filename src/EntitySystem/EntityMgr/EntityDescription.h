#ifndef _ENTITYDESCRIPTION_H_
#define _ENTITYDESCRIPTION_H_

#include <vector>
#include "../../Utility/Settings.h"
#include "../ComponentMgr/ComponentDescription.h"

namespace EntitySystem
{
	class EntityDescription
	{
	public:
		EntityDescription(void);
		~EntityDescription(void);

		void AddComponentDescription(ComponentDescription* desc);
		ComponentDescription* GetNextComponentDescription(void);
	private:
		friend class EntityMgr;

		typedef std::vector<ComponentDescription*> ComponentDescriptionsList;

		ComponentDescriptionsList mComponentDescriptions;
		uint32 mIndex;
	};
}

#endif