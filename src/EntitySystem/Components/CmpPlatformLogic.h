#ifndef _CMPPLATFORM_H_
#define _CMPPLATFORM_H_

#include "../ComponentMgr/Component.h"
#include "../Utility/Properties.h"

namespace EntitySystem
{
	class CmpPlatformLogic : public RTTIGlue<CmpPlatformLogic, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpPlatformLogic(); }

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
		virtual ~CmpPlatformLogic(void) {}
	private:
		EntityHandle mBlueprints;
		EntityHandle mParentShip; // if null, it means this platform is free
		uint32 mHitpoints;
		typedef std::vector<EntityHandle> EntityList;
		EntityList mItems;

		/// support data
		Vector2 mPickCircleCenter;
		float32 mPickCircleRadius;

		void DrawSelectionOverlay(const bool hover) const;

	};
}

#endif