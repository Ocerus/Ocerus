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

void GfxSceneMgr::AddDrawable(const EntitySystem::Component* drawable, const EntitySystem::Component* transform)
{
	mDrawables.push_back(DrawablePair(drawable, transform));
}

void GfxSceneMgr::RemoveDrawable(const EntitySystem::Component* drawable)
{
	for (DrawableVector::iterator it=mDrawables.begin(); it!=mDrawables.end(); ++it)
	{
		if (it->first == drawable)
		{
			mDrawables.erase(it);
			break;
		}
	}
}

void GfxSceneMgr::DrawVisibleDrawables()
{
	for(DrawableVector::iterator it = mDrawables.begin(); it != mDrawables.end(); ++it)
	{
		EntityComponents::Transform* transform = (EntityComponents::Transform*)it->second;
		if (!gLayerMgr.IsLayerVisible(transform->GetLayer()))
			continue;
		const EntitySystem::Component* drawable = it->first; 
		EntitySystem::eComponentType type = drawable->GetType();

		switch (type)
		{
		case CT_Sprite:
			gGfxRenderer.DrawSprite(drawable, transform);
			break;
			
		case CT_Model:
			gGfxRenderer.DrawModel(drawable, transform);
			break;
		default:
			break;
		}

	}
}