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

		static void RegisterReflection(void);

		uint32 GetHitpoints(void) const { return mHitpoints; }
		void SetHitpoints(const uint32 hp) { mHitpoints = hp; }
		EntityHandle GetBlueprints(void) const { return mBlueprints; }
		void SetBlueprints(const EntityHandle blueprints) { mBlueprints = blueprints; }
		EntityHandle GetParentShip(void) const { return mParentShip; }
		void SetParentShip(const EntityHandle parentShip) { mParentShip = parentShip; }
	
		/// Just to make sure virtual functions work ok.
		virtual ~CmpPlatformStats(void) {}
	private:
		//TODO mozna prejmenovat na PlatformLogic
		EntityHandle mBlueprints;
		EntityHandle mParentShip; // if null, it means this platform is free
		uint32 mHitpoints;

	};
}

#endif