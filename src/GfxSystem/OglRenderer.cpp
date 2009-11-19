#include "Common.h"
#include "OglRenderer.h"

#ifdef __WIN__
#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"glu32.lib")
#endif

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include "SOIL.h"


using namespace GfxSystem;


void OglRenderer::Init()  const
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

bool OglRenderer::BeginRendering()  const
{
	glClear( GL_COLOR_BUFFER_BIT );
    glLoadIdentity();
	return true;
}

void OglRenderer::EndRendering()  const
{
	SDL_GL_SwapBuffers();
}

uint32 OglRenderer::LoadTexture(
			const unsigned char *const buffer,
			int buffer_length,
			int force_channels,
			unsigned int reuse_texture_ID ) const
{
	uint32 result = SOIL_load_OGL_texture_from_memory(buffer, buffer_length, force_channels, reuse_texture_ID, 0);
	
	if( 0 == result )
	{
		ocError << "SOIL loading error: " << SOIL_last_result();
	}
	return result;
}


void OglRenderer::DeleteTexture(const uint32 &handle) const
{
	glDeleteTextures(1, &handle);
}

void OglRenderer::DrawTestQuad() const
{
	glTranslatef( 412, 283, 0 );

	glBegin( GL_QUADS );

        //Set color to red
        glColor4f( 1.0, 0.0, 0.0, 1.0 );

        //Draw square
	    glVertex3f( 0,		0,		0 );
	    glVertex3f( 200,	0,		0 );
	    glVertex3f( 200,	200,	0 );
	    glVertex3f( 0,		200,	0 );

    //End quad
    glEnd();
}

void OglRenderer::DrawTestTexturedQuad(const uint32 text_handle) const
{
	glTranslatef( 256, 256, 0 );

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, text_handle);

	glColor3f (1.0,1.0,1.0);

	glBegin( GL_QUADS );
        //Draw square
		glTexCoord2f(0.0f, 0.0f);
	    glVertex2f( 0,		0	);
		glTexCoord2f(1.0f, 0.0f);
	    glVertex2f( 512,	0	);
		glTexCoord2f(1.0f, 1.0f);
	    glVertex2f( 512,	256 );
		glTexCoord2f(0.0f, 1.0f);
	    glVertex2f( 0,		256 );

    //End quad
    glEnd();
	glDisable(GL_TEXTURE_2D);
	glFlush ();


}
