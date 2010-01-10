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

void OglRenderer::Init() const
{
	ocInfo << "*** OpenGL init ***";
	//Initialize OpenGL

	glClearColor( 0.1f, 0.1f, 0.1f, 0 );
	glMatrixMode( GL_PROJECTION );
	glOrtho( -smOrthoSizeX, smOrthoSizeX, -smOrthoSizeY, smOrthoSizeY, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glDepthFunc(GL_LEQUAL);						
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);

	if( glGetError() != GL_NO_ERROR )
    {
		ocError << "OpenGL cannot initialize!";
        return;
    }

	SDL_GL_SwapBuffers();
}

bool OglRenderer::BeginRendering() const
{
	// we must disable automatic resource unloading here because the textures would be unloaded before they would be
	// rendered on the screen
	gResourceMgr.DisableMemoryLimitEnforcing();

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
	return true;
}

void OglRenderer::EndRendering() const
{
	// after the rendering is done, we must again enable automatic unloading of resources
	gResourceMgr.EnableMemoryLimitEnforcing();

	SDL_GL_SwapBuffers();
}

void OglRenderer::FinalizeViewport() const
{
	glClear ( GL_DEPTH_BUFFER_BIT );
}

uint32 OglRenderer::LoadTexture(
			const unsigned char *const buffer,
			const int buffer_length,
			const ePixelFormat force_channels,
			const unsigned int reuse_texture_ID,
			int *width, int *height) const
{
	// this doesnt say anything about width and height
	//uint32 result = SOIL_load_OGL_texture_from_memory(buffer, buffer_length, 1, reuse_texture_ID, 0);

	unsigned char* img;
	int channels = 0;
	img = SOIL_load_image_from_memory(
					buffer, buffer_length,
					width, height, &channels,
					force_channels );
	if( NULL == img )
	{
		ocError << "SOIL error: " << SOIL_last_result();
		return 0;
	}
	if( (force_channels >= 1) && (force_channels <= 4) )
	{
		channels = force_channels;
	}

	uint32 result = SOIL_create_OGL_texture(
			img, *width, *height, channels,
			reuse_texture_ID, 0);

	SOIL_free_image_data( img );
	
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

void OglRenderer::SetTexture(const uint32 texture) const
{
	glBindTexture(GL_TEXTURE_2D, texture);
}

void OglRenderer::DrawTexturedQuad(	const Vector2& position,
									const Vector2& size,
									const float32 z,
									const float32 transp) const
{	
	glPushMatrix();

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f - transp );

	glTranslatef( position.x, position.y, z );

	float32 x = size.x/2;
	float32 y = size.y/2;

	glBegin( GL_QUADS );

        //Draw square
	    glTexCoord2f(0.0f, 0.0f); glVertex3f( -x,  y, z );
		glTexCoord2f(0.0f, 1.0f); glVertex3f( -x, -y, z );
		glTexCoord2f(1.0f, 1.0f); glVertex3f(  x, -y, z );
		glTexCoord2f(1.0f, 0.0f); glVertex3f(  x,  y, z );

    //End quad
    glEnd();

	glPopMatrix();
}

 void OglRenderer::DrawLine(const Vector2* verts, const Color& color) const
{
	OC_ASSERT(verts);

	glDisable(GL_TEXTURE_2D);
	glColor4ub( color.r, color.g, color.b, color.a );
	
	glBegin( GL_LINES );
		glVertex3f( verts[0].x,  verts[0].y, 0 );
		glVertex3f( verts[1].x,  verts[1].y, 0 );
    glEnd();

	glEnable(GL_TEXTURE_2D);
}

 
void OglRenderer::DrawPolygon(const Vector2* verts, const int32 n, const Color& color, const bool fill) const
{
	OC_ASSERT(verts);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glColor4ub( color.r, color.g, color.b, color.a );
	
	fill?
		glBegin( GL_POLYGON ):
		glBegin( GL_LINE_LOOP );

		for (int32 i = 0; i < n; ++i)
		{
			glVertex3f( verts[i].x,  verts[i].y, 0 );
		}
    glEnd();

	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
}

void OglRenderer::DrawCircle(const Vector2& position, const float32 radius, const Color& color, const bool fill) const
{
	glDisable(GL_TEXTURE_2D);
	glColor4ub( color.r, color.g, color.b, color.a );

	Vector2 circle;
	
	fill?
		glBegin( GL_POLYGON ):
		glBegin( GL_LINE_LOOP );

	    for(float32 angle = 6.2832f; angle >= 0; angle -= 0.2f)
	    {
	      glVertex2f(position.x + sin(angle) * radius, position.y + cos(angle) * radius);

	    }
	    glEnd();

    glEnd();

	glEnable(GL_TEXTURE_2D);
}

void OglRenderer::DrawRect(	const Vector2& position, const Vector2& size, const float32 rotation,
							const Color& color, const bool fill) const
{
	Vector2 verts[4];

	float32 x = size.x/2;
	float32 y = size.y/2;

	verts[0] = position + Vector2(-x,  y);
	verts[1] = position + Vector2(-x, -y);
	verts[2] = position + Vector2( x, -y);
	verts[3] = position + Vector2( x,  y);

	glPushMatrix();

	glRotatef(rotation, 0, 0, 1);

	DrawPolygon(verts, 4, color, fill);

	glPopMatrix();
}

void OglRenderer::SetViewport(const GfxViewport& viewport) const
{
	int32 resx = gGfxWindow.GetResolutionWidth();
	int32 resy = gGfxWindow.GetResolutionHeight();

	// set viewport (part of window)
	glViewport (	(int32)(viewport.position.x *	resx),	(int32)(viewport.position.y * resy),
					(int32)(viewport.size.x		*	resx),	(int32)(viewport.size.y		* resy));
	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	// set projection (part of world)
	if (viewport.relative)
	{
		// size of objects depend on resolution
		glOrtho(	-smOrthoSizeX * viewport.size.x, smOrthoSizeX * viewport.size.x,
					-smOrthoSizeY * viewport.size.y, smOrthoSizeY * viewport.size.y,
					-1, 1 );
	}
	else
	{
		// size of objects doesn't depend on resolution
		glOrtho(	-resx/2 * viewport.size.x, resx/2 * viewport.size.x,
					-resy/2 * viewport.size.y, resy/2 * viewport.size.y,
					-1, 1 );
	}

	glMatrixMode( GL_MODELVIEW );
}
	
void OglRenderer::SetCamera(const Vector2& position, const float32 zoom, const float32 rotation) const
{
	glLoadIdentity();
	glRotatef(rotation, 0, 0, 1);
	glScalef(zoom, zoom, zoom);
	glTranslatef(-position.x, -position.y, 0);
}