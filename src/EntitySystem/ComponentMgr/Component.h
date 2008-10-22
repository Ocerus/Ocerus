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

		virtual eEntityMessageResult HandleMessage(const EntityMessage& msg) = 0;

	protected:
		Component(void) {}
		virtual ~Component(void) {}
	};
}

#endif