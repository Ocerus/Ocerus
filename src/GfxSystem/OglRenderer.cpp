#include "Common.h"
#include "OglRenderer.h"

#ifdef __WIN__
#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"glu32.lib")
#endif

#define GLEW_STATIC
#include "glew/GL/glew.h"
#include "SDL/SDL.h"
#include "SOIL.h"

using namespace GfxSystem;

void OglRenderer::Init()
{
	ocInfo << "*** OpenGL init ***";

	mCurrentViewport = 0;

	// init opengl extensions
	glewInit();

	glClearColor( 0.1f, 0.1f, 0.1f, 0 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
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

	// create buffers for render2texture
	glGenFramebuffersEXT(1, &mFrameBuffer);


	if( glGetError() != GL_NO_ERROR )
	{
		ocError << "OpenGL cannot initialize!";
		return;
	}

	SDL_GL_SwapBuffers();
}

bool OglRenderer::BeginRenderingImpl() const
{
	// we must disable automatic resource unloading here because the textures would be unloaded before they would be
	// rendered on the screen
	gResourceMgr.DisableMemoryLimitEnforcing();

	glViewport(0, 0, gGfxWindow.GetResolutionWidth(), gGfxWindow.GetResolutionHeight());
	glClear(GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	return true;
}

void OglRenderer::EndRenderingImpl() const
{
	// after the rendering is done, we must again enable automatic unloading of resources
	gResourceMgr.EnableMemoryLimitEnforcing();

	SDL_GL_SwapBuffers();
}

void OglRenderer::SetViewportImpl(const GfxViewport* viewport)
{
	mCurrentViewport = viewport;

	if (viewport->AttachedToTexture())
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFrameBuffer);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, viewport->GetRenderTexture(), 0);
		if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
		{
			ocError << "Failed to init framebuffer";
		}

		Point topleft, bottomright;
		viewport->CalculateScreenBoundaries(topleft, bottomright);
		glViewport(topleft.x, topleft.y, bottomright.x-topleft.x, bottomright.y-topleft.y);
		glCullFace(GL_FRONT);
	}
	else
	{
		Point topleft, bottomright;
		viewport->CalculateScreenBoundaries(topleft, bottomright);
		// note that we are subtracting the Y pos from the resolution to workaround a bug in the SDL OpenGL impl
		glViewport(topleft.x, gGfxWindow.GetResolutionHeight()-bottomright.y, bottomright.x-topleft.x, bottomright.y-topleft.y);
		glCullFace(GL_BACK);
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	Vector2 topleftWorld, bottomrightWorld;
	viewport->CalculateWorldBoundaries(topleftWorld, bottomrightWorld);
	// just another hack to make the bloody OpenGL in SDL working right
	if (!viewport->AttachedToTexture()) swap(bottomrightWorld.y, topleftWorld.y);
	glOrtho(topleftWorld.x, bottomrightWorld.x, topleftWorld.y, bottomrightWorld.y, -1, 1);

	glMatrixMode(GL_MODELVIEW);
}

void OglRenderer::SetCameraImpl(const Vector2& position, const float32 zoom, const float32 rotation) const
{
	glLoadIdentity();
	glRotatef(MathUtils::RadToDeg(-rotation), 0, 0, 1);
	glScalef(zoom, zoom, zoom);
	glTranslatef(-position.x, -position.y, 0);
}

void OglRenderer::FinalizeRenderTargetImpl() const
{
	if (mCurrentViewport->AttachedToTexture())
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
	glClear ( GL_DEPTH_BUFFER_BIT );
}

GfxSystem::TextureHandle GfxSystem::OglRenderer::CreateRenderTexture( const uint32 width, const uint32 height ) const
{
	TextureHandle result;
	glGenTextures(1, &result);
	glBindTexture(GL_TEXTURE_2D, result);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if( glGetError() != GL_NO_ERROR )
	{
		ocError << "Failed to create render texture!";
		return InvalidTextureHandle;
	}

	return result;
}

TextureHandle OglRenderer::LoadTexture( const uint8* const buffer, const int32 buffer_length, const ePixelFormat force_channels, 
									   const uint32 reuse_texture_ID, int32* width, int32* height ) const
{
	uint8* img;
	int channels = 0;
	img = SOIL_load_image_from_memory(buffer, buffer_length, width, height, &channels, force_channels);
	if( NULL == img )
	{
		ocError << "SOIL error: " << SOIL_last_result();
		return 0;
	}
	if( (force_channels >= 1) && (force_channels <= 4) )
	{
		channels = force_channels;
	}

	TextureHandle result = SOIL_create_OGL_texture(img, *width, *height, channels, reuse_texture_ID, 0);

	SOIL_free_image_data( img );

	if( 0 == result )
	{
		ocError << "SOIL loading error: " << SOIL_last_result();
	}
	return result;
}


void OglRenderer::DeleteTexture(const TextureHandle& handle) const
{
	glDeleteTextures(1, &handle);
}

void OglRenderer::DrawTexturedQuad(const TexturedQuad& quad) const
{	
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, quad.texture);
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f - quad.transparency );
	glTranslatef( quad.position.x, quad.position.y, quad.z );
	glRotatef(MathUtils::RadToDeg(quad.angle), 0, 0, 1);
	glScalef(quad.scale.x, quad.scale.y, 1);

	float32 x = quad.size.x/2;
	float32 y = quad.size.y/2;

	glBegin( GL_QUADS );

	glTexCoord2f(0.0f, 1.0f); glVertex3f( -x,  y, 0 );
	glTexCoord2f(1.0f, 1.0f); glVertex3f(  x,  y, 0 );
	glTexCoord2f(1.0f, 0.0f); glVertex3f(  x, -y, 0 );
	glTexCoord2f(0.0f, 0.0f); glVertex3f( -x, -y, 0 );

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

	if (fill) glBegin( GL_POLYGON );
	else glBegin( GL_LINE_LOOP );

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

	if (fill) glBegin( GL_POLYGON );
	else glBegin( GL_LINE_LOOP );

	for(float32 angle = 6.2832f; angle >= 0; angle -= 0.2f)
	{
		glVertex2f(position.x + sin(angle) * radius, position.y + cos(angle) * radius);
	}

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

	glRotatef(MathUtils::RadToDeg(rotation), 0, 0, 1);

	DrawPolygon(verts, 4, color, fill);

	glPopMatrix();
}

void GfxSystem::OglRenderer::ClearScreen( const Color& color ) const
{
	glClearColor((float32)color.r / 255, (float)color.g / 255, (float32)color.b / 255, (float32)color.a / 2);
	glColorMask(true, true, true, true);
	glClear(GL_COLOR_BUFFER_BIT);
}
