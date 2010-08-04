#include "Common.h"
#include "GfxWindow.h"

#include "IGfxWindowListener.h"

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>

using namespace GfxSystem;

#ifdef __WIN__
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#endif


void GfxWindow::Init(const int32 x, const int32 y, const int32 windowWidth, const int32 windowHeight, const int32 resx, const int32 resy, const bool fullscreen, const string title)
{
	ocInfo << "*** GfxWindow Init ***";

	mWindowX = x;
	mWindowY = y;
	mWindowWidth = windowWidth;
	mWindowHeight = windowHeight;
	mFullscreenResolutionWidth = resx;
	mFullscreenResolutionHeight = resy;
	mFullscreen = fullscreen;

	// Initializes the video subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ocError << "Unable to init SDL: " << SDL_GetError();
		return;
	}

	int32 flags = SDL_OPENGL | SDL_RESIZABLE;
	if (mFullscreen) flags |= SDL_FULLSCREEN;

	// Set atributes for OpenGL
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Create drawing context
	SetWindowCaption(title);
	SetWindowPosition(mWindowX, mWindowY);

	if (mFullscreen) mScreen = SDL_SetVideoMode( mFullscreenResolutionWidth, mFullscreenResolutionHeight, 0, flags );
	else mScreen = SDL_SetVideoMode( mWindowWidth, mWindowHeight, 0, flags );

	if (mScreen != NULL) 
	{
		ocInfo << "SDL created drawing context for OpenGL. The video surface bits per pixel is "
			   << (int32)mScreen->format->BitsPerPixel;
		UpdateWindowPosition();
		SDL_ShowCursor(0);
	}
	else
	{
		ocError << "Video initialization failed: " << SDL_GetError();
		SDL_Quit();
	}


#ifdef __UNIX__
	/// SDL automatically registers handling of mouse events, but X11 allows only one client
	/// to handle mouse events. This hack reregisters event handling to allow OIS handle mouse.
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);

	if (!SDL_GetWMInfo(&info))
	{
		ocError << "Cannot get SDLWMInfo!";
	}
    XSetWindowAttributes attrs;
    attrs.event_mask = EnterWindowMask | LeaveWindowMask | ExposureMask;
    XChangeWindowAttributes(info.info.x11.display, info.info.x11.window, CWEventMask, &attrs);

	XWindowAttributes a;
	XGetWindowAttributes(info.info.x11.display, info.info.x11.window, &a);
	//ocWarning << a.all_event_masks << ", " << a.your_event_mask;
#endif
}

GfxWindow::~GfxWindow()
{
	mScreen = NULL;
	SDL_Quit();
}

bool GfxWindow::PopEvent(eWindowEvent &result)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			result = WE_QUIT;
			return true;
		case SDL_VIDEORESIZE:
			result = WE_RESIZE;
			ChangeResolution(event.resize.w, event.resize.h);
			return true;
		case SDL_ACTIVEEVENT:
			if ((event.active.state & SDL_APPINPUTFOCUS) != 0 || (event.active.state & SDL_APPACTIVE) != 0)
			{
				if (event.active.gain == 0) result = WE_LOST_FOCUS;
				else result = WE_GAINED_FOCUS;
				return true;
			}
			break;
		default:
			break;
		}
	}
	return false;
}

void GfxWindow::ChangeResolution(int32 resWidth, int32 resHeight, bool switchedFromFull)
{
	if (mFullscreen)
	{
		mFullscreenResolutionWidth = resWidth;
		mFullscreenResolutionHeight = resHeight;
	}
	else
	{
		mWindowWidth = resWidth;
		mWindowHeight = resHeight;
	}

	if (!switchedFromFull && !mFullscreen)
	{
		RefreshWindowPosition();
		SetWindowPosition(mWindowX, mWindowY);
	}

	set<IGfxWindowListener*>::iterator it;
	for(it = mGfxWindowListeners.begin(); it != mGfxWindowListeners.end(); ++it)
	{
		(*it)->ResolutionChanging(resWidth, resHeight);
	}
	
	// recreate drawing context
	int32 flags = SDL_OPENGL | SDL_RESIZABLE;
	if (mFullscreen) flags |= SDL_FULLSCREEN;
	
	mScreen = SDL_SetVideoMode( resWidth, resHeight, 0, flags );
	gGfxRenderer.Init();
	gResourceMgr.RefreshAllTextures();

	for(it = mGfxWindowListeners.begin(); it != mGfxWindowListeners.end(); ++it)
	{
		(*it)->ResolutionChanged(resWidth, resHeight);
	}
	gInputMgr.ReleaseAll();

	if (switchedFromFull)
	{
		RefreshWindowPosition();
		SetWindowPosition(mWindowX, mWindowY);
	}
}

void GfxWindow::SwitchFullscreen()
{
	mFullscreen = !mFullscreen;

	if (mFullscreen) 
		ChangeResolution(mFullscreenResolutionWidth, mFullscreenResolutionHeight);
	else 
		ChangeResolution(mWindowWidth, mWindowHeight, true);

}

WindowHandle GfxWindow::_GetWindowHandle() const
{
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);

	if (!SDL_GetWMInfo(&info))
	{
		ocError << "Cannot get SDL window handle!";
	}

#ifdef __WIN__
	HWND hWnd = info.window;
	OC_ASSERT(hWnd);
	return (WindowHandle)hWnd;
#else
    WindowHandle handle = info.info.x11.window;
    OC_ASSERT(handle);
    return handle;
#endif

}

void GfxSystem::GfxWindow::RefreshWindowPosition()
{
	if  (mFullscreen)
		return;

	SDL_SysWMinfo windowInfo;
	SDL_VERSION(&windowInfo.version);
	if (!SDL_GetWMInfo(&windowInfo))
	{
		ocError << "Can't retreive window info";
		return;
	}

#ifdef __WIN__
	RECT windowRect;
	GetWindowRect(windowInfo.window, &windowRect);
	mWindowX = windowRect.left;
	mWindowY = windowRect.top;
#else
	int windowX, windowY;
	windowInfo.info.x11.lock_func();
	Display* display = windowInfo.info.x11.display;
	Window window = windowInfo.info.x11.window;
	XSync(display, false);
	XWindowAttributes windowAttributes;
	XGetWindowAttributes(display, window, &windowAttributes);
	Window dummy;
	XTranslateCoordinates(display, window, windowAttributes.root, 0, 0, &windowX, &windowY, &dummy);
	mWindowX = static_cast<int32>(windowX);
	mWindowY = static_cast<int32>(windowY);
	windowInfo.info.x11.unlock_func();
#endif
}

void GfxSystem::GfxWindow::UpdateWindowPosition()
{
	SDL_SysWMinfo windowInfo;
	SDL_VERSION(&windowInfo.version);
	if (!SDL_GetWMInfo(&windowInfo))
	{
		ocError << "Can't retreive window info";
		return;
	}

#ifdef __WIN__
	SetWindowPos(windowInfo.window, 0, mWindowX, mWindowY, 0, 0, SWP_NOSIZE);
#else
	ocWarning << "X11 set window position not implemented";
#endif

}

void GfxSystem::GfxWindow::SetWindowCaption( const string& caption )
{
	if (GlobalProperties::Get<bool>("DevelopMode"))	SDL_WM_SetCaption(("Ocerus - " + caption).c_str(), NULL);
	else SDL_WM_SetCaption(caption.c_str(), NULL);
}

void GfxSystem::GfxWindow::SetWindowPosition(int32 xpos, int32 ypos)
{
#ifdef __UNIX__
	string windowPosEnvValue = StringConverter::ToString(xpos) + string(", ") + StringConverter::ToString(ypos);
	setenv("SDL_VIDEO_WINDOW_POS", windowPosEnvValue.c_str(), 1);
#else
	// adjustment, because SDL has it different than Windows
	xpos += 8;
	ypos += 30;
	string windowPosEnvVar = string("SDL_VIDEO_WINDOW_POS=") + StringConverter::ToString(xpos) + string(", ") + StringConverter::ToString(ypos);
	SDL_putenv(windowPosEnvVar.c_str()); // this is the way how to position the window in SDL
#endif
	
}