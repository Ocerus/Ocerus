#ifndef CmpMaterial_h__
#define CmpMaterial_h__

#include "../ComponentMgr/Component.h"
#include "../Utility/Properties.h"

namespace EntitySystem
{
	/// Represents a material used by platforms and items.
	class CmpMaterial : public RTTIGlue<CmpMaterial, Component>
	{
	public:
		static Component* CreateMe(void) { return new CmpMaterial(); }

		virtual void Init(ComponentDescription& desc);
		virtual void Deinit(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// Durability ratio means how endurable is this material.
		//@{
		float32 GetDurabilityRatio(void) const { return mDurabilityRatio; }
		void SetDurabilityRatio(const float32 ratio) { mDurabilityRatio = ratio; }
		//@}
		/// Density means how heavy products made of this material will be.
		//@{
		float32 GetDensity(void) const { return mDensity; }
		void SetDensity(const float32 density) { mDensity = density; }
		//@}

		/// Just to make sure virtual functions work ok.
		virtual ~CmpMaterial(void) {}
	private:
		float32 mDurabilityRatio;
		float32 mDensity;

	};
}

#endif // CmpMaterial_h__