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

		/// Applies a force defined in the local coordinates. First parameter is the force and the second (optional)
		/// is the application point (local coordinates).
		void ApplyForceLocalCoords(PropertyFunctionParameters params);

		/// Applies a force defined in the world coordinates. First parameter is the force and the second (optional)
		/// is the application point (world coordinates).
		void ApplyForceWorldCoords(PropertyFunctionParameters params);

		/// Applies a torque to the body.
		void ApplyTorque(PropertyFunctionParameters params);

		/// Returns the angular damping.
		float32 GetAngularDamping() const;

		/// Sets the angular damping.
		void SetAngularDamping(float32 val);

		/// Returns the linear damping.
		float32 GetLinearDamping() const;

		/// Sets the linear damping.
		void SetLinearDamping(float32 val);

		/// Linear velocity of the body.
		Vector2 GetLinearVelocity() const;

		/// Angular velocity of the body.
		float32 GetAngularVelocity() const;


	private:

		PhysicalBody* mBody;
		mutable float32 mAngularDamping;
		mutable float32 mLinearDamping;

		void CreateBody(void);

	};
}

#endif // DynamicBody_h__