#ifndef _CMPPLATFORM_H_
#define _CMPPLATFORM_H_

#include "../ComponentMgr/Component.h"
#include "../Utility/Properties.h"

namespace EntitySystem
{
	class CmpPlatform : public RTTIGlue<CmpPlatform, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpPlatform(); }

		virtual void Init(const ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection();

		int32 GetNeco() const { return mNeco; }
		void SetNeco(const int32 neco) { mNeco = neco; }
	
		// Just to make sure virtual functions work ok.
		virtual ~CmpPlatform(void) {}
	private:
		int32 mNeco;
	};
}

#endif