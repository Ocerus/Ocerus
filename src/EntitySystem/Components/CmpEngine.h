#ifndef CmpEngine_h__
#define CmpEngine_h__

#include "../ComponentMgr/Component.h"
#include "../Utility/Properties.h"

namespace EntitySystem
{
	class CmpEngine : public RTTIGlue<CmpEngine, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpEngine(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		uint32 GetPower(void) const { return mPower; }
		void SetPower(const uint32 pow) { mPower = pow; }
		float32 GetRelativeAngle(void) const { return mRelativeAngle; }
		void SetRelativeAngle(const float32 angle) { mRelativeAngle = angle; }
		float32 GetDefaultAngle(void) const { return mDefaultAngle; }
		void SetDefaultAngle(const float32 angle) { mDefaultAngle = angle; }
		float32 GetAbsoluteAngle(void) const;

		/// Just to make sure virtual functions work ok.
		virtual ~CmpEngine(void) {}
	private:
		uint32 mPower;
		float32 mDefaultAngle;
		float32 mRelativeAngle;

		void Draw(void) const;
		void DrawHoverOverlay(void) const;

	};
}

#endif // CmpEngine_h__