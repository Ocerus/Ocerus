#ifndef CmpEngine_h__
#define CmpEngine_h__

#include "../ComponentMgr/Component.h"

namespace EntitySystem
{
	/// @name Platform item - engine. Handles rendering as well as logic and physics.
	class CmpEngine : public RTTIGlue<CmpEngine, Component>
	{
	public:
		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// @name Current power going out of the engine.
		//@{
		uint32 GetPower(void) const;
		void SetPower(const uint32 pow);
		//@}
		/// @name Curret power ratio in 0-1 range.
		//@{
		float32 GetPowerRatio(void) const { return mPowerRatio; }
		void SetPowerRatio(const float32 powrat);
		//@}
		/// @name Current angle of the engine relative to the default angle.
		//@{
		float32 GetRelativeAngle(void) const { return mRelativeAngle; }
		void SetRelativeAngle(const float32 angle) { mRelativeAngle = angle; }
		//@}
		/// @name Center angle of the engine relative to the current ship angle.
		//@{
		float32 GetDefaultAngle(void) const { return mDefaultAngle; }
		void SetDefaultAngle(const float32 angle) { mDefaultAngle = angle; }
		//@}
		/// @name Current angle of the engine in absolute coords.
		float32 GetAbsoluteAngle(void) const;
		/// @name Default angle of the engine in absolute coords.
		float32 GetAbsoluteDefaultAngle(void) const;

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
