#ifndef CmpShipPhysics_h__
#define CmpShipPhysics_h__

#include "../ComponentMgr/Component.h"

class b2Body;

namespace EntitySystem
{
	/// Physical representation of a specific ship.
	class CmpShipPhysics : public RTTIGlue<CmpShipPhysics, Component>
	{
	public:
		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// Position in absolute coords.
		//@{
		Vector2& GetPosition(void) const;
		void SetPosition(Vector2& pos);
		//@}
		/// Angle in absolute coords.
		//@{
		float32 GetAngle(void) const;
		void SetAngle(const float32 angle);
		//@}
		/// @name Initialization parameters.
		//@{
		void SetInitBodyPosition(Vector2 pos) { mInitBodyPosition = pos; }
		void SetInitBodyAngle(const float32 angle) { mInitBodyAngle = angle; }
		//@}

	private:
		b2Body* mBody;

		Vector2 mInitBodyPosition;
		float32 mInitBodyAngle;

		void PostInit(void);

	};
}

#endif // CmpShipPhysics_h__
