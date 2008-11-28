#ifndef CmpPlatformLink_h__
#define CmpPlatformLink_h__

#include "../ComponentMgr/Component.h"
#include "../Utility/Properties.h"
#include <vector>

namespace EntitySystem
{
	class CmpPlatformLinks : public RTTIGlue<CmpPlatformLinks, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpPlatformLinks(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		EntityHandle GetFirstPlatform(void) const { return mFirstPlatform; }
		void SetFirstPlatform(const EntityHandle first) { mFirstPlatform = first; }
		EntityHandle GetSecondPlatform(void) const { return mSecondPlatform; }
		void SetSecondPlatform(const EntityHandle second) { mSecondPlatform = second; }
		float32 GetDetachingChance(void) const { return mDetachingChance; }
		void SetDetachingChance(const float32 chance) { mDetachingChance = chance; }
		uint32 GetNumLinks(void) const { return mAnchorsLength; }
		void SetNumLinks(const uint32 num) { mAnchorsLength = num; }
		Vector2* GetFirstAnchors(void) const { return mFirstAnchors; }
		void SetFirstAnchors(Vector2* anchors);
		Vector2* GetSecondAnchors(void) const { return mSecondAnchors; }
		void SetSecondAnchors(Vector2* anchors);

		/// We don't want anyone except the ComponentMgr to create new components, but it has to be public cos of RTTI.
		CmpPlatformLinks(void) {}
		/// Just to make sure virtual functions work ok.
		virtual ~CmpPlatformLinks(void) {}
	private:
		EntityHandle mFirstPlatform;
		EntityHandle mSecondPlatform;
		float32 mDetachingChance;
		uint32 mAnchorsLength;
		Vector2* mFirstAnchors;
		Vector2* mSecondAnchors;

		void Draw(void);
		void ComputeDetachingChance(void);
	};
}

#endif // CmpPlatformLink_h__