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

				GfxSystem::TexturePtr t = gResourceMgr.GetResource("textures", StringKey(lastSlashPos+1, str-lastSlashPos-1));
				//nejak nechapu proc to nejde priradit rovnou ...
				mTextureHandle = t;
				return EntityMessage::RESULT_OK;
			}
		}
	case EntityMessage::DRAW:
		{
			Vector2 pos = GetProperty("Position").GetValue<Vector2>();
			float32 z = (float32) GetProperty("Depth").GetValue<int32>();

			gGfxRenderer.AddSprite(GfxSystem::Sprite(pos, mSize, z, mTextureHandle->GetTexture()));
		}
	default:
		return EntityMessage::RESULT_IGNORED;
	}
}

void EntityComponents::Sprite::RegisterReflection()
{
	RegisterProperty<string>	("Path",	&GetResPath,	&SetResPath,	PA_FULL_ACCESS, "");
	RegisterProperty<Vector2>	("Size",	&GetSize,		&SetSize,		PA_FULL_ACCESS, "");
}
