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
		
		/// Adds a drawable component to the manager.
		void AddDrawable(const EntitySystem::Component* sprite, const EntitySystem::Component* transform);

		/// Removes a drawable component from the manager.
		void RemoveDrawable(const EntitySystem::Component* sprite);
		
		/// Renders all visible drawable components.
		void DrawVisibleDrawables();

	private:	
		typedef pair<const EntitySystem::Component*, const EntitySystem::Component*> DrawablePair;
		typedef vector<DrawablePair> DrawableVector;
		DrawableVector mDrawables;
	};
}

#endif