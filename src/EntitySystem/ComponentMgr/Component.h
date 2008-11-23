#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "../Utility/Properties.h"
#include "../EntityMgr/EntityHandle.h"
#include "ComponentDescriptionItem.h"

namespace EntitySystem
{

	/// Forward declarations
	//@{
	class ComponentDescription;
	struct EntityMessage;
	//@}

	/** Abstract class representing one component. All concrete components must derive from this class.
	*/
	class Component : public RTTINullClass
	{
	public:
		/// Called after the component is created.
		virtual void Init(ComponentDescription& desc) = 0;
		/// Called before the component is destroyed.
		virtual void Deinit(void) = 0;

		/// Called when a new message arrives. To be overriden.
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		/// Returns a handle to the owner of this component (an entity). It returns a reference cos we need sometimes a pointer to the handle.
		//@{
		inline EntityHandle GetOwner(void) const { return mOwner; }
		inline EntityHandle* GetOwnerPtr(void) { return &mOwner; }
		//@}

		//TODO add a mechanism to allow components state which resources they will need. This way we could preload them.
		// But maybe we can just let it to be done by the user by assigning resources to groups.

		/// We don't want anyone except the ComponentMgr to create new components, but it has to be public cos of RTTI.
		Component(void);
		/// Don't forget to override!
		virtual ~Component(void);
	protected:
		
		/// Helper method for easier Init.
		void InitProperties(const ComponentDescription& desc);
	private:
		EntityHandle mOwner;
		friend class ComponentMgr;
		inline void SetOwner(const EntityHandle owner) { mOwner = owner; }
	};
}

#endif