#include "Common.h"
#include "GfxSceneMgr.h"

using namespace GfxSystem;


/*void GfxSceneMgr::AddSpriteEntity(EntitySystem::EntityHandle ent)
{
	mSprites.push_back(ent);
}*/

void GfxSceneMgr::AddSprite(Sprite *spr)
{
	mSprites.push_back(spr);
}

/*void GfxSceneMgr::RemoveSpriteEntity(EntitySystem::EntityHandle ent)
{
	EntityHandleVector::iterator it;
	for(it = mSprites.begin(); it != mSprites.end(); ++it)
	{
		if ((*it) == ent)
		{
			mSprites.erase(it);
			return;
		}
	}
}*/

void GfxSceneMgr::RemoveSprite(Sprite *spr)
{
	SpriteVector::iterator it;
	for(it = mSprites.begin(); it != mSprites.end(); ++it)
	{
		if ((*it) == spr)
		{
			delete (*it);
			mSprites.erase(it);
			return;
		}
	}
}

void GfxSceneMgr::AddCamera(EntitySystem::EntityHandle ent)
{
	mCameras.push_back(ent);
}

void GfxSceneMgr::RemoveCamera(EntitySystem::EntityHandle ent)
{
	EntityHandleVector::iterator it;
	for(it = mCameras.begin(); it != mCameras.end(); ++it)
	{
		if ((*it) == ent)
		{
			mCameras.erase(it);
			return;
		}
	}
}

EntitySystem::EntityHandle GfxSceneMgr::GetCamera(int32 i)
{
	if (i<0 || i>=(int32)mCameras.size())
	{
		ocError << "Camera index out of bounds";
		return EntitySystem::EntityHandle::Null;
	}
	return mCameras[i];
}

void GfxSceneMgr::Draw()
{
	SpriteVector::iterator it;
	for(it = mSprites.begin(); it != mSprites.end(); ++it)
	{
		//gEntityMgr.PostMessage(*(it), EntitySystem::EntityMessage::DRAW);
		gGfxRenderer.AddSprite(*(*it));
	}
}