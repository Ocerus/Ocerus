#ifndef CmpPlatformParams_h__
#define CmpPlatformParams_h__

#include "../ComponentMgr/Component.h"
#include "../Utility/Properties.h"
#include "../Utility/Settings.h"
#include <vector>

namespace EntitySystem
{
	class CmpPlatformParams : public RTTIGlue<CmpPlatformParams, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpPlatformParams(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		EntityHandle GetMaterial(void) const { return mMaterial; }
		void SetMaterial(const EntityHandle mat) { mMaterial = mat; }
		uint32 GetMaxHitpoints(void) const { return mMaxHitpoints; }
		void SetMaxHitpoints(const uint32 maxhp) { mMaxHitpoints = maxhp; }
		uint32 GetNumSlots(void) const { return mNumSlots; }
		void SetNumSlots(const uint32 numslots) { mNumSlots = numslots; }
		uint32 GetNumLinkSlots(void) const { return mNumLinkSlots; }
		void SetNumLinkSlots(const uint32 numlinkslots) { mNumLinkSlots = numlinkslots; }
		float32 GetArea(void) const { return mArea; }
		void SetArea(const float32 area) { mArea = area; }
		float32 GetMass(void) const { return mMass; }
		void SetMass(const float32 mass) { mMass = mass; }
		float32 GetBaseDetachingChance(void) const { return mBaseDetachingChance; }
		void SetBaseDetachingChance(const float32 chance) { mBaseDetachingChance = chance; }
		uint32 GetShapeLength(void) const { return mShapeLength; }
		void SetShapeLength(const uint32 len) { mShapeLength = len; }
		Vector2* GetShape(void) const { return mShape; }
		void SetShape(Vector2* shape);

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