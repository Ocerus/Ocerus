#ifndef _CMPPLATFORM_H_
#define _CMPPLATFORM_H_

#include "Component.h"

namespace EntitySystem
{
	class CmpPlatform : public Component
	{
	public:
		static Component* CreateMe() { return new CmpPlatform(); }

	private:
		CmpPlatform() {}
		virtual ~CmpPlatform() {}


	}
}