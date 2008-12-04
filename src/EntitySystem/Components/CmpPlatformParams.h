#ifndef CmpPlatformParams_h__
#define CmpPlatformParams_h__

#include "../ComponentMgr/Component.h"
#include <vector>

namespace EntitySystem
{
	/// Represents parameters one can set for a specific platform type.
	class CmpPlatformParams : public RTTIGlue<CmpPlatformParams, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpPlatformParams(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// Material used for this platform.
		//@{
		EntityHandle GetMaterial(void) const { return mMaterial; }
		void SetMaterial(const EntityHandle mat) { mMaterial = mat; }
		//@}
		/// Maximum possible hitpoints.
		//@{
		uint32 GetMaxHitpoints(void) const { return mMaxHitpoints; }
		void SetMaxHitpoints(const uint32 maxhp) { mMaxHitpoints = maxhp; }
		//@}
		/// Number of slots which can be used for carrying platform items.
		//@{
		uint32 GetNumSlots(void) const { return mNumSlots; }
		void SetNumSlots(const uint32 numslots) { mNumSlots = numslots; }
		//@}
		/// Number of slots which can be used for connecting this platform to other platforms.
		//@{
		uint32 GetNumLinkSlots(void) const { return mNumLinkSlots; }
		void SetNumLinkSlots(const uint32 numlinkslots) { mNumLinkSlots = numlinkslots; }
		//@}
		/// Area of this platform.
		//@{
		float32 GetArea(void) const { return mArea; }
		void SetArea(const float32 area) { mArea = area; }
		//@}
		/// Total mass of this platform (not including items).
		//@{
		float32 GetMass(void) const { return mMass; }
		void SetMass(const float32 mass) { mMass = mass; }
		//@}
		/// Base chance of disconnecting from other platform. Is later used for computing the real chance when constructing links.
		//@{
		float32 GetBaseDetachingChance(void) const { return mBaseDetachingChance; }
		void SetBaseDetachingChance(const float32 chance) { mBaseDetachingChance = chance; }
		//@}
		/// Number of vertices of the shape.
		//@{
		uint32 GetShapeLength(void) const { return mShapeLength; }
		void SetShapeLength(const uint32 len) { mShapeLength = len; }
		//@}
		/// Shape of this platform.
		//@{
		Vector2* GetShape(void) const { return mShape; }
		void SetShape(Vector2* shape);
		//@}

		/// Just to make sure virtual functions work ok.
		virtual ~CmpPlatformParams(void) {}
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
			
		void ComputeParams(void);
	};
}

#endif // CmpPlatformParams_h__
