#ifndef CmpWeapon_h__
#define CmpWeapon_h__

#include "../ComponentMgr/Component.h"

namespace EntitySystem
{
	/// @name Platform item - weapon. Handles rendering as well as logic and physics.
	class CmpWeapon : public RTTIGlue<CmpWeapon, Component>
	{
	public:
		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// @name Time which must pass until the weapon can be reloaded.
		//@{
		float32 GetTimeToReload(void) const { return mTimeToReload; }
		//@}
		/// @name Current ammo used by this weapon. This is a blueprints only.
		//@{
		EntityHandle GetAmmo(void) const { return mAmmoBlueprints; }
		void SetAmmo(const EntityHandle bp) { mAmmoBlueprints = bp; }
		//@}
		/// @name Current angle of the engine relative to the default angle.
		//@{
		float32 GetRelativeAngle(void) const { return mRelativeAngle; }
		void SetRelativeAngle(const float32 angle) { mRelativeAngle = angle; }
		//@}
		/// @name Center angle of the engine relative to the current ship angle.
		//@{
		float32 GetDefaultAngle(void) const { return mDefaultAngle; }
		void SetDefaultAngle(const float32 angle) { mDefaultAngle = angle; }
		//@}
		/// @name Current angle of the engine in absolute coords.
		float32 GetAbsoluteAngle(void) const;
		/// @name Default angle of the engine in absolute coords.
		float32 GetAbsoluteDefaultAngle(void) const;
		/// @name Current target of this weapon. Can be null.
		//@{
		EntityHandle GetTarget(void) const { return mTarget; }
		void SetTarget(const EntityHandle tar);
		//@}
		/// @name Is the weapon currently shooting?
		//@{
		bool IsFiring(void) const { return mIsFiring; }
		//@}

	private:
		float32 mTimeToReload;
		float32 mDefaultAngle;
		float32 mRelativeAngle;
		EntityHandle mAmmoBlueprints;
		EntityHandle mTarget;
		bool mIsFiring;

		/// @name Internals.
		//@{
		bool mWasSelected;
		//@}

		void Fire(void);
		void Draw(const bool selected) const;
		void DrawSelectionOverlay(const bool hover) const;
		void DrawSelectionUnderlay(const bool hover) const;

	};
}

#endif // CmpWeapon_h__