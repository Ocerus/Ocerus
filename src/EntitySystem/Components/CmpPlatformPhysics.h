#ifndef CmpPlatformPhysics_h__
#define CmpPlatformPhysics_h__

#include "../ComponentMgr/Component.h"
#include "../../Utility/Properties.h"

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
		static Component* CreateMe(void) { return new CmpPlatformPhysics(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// Position relative to the parent ship. Equals global in case this platform is free.
		//@{
		Vector2& GetRelativePosition(void) const { return const_cast<Vector2&>(mRelativePosition); }
		Vector2 GetAbsolutePosition(void) const;
		//@}
		/// Position in absolute coords.
		//@{
		void SetAbsolutePosition(Vector2 pos);
		//@}
		/// Angle of the platform in absolute coords.
		//@{
		float32 GetAngle(void) const;
		void SetAngle(const float32 angle);
		//@}

		/// Just to make sure virtual functions work ok.
		virtual ~CmpPlatformPhysics(void) {}
	private:
		b2Body* mBody;
		b2Shape* mShape;
		Vector2 mRelativePosition;

	};
}

#endif // CmpPlatformPhysics_h__