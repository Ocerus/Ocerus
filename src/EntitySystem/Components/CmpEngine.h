#ifndef CmpEngine_h__
#define CmpEngine_h__

#include "../ComponentMgr/Component.h"

namespace EntitySystem
{
	/// Platform item - engine. Handles rendering as well as logic and physics.
	class CmpEngine : public RTTIGlue<CmpEngine, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpEngine(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// Current power going out of the engine.
		//@{
		uint32 GetPower(void) const;
		void SetPower(const uint32 pow);
		//@}
		/// Curret power ratio in 0-1 range.
		//@{
		float32 GetPowerRatio(void) const { return mPowerRatio; }
		void SetPowerRatio(const float32 powrat);
		//@}
		/// Current angle of the engine relative to the default angle.
		//@{
		float32 GetRelativeAngle(void) const { return mRelativeAngle; }
		void SetRelativeAngle(const float32 angle) { mRelativeAngle = angle; }
		//@}
		/// Center angle of the engine relative to the current ship angle.
		//@{
		float32 GetDefaultAngle(void) const { return mDefaultAngle; }
		void SetDefaultAngle(const float32 angle) { mDefaultAngle = angle; }
		//@}
		/// Current angle of the engine in absolute coords.
		float32 GetAbsoluteAngle(void) const;
		/// Default angle of the engine in absolute coords.
		float32 GetAbsoluteDefaultAngle(void) const;

		/// Just to make sure virtual functions work ok.
		virtual ~CmpEngine(void) {}
	private:
		float32 mPowerRatio;
		float32 mDefaultAngle;
		float32 mRelativeAngle;

		void Draw(void) const;
		void DrawSelectionOverlay(const bool hover) const;
		void DrawSelectionUnderlay(const bool hover) const;

	};
}

#endif // CmpEngine_h__
