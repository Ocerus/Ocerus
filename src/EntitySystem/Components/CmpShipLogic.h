#ifndef CmpShipLogic_h__
#define CmpShipLogic_h__

#include "../ComponentMgr/Component.h"
#include <vector>

namespace EntitySystem
{
	/// Logic and layout of a specific ship.
	class CmpShipLogic : public RTTIGlue<CmpShipLogic, Component>
	{
	public:
		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

	private:
		typedef std::vector<EntityHandle> EntityList;
		EntityList mPlatforms;
		EntityList mLinks;
	};
}

#endif // CmpShipLogic_h__
