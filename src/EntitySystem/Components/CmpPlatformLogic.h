#ifndef _CMPPLATFORM_H_
#define _CMPPLATFORM_H_

#include "../ComponentMgr/Component.h"

namespace EntitySystem
{
	/// @name Handles logic of one specific platform.
	class CmpPlatformLogic : public RTTIGlue<CmpPlatformLogic, Component>
	{
	public:
		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// @name Current hitpoints in absolute units.
		//@{
		uint32 GetHitpoints(void) const { return mHitpoints; }
		void SetHitpoints(const uint32 hp) { mHitpoints = hp; }
		//@}
		/// @name Platform type of this platform.
		//@{
		EntityHandle GetBlueprints(void) const { return mBlueprints; }
		void SetBlueprints(const EntityHandle blueprints) { mBlueprints = blueprints; }
		//@}
		/// @name Link to a parent ship owning this platform. Can be null (invalid).
		//@{
		EntityHandle GetParentShip(void) const { return mParentShip; }
		void SetParentShip(const EntityHandle parentShip) { mParentShip = parentShip; }
		//@}
	
	private:
		EntityHandle mBlueprints;
		EntityHandle mParentShip; // if null, it means this platform is free
		uint32 mHitpoints;
		typedef vector<EntityHandle> EntityList;
		EntityList mItems;

		/// @name support data
		Vector2 mPickCircleCenter;
		float32 mPickCircleRadius;

		void ComputePickStuff(void);
		void Die(void);
		void DrawSelectionOverlay(const bool hover) const;

	};
}

#endif
