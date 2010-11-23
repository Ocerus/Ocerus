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
		void Init(const int32 x, const int32 y, const int32 windowWidth, const int32 windowHeight, const int32 resx, const int32 resy, const bool fullscreen, const string title);

		/// Destroys the window.
		~GfxWindow();

		/// If there is known event in queue, returns it. Otherwise empties the queue and return false.
		bool PopEvent(eWindowEvent& result);

		/// Sets new caption to the window.
		void SetWindowCaption(const string& caption);

		/// Changes window resolution, recreates drawing context, invokes ScreenListeners.
		void ChangeResolution(const int32 x, const int32 y, const bool switchedFromFull = false);
		
		/// Sets fullscreen mode.
		void SetFullscreen(const bool value);

		/// Switches fullscreen on and off.
		void SwitchFullscreen();

		/// Returns the window width resolution.
		inline int32 GetFullscreenResolutionWidth() const { return mFullscreenResolutionWidth; }

		/// Returns the window height resolution.
		inline int32 GetFullscreenResolutionHeight() const { return mFullscreenResolutionHeight; }

		/// Returns the window resolution.
		inline Point GetFullscreenResolution() const { return Point(mFullscreenResolutionWidth, mFullscreenResolutionHeight); }

		/// Returns the window X position.
		inline int32 GetWindowX() { RefreshWindowPosition(); return mWindowX; }
		
		/// Returns the window Y position.
		inline int32 GetWindowY() { RefreshWindowPosition(); return mWindowY; }

		/// Returns the window width.
		inline int32 GetWindowWidth() const { return mWindowWidth; }

		/// Returns the window height.
		inline int32 GetWindowHeight() const { return mWindowHeight; }

		/// Returns application window resolution width.
		inline int32 GetAppResolutionWidth() const;

		/// Returns application window resolution height.
		inline int32 GetAppResolutionHeight() const;
		
		/// Returns system resolution width. Must be called after SDL_INIT.
		int32 GetSystemResolutionWidth() const;

		/// Returns system resolution height. Must be called after SDL_INIT.
		int32 GetSystemResolutionHeight() const;

		/// Returns true if the window is in fullscreen mode.
		inline bool IsFullscreen() const { return mFullscreen; }

		/// Sets window position.
		void SetWindowPosition(int32 xpos, int32 ypos);

		/// Gets windows handle. Windows OS only.
		WindowHandle _GetWindowHandle() const;

		/// Adds listener to window resolution change.
		inline virtual void AddScreenListener(IGfxWindowListener * listener) { mGfxWindowListeners.insert(listener); }

		/// Removes listener to window resolution change.
		inline virtual void RemoveScreenListener(IGfxWindowListener * listener) { mGfxWindowListeners.erase(listener); }

		/// Sets whether the SDL cursor should be shown.
		void SetSDLCursorVisibility(const bool value);

		/// Returns whether the SDL cursor is shown.
		bool GetSDLCursor();

	private:

		SDL_Surface* mScreen;
		int32 mWindowX;
		int32 mWindowY;
		int32 mWindowWidth;
		int32 mWindowHeight;
		int32 mFullscreenResolutionWidth;
		int32 mFullscreenResolutionHeight;
		bool mFullscreen;

		set<IGfxWindowListener*> mGfxWindowListeners;

		void UpdateWindowPosition();
		void RefreshWindowPosition();
	};

	
	//---------------
	// Implementation

	int32 GfxSystem::GfxWindow::GetAppResolutionWidth() const
	{
		if (mFullscreen) return mFullscreenResolutionWidth;
		else return mWindowWidth;
	}

	int32 GfxSystem::GfxWindow::GetAppResolutionHeight() const
	{
		if (mFullscreen) return mFullscreenResolutionHeight;
		else return mWindowHeight;
	}
}

#endif
