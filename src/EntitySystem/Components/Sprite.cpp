#include "Common.h"
#include "Sprite.h"
#include "../GfxSystem/Texture.h"

void EntityComponents::Sprite::Create( void )
{
}

void EntityComponents::Sprite::Destroy( void )
{
}

EntityMessage::eResult EntityComponents::Sprite::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::INIT:
		{
			if ((mResPath == ""))
				return EntityMessage::RESULT_IGNORED;
			//tohle by chtelo nejak zoptimalizovat, nejspis to pridavani resourcu udelat nejak globalnejc
			if (gResourceMgr.AddResourceFileToGroup(mResPath, "textures", ResourceSystem::RESTYPE_TEXTURE, true))
			{
				const char* lastSlashPos = 0;
				const char* str = mResPath.c_str();
				for(; *str; ++str)
				{
					if (*str == '/') lastSlashPos = str;
				}
				mTextureHandle = (GfxSystem::TexturePtr)gResourceMgr.GetResource("textures",
												StringKey(lastSlashPos+1, str-lastSlashPos-1));
				return EntityMessage::RESULT_OK;
			}
			return EntityMessage::RESULT_ERROR;
		}
	case EntityMessage::DRAW:
		{
			Vector2 pos = GetProperty("Position").GetValue<Vector2>();
			float32 z = (float32) GetProperty("Depth").GetValue<int32>();

			if (mTextureHandle)
			{
				gGfxRenderer.AddSprite(GfxSystem::Sprite(pos, mSize, z, mTextureHandle->GetTexture(), mTransparency));
			}
			else
			{
				ocWarning << "Invalid texture";
			}

			return EntityMessage::RESULT_OK;
		}
	default:
		return EntityMessage::RESULT_IGNORED;
	}
}

void EntityComponents::Sprite::RegisterReflection()
{
	RegisterProperty<string> ("Path",			&Sprite::GetResPath,		&Sprite::SetResPath,		PA_FULL_ACCESS, "");
	RegisterProperty<Vector2>("Size",			&Sprite::GetSize,			&Sprite::SetSize,			PA_FULL_ACCESS, "");
	RegisterProperty<float32>("Transparency",	&Sprite::GetTransparency,	&Sprite::SetTransparency,	PA_FULL_ACCESS, "");
}
