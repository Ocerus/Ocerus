#ifndef CmpPlatformLink_h__
#define CmpPlatformLink_h__

#include "../ComponentMgr/Component.h"
#include <vector>

namespace EntitySystem
{
	/// Links between two specific platforms.
	class CmpPlatformLinks : public RTTIGlue<CmpPlatformLinks, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpPlatformLinks(); }

		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// First of the two connected platforms.
		//@{
		EntityHandle GetFirstPlatform(void) const { return mFirstPlatform; }
		void SetFirstPlatform(const EntityHandle first) { mFirstPlatform = first; }
		//@}
		/// Second of the two connected platforms.
		//@{
		EntityHandle GetSecondPlatform(void) const { return mSecondPlatform; }
		void SetSecondPlatform(const EntityHandle second) { mSecondPlatform = second; }
		//@}
		/// Chance ratio of destrying these links and disconnecting platforms.
		//@{
		float32 GetDetachingChance(void) const { return mDetachingChance; }
		void SetDetachingChance(const float32 chance) { mDetachingChance = chance; }
		//@}
		/// Number of links between platforms.
		//@{
		uint32 GetNumLinks(void) const { return mAnchorsLength; }
		void SetNumLinks(const uint32 num) { mAnchorsLength = num; }
		//@}
		/// A list of anchors of all links on the first platform.
		//@{
		Vector2* GetFirstAnchors(void) const { return mFirstAnchors; }
		void SetFirstAnchors(Vector2* anchors);
		//@}
		/// a list of anchors of all links on the second platform.
		//@{
		Vector2* GetSecondAnchors(void) const { return mSecondAnchors; }
		void SetSecondAnchors(Vector2* anchors);
		//@}

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
