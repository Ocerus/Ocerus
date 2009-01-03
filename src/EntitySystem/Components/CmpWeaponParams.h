#ifndef CmpWeaponParams_h__
#define CmpWeaponParams_h__

#include "../ComponentMgr/Component.h"

namespace EntitySystem
{
	/// @name Represents all parameters someone can set up for a type of an engine.
	class CmpWeaponParams : public RTTIGlue<CmpWeaponParams, Component>
	{
	public:
		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// @name Maximum distance projectiles can be fired to.
		//@{
		float32 GetFiringDistance(void) const { return mFiringDistance; }
		void SetFiringDistance(const float32 d) { mFiringDistance = d; }
		//@}
		/// @name How much it will diverse from it's desired angle when fired.
		//@{
		float32 GetSpread(void) const { return mSpread; }
		void SetSpread(const float32 spread) { mSpread = spread; }
		//@}
		/// @name Time delay before the weapon can fire again.
		//@{
		float32 GetReloadTime(void) const { return mReloadTime; }
		void SetReloadTime(const float32 t) { mReloadTime = t; }
		//@}
		/// @name Enum type of the projectile.
		//@{
		uint32 GetProjectileType(void) const { return mProjectileType; }
		void SetProjectileType(const uint32 t) { mProjectileType = t; }
		//@}
		/// @name Size of the projectile accepted by this weapon.
		//@{
		uint32 GetProjectileSize(void) const { return mProjectileSize; }
		void SetProjectileSize(const uint32 s) { mProjectileSize = s; }
		//@}
		/// @name Angle variation (relative to the default angle) allowed to the weapon.
		//@{
		float32 GetArcAngle(void) const { return mArcAngle; }
		void SetArcAngle(const float32 ang) { mArcAngle = ang; }
		//@}
		/// @name Particle effect used for drawing the firing effect.
		//@{
		StringKey GetFireEffect(void) const { return mFireEffect; }
		void SetFireEffect(const StringKey effect) { mFireEffect = effect; }
		//@}
		/// @name Scale used for transforming fire PS while drawing.
		//@{
		float32 GetFireEffectScale(void) const { return mFireEffectScale; }
		void SetFireEffectScale(const float32 s) { mFireEffectScale = s; }
		//@}
		/// @name Displacement used to move the PS away from the weapon's center while drawing.
		//@{
		float32 GetFireEffectDisplacement(void) const { return mFireEffectDisplacement; }
		void SetFireEffectDisplacement(const float32 s) { mFireEffectDisplacement = s; }
		//@}
		/// @name Speed which will gain projectiles when fired.
		//@{
		float32 GetFiringSpeed(void) const { return mFiringSpeed; }
		void SetFiringSpeed(const float32 speed) { mFiringSpeed = speed; }
		//@}

	private:
		float32 mFiringDistance;
		float32 mSpread;
		float32 mReloadTime;
		uint32 mProjectileType;
		uint32 mProjectileSize;
		float32 mFiringSpeed;
		float32 mArcAngle;
		float32 mFireEffectScale;
		float32 mFireEffectDisplacement;
		StringKey mFireEffect;

		void ComputeParams(void);

	};
}

#endif // CmpWeaponParams_h__