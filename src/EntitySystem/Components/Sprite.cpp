#include "Common.h"
#include "Sprite.h"
#include "GfxSystem/Texture.h"
#include "GfxSystem/GfxSceneMgr.h"
#include <Box2D.h>

void EntityComponents::Sprite::Create( void )
{
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
			if (!mTextureHandle)
			{
				ocWarning << "Initing sprite with null texture";
				mTextureHandle = gResourceMgr.GetResource("General", "NullTexture");
			}

			Component* transform = gEntityMgr.GetEntityComponentPtr(GetOwner(), CT_Transform);
			gGfxRenderer.GetSceneManager()->AddSprite(this, transform);
				
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

	// we need the transform to be able to have the position and angle ready while creating the sprite
	AddComponentDependency(CT_Transform);
}
