#ifndef CmpShipVisual_h__
#define CmpShipVisual_h__

#include "../ComponentMgr/Component.h"
#include "../../Utility/Properties.h"

namespace EntitySystem
{
	/// Visual representation of a ship.
	class CmpShipVisual : public RTTIGlue<CmpShipVisual, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpShipVisual(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// Just to make sure virtual functions work ok.
		virtual ~CmpShipVisual(void) {}
	private:

		//TODO tahle komponenta je asi na nic

	};
}

#endif // CmpShipVisual_h__