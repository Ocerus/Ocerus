/// @file
/// Defines Window singleton.

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Base.h"
#include "Singleton.h"

#include "../GfxSystem/GfxStructures.h"

#ifdef __WIN__
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#endif

struct SDL_Surface;
class IScreenListener;

#define gGfxWindow GfxSystem::GfxWindow::GetSingleton()

namespace GfxSystem
{
	enum eWindowEvent {
    WE_QUIT,		///< User tries to close the window (map to SDL_QUIT)
	WE_RESIZE		///< User tries to resize the window (map to SDL_VIDEORESIZE)
    /// maybe we will add another events in future
	};

#ifdef __WIN__
    typedef uint32 WindowHandle;
#else
    typedef uint64 WindowHandle;
#endif

	class GfxWindow : public Singleton<GfxWindow>
	{
	public:
		/// Constructs the window.
		void Init(const int32 resx, const int32 resy, const bool fullscreen, const string title);

		/// Destroys the window.
		~GfxWindow();

		/// If there is known event in queue, returns it. Otherwise empties the queue and return false.
		bool PopEvent(eWindowEvent& result);

		/// Changes window resolution, recreates drawing context, invokes ScreenListeners.
		void ChangeResolution(int32 x, int32 y);

		/// Returns the window width resolution.
		inline int32 GetResolutionWidth() const 
		{ return mResx; }

		/// Returns the window height resolution.
		inline int32 GetResolutionHeight() const 
			{ return mResy; }

		/// Returns the window resolution.
		inline Point GetResolution() const { return Point(mResx, mResy); }

		/// Gets windows handle. Windows OS only.
		WindowHandle _GetWindowHandle() const;

		/// Adds listener to window resolution change.
		inline virtual void AddScreenListener(IGfxWindowListener * listener) 
			{ mGfxWindowListeners.insert(listener); }

		/// Removes listener to window resolution change.
		inline virtual void RemoveScreenListener(IGfxWindowListener * listener) 
			{ mGfxWindowListeners.erase(listener); }

	private:
		// currently not used
		SDL_Surface* mScreen;

		int32 mResx, mResy;
		bool mFullscreen;

		set<IGfxWindowListener*> mGfxWindowListeners;

	};
}

#endif
