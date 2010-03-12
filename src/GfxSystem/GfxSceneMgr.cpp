#include "Common.h"
#include "GfxSceneMgr.h"

using namespace GfxSystem;


GfxSceneMgr::GfxSceneMgr()
{
}

GfxSceneMgr::~GfxSceneMgr()
{

}

void GfxSceneMgr::AddSprite(const EntitySystem::Component* sprite, const EntitySystem::Component* transform)
{
	mSprites.push_back(SpritePair(sprite, transform));
}

void GfxSceneMgr::RemoveSprite(const EntitySystem::Component* sprite)
{
	for (SpriteVector::iterator it=mSprites.begin(); it!=mSprites.end(); ++it)
	{
		if (it->first == sprite)
		{
			mSprites.erase(it);
			break;
		}
	}
}

void GfxSceneMgr::DrawVisibleSprites()
{
	SpriteVector::iterator it;
	for(it = mSprites.begin(); it != mSprites.end(); ++it)
	{
		gGfxRenderer.DrawSprite(it->first, it->second);
	}
}