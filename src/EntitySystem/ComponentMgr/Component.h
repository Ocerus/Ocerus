#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "../Utility/Properties.h"

namespace EntitySystem
{

	/// Forward declarations
	//@{
	class ComponentDescription;
	struct EntityMessage;

	/** Abstract class representing one component. All concrete components must derive from this class.
	*/
	class Component : public RTTINullClass
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

		/// We don't want anyone except the ComponentMgr to create new components, but it has to be public cos of RTTI.
		Component(void);
		/// Don't forget to override!
		virtual ~Component(void);
	protected:
		friend class ComponentMgr;

	};
}

#endif