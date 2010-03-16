/// @file
/// Physical representation of dynamic entities.

#ifndef DynamicBody_h__
#define DynamicBody_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// Physical representation of dynamic entities. This component works as a layer between entities and the physics engine.
	class DynamicBody: public RTTIGlue<DynamicBody, Component>
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

		/// Physical body in the physics engine.
		PhysicalBody* GetBody(void) const { return mBody; }

		/// Physical body in the physics engine.
		void SetBody(PhysicalBody* val) { mBody = val; }

		/// Applies force to the body.
		void ApplyForce(PropertyFunctionParameters params);

	private:

		PhysicalBody* mBody;

		void CreateBody(void);

	};
}

#endif // DynamicBody_h__