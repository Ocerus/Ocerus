/// @file
/// Common stuff for all entity components.

#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "../EntityMgr/EntityMessage.h"

/// Set of all user defined entity components usable by entities.
/// Each of the components is a derivation of the base abstract class EntitySystem::Component.
namespace EntityComponents { using namespace EntitySystem; }

namespace EntitySystem
{
	/// Base class for all entity components. This class contains the only means to communicate with entity components.
	/// The user will never know the specific component class because they are hidden by ComponentMgr.
	class Component : public RTTIBaseClass
	{
	public:

		/// Called after the component is created. To be overriden.
		/// Note that no property of the whole entity has been set yet while in this state.
		virtual void Create(void) {}

		/// Called before the component is destroyed. To be overriden.
		virtual void Destroy(void) {}

		/// Called when a new message arrives. To be overriden.
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		/// Returns a handle to the owner of this component (an entity).
		inline EntityHandle GetOwner(void) const { return mOwner; }

		/// Returns a handle to the owner of this component (an entity).
		inline EntityHandle* GetOwnerPtr(void) { return &mOwner; }

		/// Returns the type of this components.
		inline eComponentType GetType(void) const { return mType; }

		/// Posts a message to the entity owning this component.
		inline EntityMessage::eResult PostMessage(const EntityMessage::eType type, Reflection::PropertyFunctionParameters data = Reflection::PropertyFunctionParameters()) const { return GetOwner().PostMessage(type, data); }

		/// Returns a property to be get or set.
		PropertyHolder GetProperty(const StringKey name, const PropertyAccessFlags mask = PA_FULL_ACCESS) const;

		/// We don't want anyone except the ComponentMgr to create new components, but it has to be public because of the RTTI.
		Component(void);

		/// Destructor.
		virtual ~Component(void);

	private:
		EntityHandle mOwner;
		eComponentType mType;
		friend class ComponentMgr;
		inline void SetOwner(const EntityHandle owner) { mOwner = owner; }
		inline void SetType(const eComponentType type) { mType = type; }
	};
}

#endif