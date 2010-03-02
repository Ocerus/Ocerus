/// @file
/// Defines Gfx Scene Manager singleton (used for renderer).

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

		/// Default constructor.
		GfxSceneMgr(void);

		/// Default destructor.
		virtual ~GfxSceneMgr(void);
		
		/// Adds sprite to inner strucure
		void AddSprite(Sprite *spr);

		/// Removes sprite from inner strucure
		void RemoveSprite(Sprite *spr);
		
		void AddCamera(EntitySystem::EntityHandle ent);

		void RemoveCamera(EntitySystem::EntityHandle ent);

		EntitySystem::EntityHandle GetCamera(int32 i);
		
		/// Adds sprites to renderers drawing queue
		void Draw();
		
	private:	
		typedef vector<EntitySystem::EntityHandle> EntityHandleVector;
		typedef vector<Sprite*> SpriteVector;
		SpriteVector mSprites;
		EntityHandleVector mCameras;
	};
}

#endif