#ifndef _ENTITYDESCRIPTION_H_
#define _ENTITYDESCRIPTION_H_

#include "../../Utility/Settings.h"
#include "../ComponentMgr/ComponentDescription.h"

namespace EntitySystem
{
	struct EntityDescription
	{
		typedef vector<ComponentDescription*> ComponentDescriptionsList;

		ComponentDescriptionsList mComponentDescriptions;
	};
}

#endif