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

		static void RegisterReflection();

		Vector2& GetPosition(void) const;
		void SetPosition(Vector2& pos);
		float32 GetAngle(void) const;
		void SetAngle(const float32 angle);

		/// Just to make sure virtual functions work ok.
		virtual ~CmpPlatformPhysics(void) {}
	private:
		b2Body* mBody;
		b2Shape* mShape;

	};
}

#endif // CmpPlatformPhysics_h__