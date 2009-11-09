#include "Common.h"
#include "GfxWindow.h"

#include "IScreenListener.h"

#include <SDL/sdl.h>
#include <SDL/SDL_syswm.h>


using namespace SceneSystem;


 void GfxWindow::Init(const int32 resx, const int32 resy, const bool fullscreen, const string title)
{
	ocInfo << "*** GfxWindow Init ***";
	// Initializes the video subsystem
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ocError << "Unable to init SDL: " << SDL_GetError();
		return;
	}

	int32 flags = SDL_OPENGL;
	if (fullscreen)
		flags |= SDL_FULLSCREEN;

	// Set atributes for OpenGL
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	
	// Create drawing context
	mScreen = NULL;
	mScreen = SDL_SetVideoMode( resx, resy, 0, flags );	
	SDL_WM_SetCaption( title. c_str(), NULL );

	if (mScreen != NULL) {
		ocInfo << "SLD created drawing context for OpenGL. The video surface bits per pixel is " 
			   << (int)mScreen->format->BitsPerPixel;
		ocInfo << "Resolution: " << resx << "x" << resy;
		mResx = resx;
		mResy = resy;
		mFullscreen = fullscreen;
	}
	else 
	{
		ocError << "Video initialization failed: " << SDL_GetError();
		SDL_Quit();
	}

}

GfxWindow::~GfxWindow()
{
	mScreen = NULL;
	SDL_Quit();
}

bool GfxWindow::PopEvent(EWindowEvent &result) const
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
		default:
			break;
		}
	}
	return false;
}

uint32 GfxWindow::_GetWindowHandle() const
{
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	
	if (!SDL_GetWMInfo(&info))
	{
		ocError << "Cannot get SDL window handle!";
	}

	HWND hWnd = info.window;

	OC_ASSERT(hWnd);
	return (uint32)hWnd;
}