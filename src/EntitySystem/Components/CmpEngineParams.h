#ifndef CmpEngineParams_h__
#define CmpEngineParams_h__

#include "../ComponentMgr/Component.h"

namespace EntitySystem
{
	/// Represents all parameters someone can set up for a type of an engine.
	class CmpEngineParams : public RTTIGlue<CmpEngineParams, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpEngineParams(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// Maximum hitpoints of the engine.
		//@{
		uint32 GetMaxHitpoints(void) const { return mMaxHitpoints; }
		void SetMaxHitpoints(const uint32 hp) { mMaxHitpoints = hp; }
		//@}
		/// Maximum power of the engine in absolute units.
		//@{
		uint32 GetMaxPower(void) const { return mMaxPower; }
		void SetMaxPower(const uint32 power) { mMaxPower = power; }
		//@}
		/// Material used by this engine.
		//@{
		EntityHandle GetMaterial(void) const { return mMaterial; }
		void SetMaterial(const EntityHandle mat) { mMaterial = mat; }
		//@}
		/// Angle variation (relative to the default angle) allowed to the engine.
		//@{
		float32 GetArcAngle(void) const { return mArcAngle; }
		void SetArcAngle(const float32 ang) { mArcAngle = ang; }
		//@}
		/// Stabilization ratio determines how fast the engine will prevent the ship from moving perpendicular to it's current direction.
		//@{
		uint32 GetStabilizationRatio(void) const { return mStabilizationRatio; }
		void SetStabilizationRatio(const uint32 ratio) { mStabilizationRatio = ratio; }
		//@}

		/// Just to make sure virtual functions work ok.
		virtual ~CmpEngineParams(void) {}
	private:
		EntityHandle mMaterial;
		uint32 mMaxHitpoints;
		uint32 mMaxPower;
		uint32 mStabilizationRatio;
		float32 mArcAngle;

		void ComputeParams(void);

	};
}

#endif // CmpEngineParams_h__
