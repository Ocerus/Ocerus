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
		
		/// Frame size in pixels.
		GfxSystem::Point GetFrameSize(void) const { return mFrameSize; }

		/// Frame size in pixels.
		void SetFrameSize(GfxSystem::Point value) { mFrameSize = value; }
		
		/// Skipped pixels between frames in texture.
		GfxSystem::Point GetSkipSpace(void) const { return mSkipSpace; }

		/// Skipped pixels between frames in texture.
		void SetSkipSpace(GfxSystem::Point value) { mSkipSpace = value; }
		
		/// Number of frames.
		uint32 GetFrameCount(void) const { return mFrameCount; }

		/// Number of frames.
		void SetFrameCount(uint32 value) { mFrameCount = value; }
		
		/// Index of drawn frame.
		uint32 GetFrameIndex(void) const { return mFrameIndex; }

		/// Index of drawn frame.
		void SetFrameIndex(uint32 value) { mFrameIndex = value; }
		
		/// Duration of one animation cycle.
		float32 GetAnimDuration(void) const { return mAnimDuration; }

		/// Duration of one animation cycle.
		void SetAnimDuration(float32 value) { mAnimDuration = value; }
		
		/// Duration of one animation cycle.
		float32 GetAnimTime(void) const { return mAnimTime; }

		/// Duration of one animation cycle.
		void SetAnimTime(float32 value) { mAnimTime = value; }
		
		/// True if animation repeats.
		bool GetAnimRepeats(void) const { return mAnimRepeats; }

		/// True if animation repeats.
		void SetAnimRepeats(bool value) { mAnimRepeats = value; }

	private:

		ResourceSystem::ResourcePtr mTextureHandle;
		float32 mTransparency;
		GfxSystem::Point mFrameSize;
		GfxSystem::Point mSkipSpace;
		int32 mFrameCount;
		int32 mFrameIndex;
		float32 mAnimDuration;
		float32 mAnimTime;
		bool mAnimRepeats;
	};
}

#endif