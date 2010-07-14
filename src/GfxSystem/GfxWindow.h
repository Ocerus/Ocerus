/// @file
/// Application window management.

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Base.h"
#include "Singleton.h"
#include "GfxSystem/GfxStructures.h"


// Forward declarations.
struct SDL_Surface;

/// Macro for easier use.
#define gGfxWindow GfxSystem::GfxWindow::GetSingleton()

namespace GfxSystem
{
	/// Event that happen to the window while the app is running.
	enum eWindowEvent {
		WE_QUIT, ///< User tries to close the window (map to SDL_QUIT).
		WE_RESIZE, ///< User tries to resize the window (map to SDL_VIDEORESIZE).
		WE_LOST_FOCUS, ///< The application has lost the focus.
		WE_GAINED_FOCUS ///< The application has gained the focus.
	};

	/// Handle to the platform specific window.
    typedef uintptr WindowHandle;


	/// Represents the only window the game can have. The window can also be spanned to the whole screen when switched
	/// to fullscreen.
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

		/// Switches fullscreen on and off.
		void SwitchFullscreen();

		/// Returns the window width resolution.
		inline int32 GetResolutionWidth() const { return mResx; }

		/// Returns the window height resolution.
		inline int32 GetResolutionHeight() const { return mResy; }

		/// Returns the window resolution.
		inline Point GetResolution() const { return Point(mResx, mResy); }
		
		/// Returns true if the window is in fullscreen mode.
		inline bool GetFullscreen() const { return mFullscreen; }

		/// Gets windows handle. Windows OS only.
		WindowHandle _GetWindowHandle() const;

		/// Adds listener to window resolution change.
		inline virtual void AddScreenListener(IGfxWindowListener * listener) { mGfxWindowListeners.insert(listener); }

		/// Removes listener to window resolution change.
		inline virtual void RemoveScreenListener(IGfxWindowListener * listener) { mGfxWindowListeners.erase(listener); }

	private:

		SDL_Surface* mScreen;
		int32 mResx, mResy;
		bool mFullscreen;

		set<IGfxWindowListener*> mGfxWindowListeners;

	};
}

#endif
