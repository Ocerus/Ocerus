#ifndef CmpShipLogic_h__
#define CmpShipLogic_h__

#include "../ComponentMgr/Component.h"
#include "../Utility/Properties.h"
#include <vector>

namespace EntitySystem
{
	/// Logic and layout of a specific ship.
	class CmpShipLogic : public RTTIGlue<CmpShipLogic, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpShipLogic(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// Just to make sure virtual functions work ok.
		virtual ~CmpShipLogic(void) {}
	private:
		typedef std::vector<EntityHandle> EntityList;
		EntityList mPlatforms;
		EntityList mLinks;
	};
}

#endif // CmpShipLogic_h__