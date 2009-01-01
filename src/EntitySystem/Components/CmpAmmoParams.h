#ifndef CmpAmmoParams_h__
#define CmpAmmoParams_h__

#include "../ComponentMgr/Component.h"

namespace EntitySystem
{
	/// @name Represents all parameters someone can set up for a type of an ammo.
	class CmpAmmoParams : public RTTIGlue<CmpAmmoParams, Component>
	{
	public:
		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// @name Material used for creation of this ammo.
		//@{
		EntityHandle GetMaterial(void) const { return mMaterial; }
		void SetMaterial(const EntityHandle mat) { mMaterial = mat; }
		//@}
		/// @name Chance of disabling parts of the enemy ship.
		//@{
		float32 GetDisableRatio(void) const { return mDisableRatio; }
		void SetDisableRatio(const float32 disrat) { mDisableRatio = disrat; }
		//@}
		/// @name How hard it will hit.
		//@{
		float32 GetPowerRatio(void) const { return mPowerRatio; }
		void SetPowerRatio(const float32 rat) { mPowerRatio = rat; }
		//@}
		/// @name How strongly it will knockback the enemy ship after impact.
		//@{
		float32 GetKnockbackRatio(void) const { return mKnockbackRatio; }
		void SetKnockbackRatio(const float32 rat) { mKnockbackRatio = rat; }
		//@}
		/// @name How far it will fly related to the weapon shooting distance.
		//@{
		float32 GetDistanceRatio(void) const { return mDistanceRatio; }
		void SetDistanceRatio(const float32 rat) { mDistanceRatio = rat; }
		//@}
		/// @name How fast it will fly related to the weapon firing speed.
		//@{
		float32 GetSpeedRatio(void) const { return mSpeedRatio; }
		void SetSpeedRatio(const float32 rat) { mSpeedRatio = rat; }
		//@}
		/// @name Scaling factor of the trail effect.
		//@{
		float32 GetTrailEffectScale(void) const { return mTrailEffectScale; }
		void SetTrailEffectScale(const float32 scale) { mTrailEffectScale = scale; }
		//@}
		/// @name ID of the trail effect resource.
		//@{
		StringKey GetTrailEffect(void) const { return mTrailEffect; }
		void SetTrailEffect(const StringKey effect) { mTrailEffect = effect; }
		//@}
		/// @name Scaling factor of the explode effect.
		//@{
		float32 GetExplodeEffectScale(void) const { return mExplodeEffectScale; }
		void SetExplodeEffectScale(const float32 scale) { mExplodeEffectScale = scale; }
		//@}
		/// @name ID of the explode effect resource.
		//@{
		StringKey GetExplodeEffect(void) const { return mExplodeEffect; }
		void SetExplodeEffect(const StringKey effect) { mExplodeEffect = effect; }
		//@}
		/// @name ID of the resource group containing resources related to this entity.
		//@{
		StringKey GetResourceGroup(void) const { return mResourceGroup; }
		void SetResourceGroup(const StringKey gr) { mResourceGroup = gr; }
		//@}
		/// @name Size of the projectiles. Must fit into the weapon gun size.
		//@{
		uint32 GetSize(void) const { return mSize; }
		void SetSize(const uint32 s) { mSize = s; }
		//@}

	private:
		EntityHandle mMaterial;
		uint32 mSize;
		float32 mDisableRatio;
		float32 mPowerRatio;
		float32 mKnockbackRatio;
		float32 mDistanceRatio;
		float32 mSpeedRatio;
		float32 mTrailEffectScale;
		float32 mExplodeEffectScale;
		StringKey mTrailEffect;
		StringKey mExplodeEffect;
		StringKey mResourceGroup;

	};
}

#endif // CmpAmmoParams_h__