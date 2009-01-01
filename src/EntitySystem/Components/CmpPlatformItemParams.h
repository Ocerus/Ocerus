#ifndef CmpPlatformItemParams_h__
#define CmpPlatformItemParams_h__

#include "../ComponentMgr/Component.h"

namespace EntitySystem
{
	/// @name Represents all parameters someone can set up for an item placed on the top of a platform.
	class CmpPlatformItemParams : public RTTIGlue<CmpPlatformItemParams, Component>
	{
	public:
		virtual void Init(void);
		virtual void Clean(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		static void RegisterReflection(void);

		/// @name Maximum hitpoints of the item.
		//@{
		uint32 GetMaxHitpoints(void) const { return mMaxHitpoints; }
		void SetMaxHitpoints(const uint32 hp) { mMaxHitpoints = hp; }
		//@}
		/// @name Material used by this item.
		//@{
		EntityHandle GetMaterial(void) const { return mMaterial; }
		void SetMaterial(const EntityHandle mat) { mMaterial = mat; }
		//@}
		/// @name Texture used for a visual representation of this item.
		//@{
		StringKey GetTexture(void) const { return mTexture; }
		void SetTexture(StringKey tex) { mTexture = tex; }
		//@}
		/// @name Angle used for transforming texture while drawing.
		//@{
		float32 GetTextureAngle(void) const { return mTextureAngle; }
		void SetTextureAngle(const float32 angle) { mTextureAngle = angle; }
		//@}
		/// @name Scale used for transforming texture while drawing.
		//@{
		float32 GetTextureScale(void) const { return mTextureScale; }
		void SetTextureScale(const float32 s) { mTextureScale = s; }
		//@}
		/// @name ResourceMgr group identifier where are located all data related to this item.
		//@{
		StringKey GetResourceGroup(void) const { return mResourceGroup; }
		void SetResourceGroup(const StringKey group) { mResourceGroup = group; }
		//@}

	private:
		EntityHandle mMaterial;
		uint32 mMaxHitpoints;
		float32 mTextureScale;
		float32 mTextureAngle;
		StringKey mTexture;
		StringKey mResourceGroup;

		void ComputeParams(void);

	};
}

#endif // CmpPlatformItemParams_h__