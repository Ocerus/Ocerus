#ifndef CmpPlatformVisual_h__
#define CmpPlatformVisual_h__

#include "../ComponentMgr/Component.h"
#include "../Utility/Properties.h"
#include <vector>

namespace EntitySystem
{
	/// Visual representation of a specific platform.
	class CmpPlatformVisual : public RTTIGlue<CmpPlatformVisual, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpPlatformVisual(); }

		virtual void Init( ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// Just to make sure virtual functions work ok.
		virtual ~CmpPlatformVisual(void) {}
	private:
		void Draw(void) const;
	};
}

#endif // CmpPlatformVisual_h__