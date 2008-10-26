#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "ComponentDescription.h"
#include "../EntityMgr/EntityMessage.h"

namespace EntitySystem
{
	class Component
	{
	public:
		virtual void Init(const ComponentDescription& desc) = 0;
		virtual void Deinit(void) = 0;

		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		//TODO add a mechanism to allow components state which resources they will need. This way we could preload them.

	protected:
		friend class ComponentMgr;

		Component(void);
		virtual ~Component(void);
	};
}

#endif