#include "Common.h"
#include "GfxSceneMgr.h"
#include "EntitySystem/EntityMgr/LayerMgr.h"
#include "EntitySystem/Components/Transform.h"

using namespace GfxSystem;


GfxSceneMgr::GfxSceneMgr()
{
}

GfxSceneMgr::~GfxSceneMgr()
{

}

void GfxSceneMgr::AddSprite(const EntitySystem::Component* sprite, const EntitySystem::Component* transform)
{
	mDrawables.push_back(DrawablePair(sprite, transform));
}

void GfxSystem::GfxSceneMgr::AddModel( const EntitySystem::Component* model, const EntitySystem::Component* transform )
{
	mDrawables.push_back(DrawablePair(model, transform));
}

void GfxSceneMgr::RemoveSprite(const EntitySystem::Component* sprite)
{
	for (DrawableVector::iterator it=mDrawables.begin(); it!=mDrawables.end(); ++it)
	{
		if (it->first == sprite)
		{
			mDrawables.erase(it);
			break;
		}
	}
}

void GfxSystem::GfxSceneMgr::RemoveModel( const EntitySystem::Component* model )
{
	for (DrawableVector::iterator it=mDrawables.begin(); it!=mDrawables.end(); ++it)
	{
		if (it->first == model)
		{
			mDrawables.erase(it);
			break;
		}
	}
}

void GfxSceneMgr::DrawVisibleSprites()
{
	for(DrawableVector::iterator it = mDrawables.begin(); it != mDrawables.end(); ++it)
	{
		if (it->first->GetType() == CT_Sprite)
		{
			EntityComponents::Transform* transform = (EntityComponents::Transform*)it->second;
			if (gLayerMgr.IsLayerVisible(transform->GetLayer()))
			{
				gGfxRenderer.DrawSprite(it->first, it->second);
			}
		}
	}
}

void GfxSystem::GfxSceneMgr::DrawVisibleModels()
{
	for(DrawableVector::iterator it = mDrawables.begin(); it != mDrawables.end(); ++it)
	{
		if (it->first->GetType() == CT_Model)
		{
			EntityComponents::Transform* transform = (EntityComponents::Transform*)it->second;
			if (gLayerMgr.IsLayerVisible(transform->GetLayer()))
			{
				gGfxRenderer.DrawModel(it->first, it->second);
			}
		}
	}
}