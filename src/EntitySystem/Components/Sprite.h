/// @file
/// Sprite component.

#ifndef Sprite_h__
#define Sprite_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// Visual 2D representation of the entity.
	/// It allows the entity to define its visual appearance as a flat (2D) image. It is automatically used by GfxSystem
	/// while rendering the scene.
	class Sprite : public RTTIGlue<Sprite, Component>
	{
	public:

		/// Called after the component is created.
		virtual void Create(void);

		/// Called before the component is destroyed.
		virtual void Destroy(void);

		/// Called when a new message arrives.
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);

		/// Called from RTTI when the component is allowed to set up its properties.
		static void RegisterReflection(void);

		/// Texture used for rendering the sprite.
		ResourceSystem::ResourcePtr GetTexture(void) const { return mTextureHandle; }

		/// Texture used for rendering the sprite.
		void SetTexture(ResourceSystem::ResourcePtr value);

		/// Transparency from 0 to 1.
		float32 GetTransparency(void) const { return mTransparency; }

		/// Transparency from 0 to 1.
		void SetTransparency(float32 value) { mTransparency = value; }

	private:

		ResourceSystem::ResourcePtr mTextureHandle;
		float32 mTransparency;
	};
}

#endif