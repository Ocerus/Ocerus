#include "Common.h"
#include "Sprite.h"
#include "GfxSystem/Texture.h"
#include "GfxSystem/GfxSceneMgr.h"
#include <Box2D.h>

void EntityComponents::Sprite::Create( void )
{
	mResPath = "";
	mSize = Vector2_Zero;
	mTransparency = 0.0f;
}

void EntityComponents::Sprite::Destroy( void )
{
	gGfxRenderer.GetSceneManager()->RemoveSprite(this);
}

EntityMessage::eResult EntityComponents::Sprite::HandleMessage( const EntityMessage& msg )
{
	switch (msg.type)
	{
	case EntityMessage::INIT:
		{
			if ((mResPath == "")) return EntityMessage::RESULT_IGNORED;

			///@todo tohle by chtelo nejak zoptimalizovat, nejspis to pridavani resourcu udelat nejak globalnejc
			if (gResourceMgr.AddResourceFileToGroup(mResPath, "Textures", ResourceSystem::RESTYPE_TEXTURE, ResourceSystem::BPT_SYSTEM))
			{
				const char* lastSlashPos = 0;
				const char* str = mResPath.c_str();
				for(; *str; ++str)
				{
					if (*str == '/') lastSlashPos = str;
				}
				mTextureHandle = gResourceMgr.GetResource("Textures", StringKey(lastSlashPos+1, str-lastSlashPos-1));
								
				Component* transform = gEntityMgr.GetEntityComponentPtr(GetOwner(), CT_Transform);
				gGfxRenderer.GetSceneManager()->AddSprite(this, transform);
				
				return EntityMessage::RESULT_OK;
			}
			return EntityMessage::RESULT_ERROR;
		}
	default:
		return EntityMessage::RESULT_IGNORED;
	}
}

void EntityComponents::Sprite::RegisterReflection()
{
	RegisterProperty<Vector2>("Size", &Sprite::GetSize, &Sprite::SetSize, PA_FULL_ACCESS, "");
	RegisterProperty<ResourceSystem::ResourcePtr>("Texture", &Sprite::GetTexture, &Sprite::SetTexture, PA_FULL_ACCESS | PA_TRANSIENT, "");
	RegisterProperty<float32>("Transparency", &Sprite::GetTransparency, &Sprite::SetTransparency, PA_FULL_ACCESS, "");

	RegisterProperty<string>("Path", &Sprite::GetResPath, &Sprite::SetResPath, PA_INIT, "");

	// we need the transform to be able to have the position and angle ready while creating the sprite
	AddComponentDependency(CT_Transform);
}
