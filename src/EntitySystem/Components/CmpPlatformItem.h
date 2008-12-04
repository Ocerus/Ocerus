#ifndef CmpPlatformItem_h__
#define CmpPlatformItem_h__

#include "../ComponentMgr/Component.h"
#include "../../Utility/Properties.h"

namespace EntitySystem
{
	/// Represents an item placed on the top of a platform.
	class CmpPlatformItem : public RTTIGlue<CmpPlatformItem, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpPlatformItem(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// Current hitpoints.
		//@{
		uint32 GetHitpoints(void) const { return mHitpoints; }
		void SetHitpoints(const uint32 hp) { mHitpoints = hp; }
		//@}
		/// Platform which houses this item.
		//@{
		EntityHandle GetParentPlatform(void) const { return mParentPlatform; }
		void SetParentPlatform(const EntityHandle parent) { mParentPlatform = parent; }
		//@}
		/// Engine type of this specific engine. Holds some useful params.
		//@{
		EntityHandle GetBlueprints(void) const { return mBlueprints; }
		void SetBlueprints(const EntityHandle blueprints) { mBlueprints = blueprints; }
		//@}
		/// Position relative to the parent platform.
		//@{
		Vector2& GetRelativePosition(void) const { return const_cast<Vector2&>(mRelativePosition); }
		void SetRelativePosition(Vector2& relpos) { mRelativePosition = relpos; }
		//@}
		/// Position in absolute coords.
		Vector2 GetAbsolutePosition(void) const;

		/// Just to make sure virtual functions work ok.
		virtual ~CmpPlatformItem(void) {}
	private:
		Vector2 mRelativePosition;
		uint32 mHitpoints;
		EntityHandle mParentPlatform;
		EntityHandle mBlueprints;

	};
}

#endif // CmpPlatformItem_h__