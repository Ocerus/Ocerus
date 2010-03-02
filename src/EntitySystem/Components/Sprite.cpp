#include "Common.h"
#include "Sprite.h"
#include "GfxSystem/Texture.h"
#include <Box2D.h>

void EntityComponents::Sprite::Create( void )
{
	mResPath = "";
	mSize = Vector2_Zero;
	mTransparency = 0.0f;
	mSprite = NULL;
	mBoundToPhysics = false;
}

void EntityComponents::Sprite::Destroy( void )
{
	gGfxSceneMgr.RemoveSprite(mSprite);
}

EntityMessage::eResult EntityComponents::Sprite::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::INIT:
		{
			if (GetOwner().HasProperty("PhysicalBody")) mBoundToPhysics = true;
			if ((mResPath == "")) return EntityMessage::RESULT_IGNORED;

			///@todo tohle by chtelo nejak zoptimalizovat, nejspis to pridavani resourcu udelat nejak globalnejc
			if (gResourceMgr.AddResourceFileToGroup(mResPath, "Textures", ResourceSystem::RESTYPE_TEXTURE, true))
			{
				const char* lastSlashPos = 0;
				const char* str = mResPath.c_str();
				for(; *str; ++str)
				{
					if (*str == '/') lastSlashPos = str;
				}
				mTextureHandle = (GfxSystem::TexturePtr)gResourceMgr.GetResource("Textures", StringKey(lastSlashPos+1, str-lastSlashPos-1));
								
				mSprite = new GfxSystem::Sprite();
				mSprite->position = GetOwner().GetProperty("Position").GetValue<Vector2>();
				mSprite->angle = GetOwner().GetProperty("Angle").GetValue<float32>();
				mSprite->z = (float32) GetOwner().GetProperty("Depth").GetValue<int32>();
				mSprite->size = mSize;
				mSprite->texture = mTextureHandle->GetTexture();
				mSprite->transparency = mTransparency;

				gGfxSceneMgr.AddSprite(mSprite);
				
				return EntityMessage::RESULT_OK;
			}
			return EntityMessage::RESULT_ERROR;
		}

	case EntityMessage::UPDATE_POST_PHYSICS:	// update new pos etc. from physics
		if (mBoundToPhysics)
		{
			PhysicalBody* body = GetOwner().GetProperty("PhysicalBody").GetValue<PhysicalBody*>();
			OC_ASSERT(body);
			mSprite->position = body->GetPosition();
			mSprite->angle = body->GetAngle();
		}
		return EntityMessage::RESULT_OK;

	default:
		return EntityMessage::RESULT_IGNORED;
	}
}

void EntityComponents::Sprite::RegisterReflection()
{
	RegisterProperty<string> ("Path",			&Sprite::GetResPath,		&Sprite::SetResPath,		PA_FULL_ACCESS, "");
	RegisterProperty<Vector2>("Size",			&Sprite::GetSize,			&Sprite::SetSize,			PA_FULL_ACCESS, "");
	RegisterProperty<float32>("Transparency",	&Sprite::GetTransparency,	&Sprite::SetTransparency,	PA_FULL_ACCESS, "");

	// we need the transform to be able to have the position and angle ready while creating the sprite
	AddComponentDependency(CT_Transform);
}
