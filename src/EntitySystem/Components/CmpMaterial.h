#ifndef CmpMaterial_h__
#define CmpMaterial_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// @name Represents a material used by platforms and items.
	class CmpMaterial : public RTTIGlue<CmpMaterial, Component>
	{
	public:
		virtual void Create(void);
		virtual void Destroy(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// @name Durability ratio means how endurable is this material.
		//@{
		float32 GetDurabilityRatio(void) const { return mDurabilityRatio; }
		void SetDurabilityRatio(const float32 ratio) { mDurabilityRatio = ratio; }
		//@}
		/// @name Density means how heavy products made of this material will be.
		//@{
		float32 GetDensity(void) const { return mDensity; }
		void SetDensity(const float32 density) { mDensity = density; }
		//@}

	private:
		float32 mDurabilityRatio;
		float32 mDensity;

	};
}

#endif // CmpMaterial_h__
