#ifndef CmpPlatformParams_h__
#define CmpPlatformParams_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// @name Represents parameters one can set for a specific platform type.
	class CmpPlatformParams : public RTTIGlue<CmpPlatformParams, Component>
	{
	public:
		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// @name Material used for this platform.
		//@{
		EntityHandle GetMaterial(void) const { return mMaterial; }
		void SetMaterial(const EntityHandle mat) { mMaterial = mat; }
		//@}
		/// @name Maximum possible hitpoints.
		//@{
		uint32 GetMaxHitpoints(void) const { return mMaxHitpoints; }
		void SetMaxHitpoints(const uint32 maxhp) { mMaxHitpoints = maxhp; }
		//@}
		/// @name Number of slots which can be used for carrying platform items.
		//@{
		uint32 GetNumSlots(void) const { return mNumSlots; }
		void SetNumSlots(const uint32 numslots) { mNumSlots = numslots; }
		//@}
		/// @name Number of slots which can be used for connecting this platform to other platforms.
		//@{
		uint32 GetNumLinkSlots(void) const { return mNumLinkSlots; }
		void SetNumLinkSlots(const uint32 numlinkslots) { mNumLinkSlots = numlinkslots; }
		//@}
		/// @name Area of this platform.
		//@{
		float32 GetArea(void) const { return mArea; }
		void SetArea(const float32 area) { mArea = area; }
		//@}
		/// @name Total mass of this platform (not including items).
		//@{
		float32 GetMass(void) const { return mMass; }
		void SetMass(const float32 mass) { mMass = mass; }
		//@}
		/// @name Base chance of disconnecting from other platform. Is later used for computing the real chance when constructing links.
		//@{
		float32 GetBaseDetachingChance(void) const { return mBaseDetachingChance; }
		void SetBaseDetachingChance(const float32 chance) { mBaseDetachingChance = chance; }
		//@}
		/// @name Number of vertices of the shape.
		//@{
		uint32 GetShapeLength(void) const { return mShapeLength; }
		void SetShapeLength(const uint32 len) { mShapeLength = len; }
		//@}
		/// @name Shape of this platform.
		//@{
		Vector2* GetShape(void) const { return mShape; }
		void SetShape(Vector2* shape);
		//@}
		/// @name Effect used when the platform is destroyed.
		//@{
		StringKey GetExplodeEffect(void) const { return mExplodeEffect; }
		void SetExplodeEffect(const StringKey effect) { mExplodeEffect = effect; }
		float32 GetExplodeEffectScale(void) const { return mExplodeEffectScale; }
		void SetExplodeEffectScale(const float32 scale) { mExplodeEffectScale = scale; }
		//@}
		/// @name Group containing all resources needed for this entity.
		//@{
		StringKey GetResourceGroup(void) const { return mResourceGroup; }
		void SetResourceGroup(const StringKey group) { mResourceGroup = group; }
		//@}
		/// @name Color of the platform.
		//@{
		GfxSystem::Color GetFillColor(void) const { return mFillColor; }
		void SetFillColor(const GfxSystem::Color col) { mFillColor = col; }
		//@}

	private:
		EntityHandle mMaterial;
		uint32 mMaxHitpoints;
		uint32 mNumSlots;
		uint32 mNumLinkSlots;
		float32 mArea;
		float32 mMass;
		float32 mBaseDetachingChance;
		uint32 mShapeLength;
		Vector2* mShape;
		float32 mExplodeEffectScale;
		StringKey mExplodeEffect;
		StringKey mResourceGroup;
		GfxSystem::Color mFillColor;
			
		void ComputeParams(void);
	};
}

#endif // CmpPlatformParams_h__
