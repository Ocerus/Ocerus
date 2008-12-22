#ifndef CmpPlatformPhysics_h__
#define CmpPlatformPhysics_h__

#include "../ComponentMgr/Component.h"

/// Forward declarations
//@{
class b2Body;
class b2Shape;
//@}

namespace EntitySystem
{
	/// Handles physics representation of a specific platform.
	class CmpPlatformPhysics : public RTTIGlue<CmpPlatformPhysics, Component>
	{
	public:
		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// Position relative to the parent ship. Equals global in case this platform is free.
		//@{
		Vector2& GetRelativePosition(void) const { return const_cast<Vector2&>(mRelativePosition); }
		void SetRelativePosition(Vector2& pos) { mRelativePosition = pos; }
		//@}
		/// Position in absolute coords.
		//@{
		Vector2 GetAbsolutePosition(void) const;
		void SetAbsolutePosition(Vector2 pos);
		//@}
		/// Angle of the platform in absolute coords.
		//@{
		float32 GetAngle(void) const;
		void SetAngle(const float32 angle);
		//@}
		/// @name Initialization parameters.
		//@{
		void SetInitBodyPosition(Vector2 pos) { mInitBodyPosition = pos; }
		void SetInitBodyAngle(const float32 angle) { mInitBodyAngle = angle; }
		void SetInitShapeAngle(const float32 angle) { mInitShapeAngle = angle; }
		void SetInitShapeFlip(const bool flip) { mInitShapeFlip = flip; }
		//@}

	private:
		b2Body* mBody;
		b2Shape* mShape;
		Vector2 mRelativePosition;

		Vector2 mInitBodyPosition;
		float32 mInitBodyAngle;
		float32 mInitShapeAngle;
		bool mInitShapeFlip;

		void PostInit(void);

	};
}

#endif // CmpPlatformPhysics_h__
