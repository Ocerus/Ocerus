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

		GfxSystem::TexturePtr GetTexture(void) const { return mTextureHandle; }
		void SetTexture(GfxSystem::TexturePtr value) { mTextureHandle = value; }
		
		string GetResPath(void) const { return mResPath; }
		void SetResPath(string value) { mResPath = value; }
		
		Vector2 GetSize(void) const { return mSize; }
		void SetSize(Vector2 value) { mSize = value; }
		
		float32 GetTransparency(void) const { return mTransparency; }
		void SetTransparency(float32 value) { mTransparency = value; }

	private:
		GfxSystem::TexturePtr mTextureHandle;
		string mResPath;						//path to resource (texture)
		Vector2 mSize;
		float32 mTransparency;
		GfxSystem::Sprite* mSprite;
		bool mBoundToPhysics;
	};
}

#endif