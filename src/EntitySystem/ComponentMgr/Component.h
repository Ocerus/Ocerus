#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "Properties.h"
#include "../EntityMgr/EntityHandle.h"
#include "StringKey.h"

namespace EntitySystem
{

	/// @name Forward declarations
	//@{
	class ComponentDescription;
	struct EntityMessage;
	//@}

	/** Abstract class representing one component. All concrete components must derive from this class.
	*/
	class Component : public RTTIBaseClass
	{
	public:
		/// @name Called after the component is created.
		virtual void Init(void) {}
		/// @name Called before the component is destroyed.
		virtual void Clean(void) {}

		/// @name Called when a new message arrives. To be overriden.
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		/// @name Returns a handle to the owner of this component (an entity). It returns a reference cos we need sometimes a pointer to the handle.
		//@{
		inline EntityHandle GetOwner(void) const { return mOwner; }
		inline EntityHandle* GetOwnerPtr(void) { return &mOwner; }
		//@}

		/// @name Posts a message to the entity owning this component.
		inline EntityMessage::eResult PostMessage(const EntityMessage::eType type, void* data = 0) const { return GetOwner().PostMessage(type, data); }

		/// @name Returns a property inside a holder.
		inline PropertyHolderMediator GetProperty(const StringKey name) const { return GetOwner().GetProperty(name); }

		//TODO add a mechanism to allow components state which resources they will need. This way we could preload them.
		// But maybe we can just let it to be done by the user by assigning resources to groups.

		/// @name We don't want anyone except the ComponentMgr to create new components, but it has to be public cos of RTTI.
		Component(void);
		/// @name Don't forget to override!
		virtual ~Component(void);
	protected:
		
		/// @name Helper method for easier Init.
		void InitProperties(const ComponentDescription& desc);
	private:
		EntityHandle mOwner;
		friend class ComponentMgr;
		inline void SetOwner(const EntityHandle owner) { mOwner = owner; }
	};
}

#endif