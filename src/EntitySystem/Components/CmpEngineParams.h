#ifndef CmpEngineParams_h__
#define CmpEngineParams_h__

#include "../ComponentMgr/Component.h"
#include "../Utility/Properties.h"

namespace EntitySystem
{
	class CmpEngineParams : public RTTIGlue<CmpEngineParams, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpEngineParams(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		uint32 GetMaxHitpoints(void) const { return mMaxHitpoints; }
		void SetMaxHitpoints(const uint32 hp) { mMaxHitpoints = hp; }
		uint32 GetMaxPower(void) const { return mMaxPower; }
		void SetMaxPower(const uint32 power) { mMaxPower = power; }
		EntityHandle GetMaterial(void) const { return mMaterial; }
		void SetMaterial(const EntityHandle mat) { mMaterial = mat; }
		float32 GetArcAngle(void) const { return mArcAngle; }
		void SetArcAngle(const float32 ang) { mArcAngle = ang; }
		float32 GetAngularSpeed(void) const { return mAngularSpeed; }
		void SetAngularSpeed(const float32 angspeed) { mAngularSpeed = angspeed; }

		/// Just to make sure virtual functions work ok.
		virtual ~CmpEngineParams(void) {}
	private:
		EntityHandle mMaterial;
		uint32 mMaxHitpoints;
		uint32 mMaxPower;
		float32 mArcAngle;
		float32 mAngularSpeed;

		void ComputeParams(void);

	};
}

#endif // CmpEngineParams_h__