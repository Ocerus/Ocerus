#ifndef CmpPlatformPhysics_h__
#define CmpPlatformPhysics_h__

#include "../ComponentMgr/Component.h"

/// @name Forward declarations
//@{
class b2Body;
class b2Shape;
//@}

namespace EntityComponents
{
	/// @name Handles physics representation of a specific platform.
	class CmpPlatformPhysics : public RTTIGlue<CmpPlatformPhysics, Component>
	{
	public:
		virtual void Create(void);
		virtual void Destroy(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		Vector2 GetLinearVelocity(void) const;
		void SetLinearVelocity(const Vector2 linVel);
		float32 GetInitShapeAngle(void) const { return mInitShapeAngle; }
		void SetInitShapeAngle(const float32 angle) { mInitShapeAngle = angle; }
		bool GetInitShapeFlip(void) const { return mInitShapeFlip; }
		void SetInitShapeFlip(const bool flip) { mInitShapeFlip = flip; }
		Vector2* GetShape(void) const;
		uint32 GetShapeLength(void) const;

	private:
		b2Body* mBody;
		b2Shape* mShape;

		float32 mInitShapeAngle;
		bool mInitShapeFlip;

		void Init(void);
		void CreateBody(const bool hasParentShip);

	};
}

#endif // CmpPlatformPhysics_h__
