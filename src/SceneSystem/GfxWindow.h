#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Base.h"
#include "Singleton.h"

#include "../GfxSystem/GfxStructures.h"

#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")

struct SDL_Surface;
class IScreenListener;

#define gGfxWindow SceneSystem::GfxWindow::GetSingleton()

namespace SceneSystem
{
	enum EWindowEvent {
    WE_QUIT         ///< User tries to close the window (map to SDL_QUIT)
    /// maybe we will add another events in future
	};


	class GfxWindow : public Singleton<GfxWindow>
	{
	public:
		/// Constructs the window.
		void Init(const int32 resx, const int32 resy, const bool fullscreen, const string title);

		/// Destroys the window.
		~GfxWindow();

		//TODO: Zahazovat neznamy eventy?
		/// If there is known event in queue, returns it. Otherwise empties the queue and return false.
		bool PopEvent(EWindowEvent& result) const;

		inline int32 GetResolutionWidth() const {return mResx;}
		inline int32 GetResolutionHeight() const {return mResy;}

		/// Gets windows handle. Windows OS only.
		uint32 GfxWindow::_GetWindowHandle() const;

		inline virtual void AddScreenListener(IScreenListener * listener) { mScreenListeners.insert(listener); }
		inline virtual void RemoveResolutionChangeListener(IScreenListener * listener) { mScreenListeners.erase(listener); }

	private:
		// maybe not needed ...
		SDL_Surface* mScreen;

		int32 mResx, mResy;
		bool mFullscreen;
		
		set<IScreenListener*> mScreenListeners;

	};
}

#endif