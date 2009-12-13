#ifndef _GFXSCENEMANAGER_H_
#define _GFXSCENEMANAGER_H_

#include "Base.h"
#include "Singleton.h"
#include "../GfxSystem/GfxStructures.h"

/// Macro for easier use.
#define gGfxSceneMgr GfxSystem::GfxSceneMgr::GetSingleton()

namespace GfxSystem
{

	class GfxSceneMgr : public Singleton<GfxSceneMgr>
	{
	public:
		
		void AddSpriteEntity(EntitySystem::EntityHandle ent);

		void RemoveSpriteEntity(EntitySystem::EntityHandle ent);
		
		void AddCamera(EntitySystem::EntityHandle ent);

		void RemoveCamera(EntitySystem::EntityHandle ent);

		EntitySystem::EntityHandle GetCamera(int32 i);
		
		/// Sends DRAW message to stored sprite entities
		void Draw();
		
	private:	
		typedef vector<EntitySystem::EntityHandle> EntityHandleVector;
		EntityHandleVector mSprites;	//temprorary
		EntityHandleVector mCameras;
	};
}

#endif