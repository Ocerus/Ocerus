#ifndef CmpPlatformVisual_h__
#define CmpPlatformVisual_h__

#include "../ComponentMgr/Component.h"

namespace EntitySystem
{
	/// @name Visual representation of a specific platform.
	class CmpPlatformVisual : public RTTIGlue<CmpPlatformVisual, Component>
	{
	public:
		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

	private:
		void Draw(void) const;
	};
}

#endif // CmpPlatformVisual_h__
