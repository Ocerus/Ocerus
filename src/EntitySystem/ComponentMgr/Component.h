/// @file
/// Common stuff for all entity components.

#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "Properties.h"
#include "../EntityMgr/EntityHandle.h"
#include "StringKey.h"

/// Set of all user defined entity components usable by entities.
namespace EntityComponents { using namespace EntitySystem; }

namespace EntitySystem
{
	struct EntityMessage;

	/// Base class for all entity components. This class contains the only means to communicate with entity components.
	class Component : public RTTIBaseClass
	{
	public:

		/// Called after the component is created. To be overriden.
		virtual void Init(void) {}

		/// Called before the component is destroyed. To be overriden.
		virtual void Clean(void) {}

		/// Called when a new message arrives. To be overriden.
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		/// Returns a handle to the owner of this component (an entity).
		inline EntityHandle GetOwner(void) const { return mOwner; }

		/// Returns a handle to the owner of this component (an entity).
		inline EntityHandle* GetOwnerPtr(void) { return &mOwner; }

		/// Posts a message to the entity owning this component.
		inline EntityMessage::eResult PostMessage(const EntityMessage::eType type, void* data = 0) const { return GetOwner().PostMessage(type, data); }

		/// Returns a property to be get or set.
		inline PropertyHolderMediator GetProperty(const StringKey name) const { return GetOwner().GetProperty(name); }

		/// @remarks
		/// We don't want anyone except the ComponentMgr to create new components, but it has to be public because of the RTTI.
		Component(void);

		virtual ~Component(void);

	private:
		EntityHandle mOwner;
		friend class ComponentMgr;
		inline void SetOwner(const EntityHandle owner) { mOwner = owner; }
	};
}

#endif