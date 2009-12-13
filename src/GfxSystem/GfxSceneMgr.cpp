#include "Common.h"
#include "GfxSceneMgr.h"

using namespace GfxSystem;


void GfxSceneMgr::AddSpriteEntity(EntitySystem::EntityHandle ent)
{
	mSprites.push_back(ent);
}

void GfxSceneMgr::RemoveSpriteEntity(EntitySystem::EntityHandle ent)
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
	return mCameras[i];
}

void GfxSceneMgr::Draw()
{
	EntityHandleVector::iterator it;
	for(it = mSprites.begin(); it != mSprites.end(); ++it)
	{
		gEntityMgr.PostMessage(*(it), EntitySystem::EntityMessage::DRAW);
	}
}