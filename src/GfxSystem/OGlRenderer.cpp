#include "Common.h"
#include "OGlRenderer.h"

#ifdef __WIN__
#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"glu32.lib")
#endif

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"


using namespace GfxSystem;


void OGlRenderer::Init()
{
	ocInfo << "*** OpenGL init ***";
	//Initialize OpenGL

	glClearColor( 0, 0, 0, 0 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, 1024, 768, 0, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glClear( GL_COLOR_BUFFER_BIT );

	if( glGetError() != GL_NO_ERROR )
    {
		ocError << "OpenGL cannot initialize!";
        return;
    }

	SDL_GL_SwapBuffers();
}

bool OGlRenderer::BeginRendering()
{
	glClear( GL_COLOR_BUFFER_BIT );
    glLoadIdentity();
	return true;
}

void OGlRenderer::EndRendering()
{
	SDL_GL_SwapBuffers();
}

void OGlRenderer::DrawTestQuad()
{
	glTranslatef( 412, 283, 0 );

	glBegin( GL_QUADS );

        //Set color to white
        glColor4f( 1.0, 0.0, 0.0, 1.0 );

        //Draw square
	    glVertex3f( 0,		0,		0 );
	    glVertex3f( 200,	0,		0 );
	    glVertex3f( 200,	200,	0 );
	    glVertex3f( 0,		200,	0 );

    //End quad
    glEnd();
}
