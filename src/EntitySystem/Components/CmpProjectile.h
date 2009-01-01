#ifndef CmpProjectile_h__
#define CmpProjectile_h__

#include "../ComponentMgr/Component.h"

/// @name Forward declarations.
//@{
class b2Body;
//@}

namespace EntitySystem
{
	/// @name Weapon projectile.
	class CmpProjectile : public RTTIGlue<CmpProjectile, Component>
	{
	public:
		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// @name Ammo blueprints.
		//@{
		EntityHandle GetBlueprints(void) const { return mBlueprints; }
		void SetBlueprints(const EntityHandle b) { mBlueprints = b; }
		//@}
		/// @name Position of the projectile when it was fired.
		//@{
		Vector2 GetInitBodyPosition(void) const { return mInitBodyPosition; }
		void SetInitBodyPosition(const Vector2 pos) { mInitBodyPosition = pos; }
		//@}
		/// @name Speed of the projectile when it was fired.
		//@{
		Vector2 GetInitBodySpeed(void) const { return mInitBodySpeed; }
		void SetInitBodySpeed(const Vector2 s) { mInitBodySpeed = s; }
		//@}
		/// @name Maximum distance it can fly.
		//@{
		float32 GetMaxDistance(void) const { return mMaxDistance; }
		void SetMaxDistance(const float32 d) { mMaxDistance = d; }
		//@}

	private:
		EntityHandle mBlueprints;
		Vector2 mInitBodyPosition;
		Vector2 mInitBodySpeed;
		float32 mMaxDistance;

		/// @name Internals.
		//@{
		GfxSystem::ParticleSystemPtr mTrailPS;
		b2Body* mBody;
		//@}

		void PostInit(void);
		void Draw(void) const;

	};
}

#endif // CmpProjectile_h__