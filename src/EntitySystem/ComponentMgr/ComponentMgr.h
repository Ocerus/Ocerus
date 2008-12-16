#ifndef _COMPONENTMGR_H_
#define _COMPONENTMGR_H_

#include <map>
#include "../EntityMgr/EntityHandle.h"
#include "../../Utility/Singleton.h"
#include "ComponentIterators.h"
#include "ComponentEnums.h"
#include "../../Utility/rtti/RTTI.h"

namespace EntitySystem
{

	/// Forward declarations
	//@{
	class Component;
	//#}

	/// Type used for registration function individual components.
	typedef Component* (*ComponentCreationMethod)(void);

	/** This class manages instances of all entity components in the system.
	*/
	class ComponentMgr : public Singleton<ComponentMgr>
	{
	public:
		/// All components must be registered in this constructor, so don't forget to add new types there!
		ComponentMgr(void);
		~ComponentMgr(void);
		
		//TODO doplnit const pred EntityHandle

		/// Creates a component and attaches it to an entity.
		bool CreateComponent(EntityHandle h, const eComponentType type);
		/// Destroys all components of a specified entity.
		//@{
		inline void DestroyEntityComponents(EntityHandle h) { DestroyEntityComponents(h.GetID()); }
		void DestroyEntityComponents(EntityID id);
		//@}
		/// Returns a collection of components (represented by an iterator) of a specified entity.
		//@{
		inline EntityComponentsIterator GetEntityComponents(EntityHandle h) { return GetEntityComponents(h.GetID()); }
		EntityComponentsIterator GetEntityComponents(EntityID id);
		//@}

		/// Retrieves properties of an entity. A filter related to access flags can be specified.
		//@{
		inline bool GetEntityProperties(const EntityHandle h, PropertyList& out, const uint8 flagMask = 0xff) { return GetEntityProperties(h.GetID(), out, flagMask); }
		bool GetEntityProperties(const EntityID id, PropertyList& out, const uint8 flagMask = 0xff);
		//@}

	private:
		typedef std::map<EntityID, ComponentsList> EntityComponentsMap;

		/// Static component data.
		ComponentCreationMethod mComponentCreationMethod[NUM_COMPONENT_TYPES];

		/// Dynamic component data.
		EntityComponentsMap mEntityComponentsMap;

	};
}

#endif