#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "ComponentDescription.h"
#include "../EntityMgr/EntityMessage.h"

namespace EntitySystem
{
	/** Abstract class representing one component. All concrete components must derive from this class.
	*/
	class Component
	{
	public:
		/// Called after the component is created.
		virtual void Init(const ComponentDescription& desc) = 0;
		/// Called before the component is destroyed.
		virtual void Deinit(void) = 0;

		/// Called when a new message arrives. To be overriden.
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		//TODO add a mechanism to allow components state which resources they will need. This way we could preload them.
		// But maybe we can just let it to be done by the user by assigning resources to groups.

	protected:
		friend class ComponentMgr;

		/// We don't want anyone except the CompoenentMgr to create new components.
		Component(void);
		/// Don't forget to override!
		virtual ~Component(void);
	};
}

#endif