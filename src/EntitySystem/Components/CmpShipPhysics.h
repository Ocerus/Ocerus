#ifndef CmpShipPhysics_h__
#define CmpShipPhysics_h__

#include "../ComponentMgr/Component.h"
#include "../Utility/Properties.h"

class b2Body;

namespace EntitySystem
{
	class CmpShipPhysics : public RTTIGlue<CmpShipPhysics, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpShipPhysics(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		Vector2& GetPosition(void) const;
		void SetPosition(Vector2& pos);
		float32 GetAngle(void) const;
		void SetAngle(const float32 angle);

		/// Just to make sure virtual functions work ok.
		virtual ~CmpShipPhysics(void) {}
	private:
		b2Body* mBody;

	};
}

#endif // CmpShipPhysics_h__