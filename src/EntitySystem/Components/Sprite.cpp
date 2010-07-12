#include "Common.h"
#include "Sprite.h"
#include "GfxSystem/Texture.h"
#include "GfxSystem/GfxSceneMgr.h"
#include <Box2D.h>

void EntityComponents::Sprite::Create( void )
{
	mTransparency = 0.0f;
	mFrameSize = GfxSystem::Point::Point_Zero;
	mSkipSpace = GfxSystem::Point::Point_Zero;
	mFrameCount = 1;
	mFrameIndex = 0;
	mAnimDuration = 0;
	mAnimTime = 0;
	mAnimRepeats = false;
}

void EntityComponents::Sprite::Destroy( void )
{
	gGfxRenderer.GetSceneManager()->RemoveDrawable(this);
}

EntityMessage::eResult EntityComponents::Sprite::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::INIT:
		{
			if (!mTextureHandle)
			{
				ocWarning << "Initing sprite with null texture";
				mTextureHandle = gResourceMgr.GetResource("General", "NullTexture");
				OC_ASSERT(mTextureHandle);
			}

			if (!gEntityMgr.IsEntityPrototype(GetOwner()))
			{
				Component* transform = gEntityMgr.GetEntityComponentPtr(GetOwner(), CT_Transform);
				gGfxRenderer.GetSceneManager()->AddDrawable(this, transform);
			}
				
			return EntityMessage::RESULT_OK;
		}
	// animatig sprite
	case EntityMessage::UPDATE_LOGIC:
		{
			// if not animating or when animation is over and not repeating, do nothing
			if ((mAnimDuration <= 0.01f) || (!mAnimRepeats && (mFrameIndex == (mFrameCount - 1))))
				return EntityMessage::RESULT_OK;

			mAnimTime += *msg.parameters.GetParameter(0).GetData<float32>();

			// repeating
			while ((mAnimTime >= mAnimDuration))
			{
				if (!mAnimRepeats)		//< no repeating = set last frame and end
				{
					mAnimTime = mAnimDuration;
					mFrameIndex = mFrameCount - 1;
					return EntityMessage::RESULT_OK;
				}
				mAnimTime -= mAnimDuration;
			}

			mFrameIndex = (int32)((mAnimTime / mAnimDuration) * mFrameCount);
			return EntityMessage::RESULT_OK;
		}
	default:
		return EntityMessage::RESULT_IGNORED;
	}
}

void EntityComponents::Sprite::RegisterReflection()
{
	RegisterProperty<ResourceSystem::ResourcePtr>("Texture", &Sprite::GetTexture, &Sprite::SetTexture, PA_FULL_ACCESS, "");
	RegisterProperty<float32>("Transparency", &Sprite::GetTransparency, &Sprite::SetTransparency, PA_FULL_ACCESS, "");
	
	RegisterProperty<GfxSystem::Point>("FrameSize", &Sprite::GetFrameSize, &Sprite::SetFrameSize, PA_FULL_ACCESS, "");
	RegisterProperty<GfxSystem::Point>("SkipSpace", &Sprite::GetSkipSpace, &Sprite::SetSkipSpace, PA_FULL_ACCESS, "");
	RegisterProperty<uint32>("FrameCount", &Sprite::GetFrameCount, &Sprite::SetFrameCount, PA_FULL_ACCESS, "");
	RegisterProperty<uint32>("FrameIndex", &Sprite::GetFrameIndex, &Sprite::SetFrameIndex, PA_FULL_ACCESS, "");
	RegisterProperty<float32>("AnimDuration", &Sprite::GetAnimDuration, &Sprite::SetAnimDuration, PA_FULL_ACCESS, "");
	RegisterProperty<float32>("AnimTime", &Sprite::GetAnimTime, &Sprite::SetAnimTime, PA_FULL_ACCESS, "");
	RegisterProperty<bool>("AnimRepeats", &Sprite::GetAnimRepeats, &Sprite::SetAnimRepeats, PA_FULL_ACCESS, "");

	// we need the transform to be able to have the position and angle ready while creating the sprite
	AddComponentDependency(CT_Transform);
}

void EntityComponents::Sprite::SetTexture(ResourceSystem::ResourcePtr value)
{ 
	if (value && value->GetType() == ResourceSystem::RESTYPE_TEXTURE)
	{
		mTextureHandle = value;
	}
}

void EntityComponents::Sprite::SetFrameSize(GfxSystem::Point value)
{
	mFrameSize.x = MathUtils::Max<int32>(0,value.x);
	mFrameSize.y = MathUtils::Max<int32>(0,value.y);

	RefreshFrameCount();
}

void EntityComponents::Sprite::SetSkipSpace(GfxSystem::Point value)
{
	mSkipSpace.x = MathUtils::Max<int32>(0,value.x);
	mSkipSpace.y = MathUtils::Max<int32>(0,value.y);

	RefreshFrameCount();
}

void EntityComponents::Sprite::RefreshFrameCount()
{ 
	int32 w = ((GfxSystem::TexturePtr)mTextureHandle)->GetWidth();
	int32 h = ((GfxSystem::TexturePtr)mTextureHandle)->GetHeight();

	mFrameCount = 1;
	if (((mFrameSize.x + mSkipSpace.x) != 0) && ((mFrameSize.y + mSkipSpace.y) != 0))
	{
		int32 inRow = w / (mFrameSize.x + mSkipSpace.x);
		mFrameCount = inRow * h / (mFrameSize.y + mSkipSpace.x);
	}
}