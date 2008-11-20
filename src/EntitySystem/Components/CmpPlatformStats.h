#ifndef _CMPPLATFORM_H_
#define _CMPPLATFORM_H_

#include "../ComponentMgr/Component.h"
#include "../Utility/Properties.h"

namespace EntitySystem
{
	class CmpPlatformStats : public RTTIGlue<CmpPlatformStats, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpPlatformStats(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection();

		uint32 GetHitpoints(void) const { return mHitpoints; }
		void SetHitpoints(const uint32 hp) { mHitpoints = hp; }
	
		/// Just to make sure virtual functions work ok.
		virtual ~CmpPlatformStats(void) {}
	private:
		uint32 mHitpoints;

	};
}

#endif