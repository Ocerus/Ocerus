#ifndef CmpPlatformVisual_h__
#define CmpPlatformVisual_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// @name Visual representation of a specific platform.
	class CmpPlatformVisual : public RTTIGlue<CmpPlatformVisual, Component>
	{
	public:
		virtual void Create(void);
		virtual void Destroy(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		void MyFunction(PropertyFunctionParameters params)
		{
			gLogMgr.LogMessage(*((string*)params.GetParameter(0)));
		}

		static void RegisterReflection(void);

	private:
		void Draw(void) const;
	};
}

#endif // CmpPlatformVisual_h__
