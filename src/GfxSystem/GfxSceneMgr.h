/// @file
/// Defines Gfx Scene Manager singleton (used for renderer).

#ifndef _GFXSCENEMANAGER_H_
#define _GFXSCENEMANAGER_H_

#include "Base.h"
#include "Singleton.h"
#include "../GfxSystem/GfxStructures.h"

namespace GfxSystem
{

	class GfxSceneMgr
	{
	public:

		/// Default constructor.
		GfxSceneMgr(void);

		/// Default destructor.
		virtual ~GfxSceneMgr(void);
		
		/// Adds a sprite to the manager.
		void AddSprite(const EntitySystem::Component* sprite, const EntitySystem::Component* transform);

		/// Removes a sprite from the manager.
		void RemoveSprite(const EntitySystem::Component* sprite);
		
		/// Renders all sprites.
		void DrawVisibleSprites();
		
	private:	

		typedef pair<const EntitySystem::Component*, const EntitySystem::Component*> SpritePair;
		typedef vector<SpritePair> SpriteVector;
		SpriteVector mSprites;
	};
}

#endif