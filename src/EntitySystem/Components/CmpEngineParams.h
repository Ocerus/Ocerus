#ifndef CmpEngineParams_h__
#define CmpEngineParams_h__

#include "../ComponentMgr/Component.h"

namespace EntitySystem
{
	/// @name Represents all parameters someone can set up for a type of an engine.
	class CmpEngineParams : public RTTIGlue<CmpEngineParams, Component>
	{
	public:
		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// @name Maximum power of the engine in absolute units.
		//@{
		uint32 GetMaxPower(void) const { return mMaxPower; }
		void SetMaxPower(const uint32 power) { mMaxPower = power; }
		//@}
		/// @name Angle variation (relative to the default angle) allowed to the engine.
		//@{
		float32 GetArcAngle(void) const { return mArcAngle; }
		void SetArcAngle(const float32 ang) { mArcAngle = ang; }
		//@}
		/// @name Stabilization ratio determines how fast the engine will prevent the ship from moving perpendicular to it's current direction.
		//@{
		uint32 GetStabilizationRatio(void) const { return mStabilizationRatio; }
		void SetStabilizationRatio(const uint32 ratio) { mStabilizationRatio = ratio; }
		//@}
		/// @name Particle effect used for drawing thrust.
		//@{
		StringKey GetThrustEffect(void) const { return mThrustEffect; }
		void SetThrustEffect(const StringKey effect) { mThrustEffect = effect; }
		//@}
		/// @name Scale used for transforming thrust PS while drawing.
		//@{
		float32 GetThrustEffectScale(void) const { return mThrustEffectScale; }
		void SetThrustEffectScale(const float32 s) { mThrustEffectScale = s; }
		//@}
		/// @name Displacement used to move the PS away from the engine's center while drawing.
		//@{
		float32 GetThrustEffectDisplacement(void) const { return mThrustEffectDisplacement; }
		void SetThrustEffectDisplacement(const float32 s) { mThrustEffectDisplacement = s; }
		//@}
		/// @name Scale of the effect's emitting speed related to the current power ratio.
		//@{
		float32 GetThrustEffectPowerScale(void) const { return mThrustEffectPowerScale; }
		void SetThrustEffectPowerScale(const float32 s) { mThrustEffectPowerScale = s; }
		//@}

	private:
		uint32 mMaxPower;
		uint32 mStabilizationRatio;
		float32 mArcAngle;
		float32 mThrustEffectScale;
		float32 mThrustEffectDisplacement;
		float32 mThrustEffectPowerScale;
		StringKey mThrustEffect;

		void ComputeParams(void);

	};
}

#endif // CmpEngineParams_h__
