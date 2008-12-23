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

		/// @name Maximum hitpoints of the engine.
		//@{
		uint32 GetMaxHitpoints(void) const { return mMaxHitpoints; }
		void SetMaxHitpoints(const uint32 hp) { mMaxHitpoints = hp; }
		//@}
		/// @name Maximum power of the engine in absolute units.
		//@{
		uint32 GetMaxPower(void) const { return mMaxPower; }
		void SetMaxPower(const uint32 power) { mMaxPower = power; }
		//@}
		/// @name Material used by this engine.
		//@{
		EntityHandle GetMaterial(void) const { return mMaterial; }
		void SetMaterial(const EntityHandle mat) { mMaterial = mat; }
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
		/// @name Texture used for a visual representation of this engine.
		//@{
		char* GetTexture(void) const { return mTexture; }
		void SetTexture(char* tex) { mTexture = tex; }
		//@}
		/// @name Angle used for transforming texture while drawing.
		//@{
		float32 GetTextureAngle(void) const { return mTextureAngle; }
		void SetTextureAngle(const float32 angle) { mTextureAngle = angle; }
		//@}
		/// @name Scale used for transforming texture while drawing.
		//@{
		float32 GetTextureScale(void) const { return mTextureScale; }
		void SetTextureScale(const float32 s) { mTextureScale = s; }
		//@}

	private:
		EntityHandle mMaterial;
		uint32 mMaxHitpoints;
		uint32 mMaxPower;
		uint32 mStabilizationRatio;
		float32 mArcAngle;
		float32 mTextureScale;
		float32 mTextureAngle;
		//TODO tady predelat char* na univerzalni hash stringu
		char* mTexture;

		void ComputeParams(void);

	};
}

#endif // CmpEngineParams_h__
