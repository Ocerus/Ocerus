#ifndef CmpShipVisual_h__
#define CmpShipVisual_h__

#include "../ComponentMgr/Component.h"

namespace EntitySystem
{
	/// Visual representation of a ship.
	class CmpShipVisual : public RTTIGlue<CmpShipVisual, Component>
	{
	public:
		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

	private:

		//TODO tahle komponenta je asi na nic

	};
}

#endif // CmpShipVisual_h__
