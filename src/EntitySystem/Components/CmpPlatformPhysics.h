#ifndef CmpPlatformPhysics_h__
#define CmpPlatformPhysics_h__

#include "../ComponentMgr/Component.h"
#include "../Utility/Properties.h"

/// Forward declarations
//@{
class b2Body;
class b2Shape;
//@}

namespace EntitySystem
{
	class CmpPlatformPhysics : public RTTIGlue<CmpPlatformPhysics, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpPlatformPhysics(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		Vector2& GetRelativePosition(void) const { return const_cast<Vector2&>(mRelativePosition); }
		Vector2 GetAbsolutePosition(void) const;
		void SetAbsolutePosition(Vector2 pos);
		float32 GetAngle(void) const;
		void SetAngle(const float32 angle);

		/// Just to make sure virtual functions work ok.
		virtual ~CmpPlatformPhysics(void) {}
	private:
		b2Body* mBody;
		b2Shape* mShape;
		Vector2 mRelativePosition;

	};
}

#endif // CmpPlatformPhysics_h__