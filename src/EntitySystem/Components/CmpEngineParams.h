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
		float32 GetMinAngle(void) const { return mMinAngle; }
		void SetMinAngle(const float32 ang) { mMinAngle = ang; }
		float32 GetMaxAngle(void) const { return mMaxAngle; }
		void SetMaxAngle(const float32 ang) { mMaxAngle = ang; }
		float32 GetAngularSpeed(void) const { return mAngularSpeed; }
		void SetAngularSpeed(const float32 angspeed) { mAngularSpeed = angspeed; }

		/// Just to make sure virtual functions work ok.
		virtual ~CmpEngineParams(void) {}
	private:
		EntityHandle mMaterial;
		uint32 mMaxHitpoints;
		uint32 mMaxPower;
		float32 mMinAngle;
		float32 mMaxAngle;
		float32 mAngularSpeed;

		void ComputeParams(void);

	};
}

#endif // CmpEngineParams_h__