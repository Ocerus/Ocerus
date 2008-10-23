#ifndef _CMPPLATFORM_H_
#define _CMPPLATFORM_H_

#include "../ComponentMgr/Component.h"

namespace EntitySystem
{
	class CmpPlatform : public Component
	{
	public:
		static Component* CreateMe(void) { return new CmpPlatform(); }

		virtual void Init(const ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

	protected:
		CmpPlatform(void) {}
		virtual ~CmpPlatform(void) {}

	};
}

#endif