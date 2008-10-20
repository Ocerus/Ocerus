#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "ComponentDescription.h"

namespace EntitySystem
{
	class Component
	{
	public:
		virtual void Init(const ComponentDescription& desc) = 0;
		virtual void Deinit(void) = 0;

		virtual eEntityMessageResult HandleMessage(const EntityMessage& msg);

	private:
		Component();
		virtual ~Component();
	};
}

#endif