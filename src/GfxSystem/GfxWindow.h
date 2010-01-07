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
	enum EWindowEvent {
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

		//TODO: Zahazovat neznamy eventy?
		/// If there is known event in queue, returns it. Otherwise empties the queue and return false.
		bool PopEvent(EWindowEvent& result);

		void ChangeResolution(int32 x, int32 y);

		inline int32 GetResolutionWidth() const {
			return mResx;}
		inline int32 GetResolutionHeight() const {
			return mResy;}

		/// Gets windows handle. Windows OS only.
		WindowHandle _GetWindowHandle() const;

		inline virtual void AddScreenListener(IGfxWindowListener * listener) { 
			mGfxWindowListeners.insert(listener); }
		inline virtual void RemoveResolutionChangeListener(IGfxWindowListener * listener) { 
			mGfxWindowListeners.erase(listener); }

	private:
		// maybe not needed ...
		SDL_Surface* mScreen;

		int32 mResx, mResy;
		bool mFullscreen;
		
		set<IGfxWindowListener*> mGfxWindowListeners;

	};
}

#endif
