/// @file
/// Sprite component.

#ifndef Sprite_h__
#define Sprite_h__

#include "../ComponentMgr/Component.h"

namespace EntityComponents
{
	/// Sprite component.
	class Sprite : public RTTIGlue<Sprite, Component>
	{
	public:
		virtual void Create(void);
		virtual void Destroy(void);
		virtual EntityMessage::eResult HandleMessage(const EntityMessage& msg);
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
		string mResPath;
		Vector2 mSize;
		float32 mTransparency;
	};
}

#endif