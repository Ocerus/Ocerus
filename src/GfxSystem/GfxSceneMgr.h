/// @file
/// Manages objects in the game which have a visual representation. The purpose is to speed the rendering up.

#ifndef _GFXSCENEMANAGER_H_
#define _GFXSCENEMANAGER_H_

#include "Base.h"
#include "Singleton.h"
#include "GfxStructures.h"

namespace GfxSystem
{
	/// Manages objects in the game which have a visual representation. The purpose is to speed the rendering up.
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