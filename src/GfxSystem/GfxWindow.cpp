#include "Common.h"
#include "GfxWindow.h"

#include "IGfxWindowListener.h"

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>


using namespace GfxSystem;


void GfxWindow::Init(const int32 resx, const int32 resy, const bool fullscreen, const string title)
{
	ocInfo << "*** GfxWindow Init ***";
	// Initializes the video subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ocError << "Unable to init SDL: " << SDL_GetError();
		return;
	}

	mFullscreen = fullscreen;

	int32 flags = SDL_OPENGL | SDL_RESIZABLE;
	if (mFullscreen) flags |= SDL_FULLSCREEN;

	// Set atributes for OpenGL
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#ifdef __WIN__
	// Causes "Couldn't find matching GLX visual" on my linux
	// Maybe those values should be stored in config
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
#endif

	// Create drawing context
	SDL_WM_SetCaption( title.c_str(), NULL );
	mScreen = SDL_SetVideoMode( resx, resy, 0, flags );

	if (mScreen != NULL) {
		ocInfo << "SDL created drawing context for OpenGL. The video surface bits per pixel is "
			   << (int32)mScreen->format->BitsPerPixel;
		ocInfo << "Resolution: " << resx << "x" << resy;
		mResx = resx;
		mResy = resy;
		mFullscreen = fullscreen;
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
			//break;
		case SDL_VIDEORESIZE:
			result = WE_RESIZE;
			ChangeResolution(event.resize.w, event.resize.h);
			return true;
		default:
			break;
		}
	}
	return false;
}

void GfxWindow::ChangeResolution(int32 x, int32 y)
{
	mResx = x;
	mResy = y;

	set<IGfxWindowListener*>::iterator it;
	for(it = mGfxWindowListeners.begin(); it != mGfxWindowListeners.end(); ++it)
	{
		(*it)->ResolutionChanged(mResx, mResy);
	}
	
	// recreate drawing index
	int32 flags = SDL_OPENGL | SDL_RESIZABLE;
	if (mFullscreen)
		flags |= SDL_FULLSCREEN;
	
	mScreen = SDL_SetVideoMode( mResx, mResy, 0, flags );
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
	return (uint32)hWnd;
#else
    WindowHandle handle = info.info.x11.window;
    OC_ASSERT(handle);
    return handle;
#endif

}
