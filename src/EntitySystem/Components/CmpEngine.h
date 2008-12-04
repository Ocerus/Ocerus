#ifndef CmpEngine_h__
#define CmpEngine_h__

#include "../ComponentMgr/Component.h"
#include "../../Utility/Properties.h"

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
		uint32 GetPower(void) const { return mPower; }
		void SetPower(const uint32 pow) { mPower = pow; }
		//@}
		/// Current angle of the engine relative to the default angle.
		//@{
		float32 GetRelativeAngle(void) const { return mRelativeAngle; }
		void SetRelativeAngle(const float32 angle) { mTargetAngle = mRelativeAngle = angle; }
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
		/// Target angle of the engine (current angle will move to this angle) in absolute coords.
		float32 GetAbsoluteTargetAngle(void) const;

		/// Just to make sure virtual functions work ok.
		virtual ~CmpEngine(void) {}
	private:
		uint32 mPower;
		float32 mDefaultAngle;
		float32 mRelativeAngle;

		// inner vars
		uint32 mTargetPower;
		float32 mTargetAngle;

		void Draw(void) const;
		void DrawSelectionOverlay(const bool hover) const;
		void DrawSelectionUnderlay(const bool hover) const;

	};
}

#endif // CmpEngine_h__