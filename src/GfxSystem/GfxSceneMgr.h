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
		
		/// Adds a 3d model to the manager.
		void AddModel(const EntitySystem::Component* model, const EntitySystem::Component* transform);

		/// Removes a model from the manager.
		void RemoveModel(const EntitySystem::Component* model);

		/// Renders all sprites.
		void DrawVisibleSprites();

		/// Renders all 3d models.
		void DrawVisibleModels();

	private:	
		typedef pair<const EntitySystem::Component*, const EntitySystem::Component*> DrawablePair;
		typedef vector<DrawablePair> DrawableVector;
		DrawableVector mDrawables;
	};
}

#endif