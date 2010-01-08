/// @file
/// Physical representation of static entities.

#ifndef StaticBody_h__
#define StaticBody_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// Physical representation of static entities. This component works as a layer between entities and the physics engine.
	class StaticBody: public RTTIGlue<StaticBody, Component>
	{
	public:

		/// Called after the component is created.
		virtual void Create(void);

		/// Called before the component is destroyed.
		virtual void Destroy(void);

		/// Called when a new message arrives.
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		/// Called from RTTI when the component is allowed to set up its properties.
		static void RegisterReflection(void);

	private:

		PhysicalBody* mBody;

	};
}

#endif // StaticBody_h__
