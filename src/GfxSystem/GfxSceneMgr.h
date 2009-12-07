#ifndef _GFXSCENEMANAGER_H_
#define _GFXSCENEMANAGER_H_

#include "Base.h"
#include "Singleton.h"
#include "../GfxSystem/GfxStructures.h"

/// Macro for easier use.
#define gGfxSceneMgr GfxSystem::GfxSceneMgr::GetSingleton()

namespace GfxSystem
{
	typedef vector<EntitySystem::EntityHandle> EntityHandleVector;

	class GfxSceneMgr : public Singleton<GfxSceneMgr>
	{
	public:
		void AddEntity(EntitySystem::EntityHandle ent)
		{
			mEntities.push_back(ent);
		}
		void Draw()
		{
			EntityHandleVector::iterator it;
			for(it = mEntities.begin(); it != mEntities.end(); ++it)
			{
				gEntityMgr.PostMessage(*(it), EntitySystem::EntityMessage::DRAW);
			}
		}
		EntityHandleVector mEntities;	//temprorary
	};
}

#endif