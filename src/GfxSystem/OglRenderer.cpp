#include "Common.h"
#include "OglRenderer.h"
#include "Texture.h"
#include "objloader/model_obj.h"

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

	// Set background color
	glClearColor( 0.1f, 0.1f, 0.1f, 0 );

	// Reset matrices
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Clear buffers
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Set max depth
	glClearDepth(1.0);

	// Set depth testing method
	glDepthFunc(GL_LEQUAL);

	// Set blending method
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable blending
	glEnable(GL_BLEND);

	// Specify the alpha test function (only pixels with alpha > 0.1 are drawn)
	glAlphaFunc(GL_GREATER, 0.1f);

	// Enable alpha testing
	glEnable(GL_ALPHA_TEST);
	
	// Enable texturing
	glEnable(GL_TEXTURE_2D);

	//Enable face culling
	//glEnable(GL_CULL_FACE);

	// create buffers for render2texture
	mFrameBuffer = 0;
	if (GLEW_EXT_framebuffer_object) glGenFramebuffersEXT(1, &mFrameBuffer);
	else ocWarning << "Missing OpenGL extension EXT_framebuffer_object";


	if( glGetError() != GL_NO_ERROR )
	{
		ocError << "OpenGL cannot initialize!";
		return;
	}

	SDL_GL_SwapBuffers();
}

bool OglRenderer::BeginRenderingImpl() const
{
	glViewport(0, 0, gGfxWindow.GetAppResolutionWidth(), gGfxWindow.GetAppResolutionHeight());
	glClear(GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	return true;
}

void OglRenderer::EndRenderingImpl() const
{
	SDL_GL_SwapBuffers();
}

void OglRenderer::SetViewportImpl(const GfxViewport* viewport)
{
	mCurrentViewport = viewport;

	if (viewport->AttachedToTexture())
	{
		OC_ASSERT(mFrameBuffer);
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
		glViewport(topleft.x, gGfxWindow.GetAppResolutionHeight()-bottomright.y, bottomright.x-topleft.x, bottomright.y-topleft.y);
		glCullFace(GL_FRONT);
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	Vector2 topleftWorld, bottomrightWorld;
	viewport->CalculateWorldBoundaries(topleftWorld, bottomrightWorld);
	// just another hack to make the bloody OpenGL in SDL working right
	if (!viewport->AttachedToTexture()) swap(bottomrightWorld.y, topleftWorld.y);
	glOrtho(topleftWorld.x, bottomrightWorld.x, topleftWorld.y, bottomrightWorld.y, -100.0f, 100.0f);

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
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void GfxSystem::OglRenderer::FlushGraphics() const
{
	glClear(GL_DEPTH_BUFFER_BIT);
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
	img = SOIL_load_image_from_memory(buffer, buffer_length, (int*)width, (int*)height, &channels, force_channels);
	if( NULL == img )
	{
		ocError << "SOIL error: " << SOIL_last_result();
		*width = *height = 0;
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
		*width = *height = 0;
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

	// Set texture wraping
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Disables texture filtering (filtering could cause artifacts at the edge between parts where Alpha=0 and Alpha=1)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, quad.texture);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f - quad.transparency);
	glTranslatef(quad.position.x, quad.position.y, quad.z);
	glRotatef(MathUtils::RadToDeg(quad.angle), 0, 0, 1);
	glScalef(quad.scale.x, quad.scale.y, 1);

	float32 x = 0.5f * quad.size.x / PIXELS_PER_WORLD_UNIT;
	float32 y = 0.5f * quad.size.y / PIXELS_PER_WORLD_UNIT;

	glBegin( GL_QUADS );

	glTexCoord2f(quad.texOffset.x, quad.texOffset.y + quad.frameSize.y); 
	glVertex3f(-x, y, 0);
	glTexCoord2f(quad.texOffset.x, quad.texOffset.y);
	glVertex3f(-x, -y, 0);
	glTexCoord2f(quad.texOffset.x + quad.frameSize.x, quad.texOffset.y); 
	glVertex3f( x, -y, 0);
	glTexCoord2f(quad.texOffset.x + quad.frameSize.x, quad.texOffset.y + quad.frameSize.y);
	glVertex3f( x, y, 0);

	glEnd();

	glPopMatrix();
}

void GfxSystem::OglRenderer::DrawTexturedMesh( const TexturedMesh& mesh ) const
{
	glPushMatrix();

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f - mesh.transparency );
	glTranslatef( mesh.position.x, mesh.position.y, mesh.z );
	glRotatef(MathUtils::RadToDeg(mesh.angle), 0, 0, 1);
	glRotatef(MathUtils::RadToDeg(mesh.yAngle), 0, 1, 0);
	glScalef(mesh.scale.x, mesh.scale.y, MathUtils::Max(mesh.scale.x, mesh.scale.y));

	ModelOBJ* model = (ModelOBJ*)mesh.mesh;
	OC_DASSERT(model);

	for (int i=0; i<model->getNumberOfMeshes(); ++i)
	{
		const ModelOBJ::Mesh* objMesh = &model->getMesh(i);
		const ModelOBJ::Material* objMaterial = objMesh->pMaterial;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, objMaterial->ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, objMaterial->diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, objMaterial->specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, objMaterial->shininess * 128.0f);

		if (!objMaterial->colorMapFilename.empty())
		{
			glEnable(GL_TEXTURE_2D);
			TexturePtr texRes = (TexturePtr)gResourceMgr.GetResource("MeshTextures", objMaterial->colorMapFilename);
			if (!texRes)
				texRes = (TexturePtr)gResourceMgr.GetResource("General", ResourceSystem::RES_NULL_TEXTURE);

			glBindTexture(GL_TEXTURE_2D, texRes->GetTexture());
		}

		if (model->hasPositions())
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, model->getVertexSize(), model->getVertexBuffer()->position);
		}

		if (model->hasTextureCoords())
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, model->getVertexSize(), model->getVertexBuffer()->texCoord);
		}

		if (model->hasNormals())
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, model->getVertexSize(), model->getVertexBuffer()->normal);
		}

		glDrawElements(GL_TRIANGLES, objMesh->triangleCount * 3, GL_UNSIGNED_INT, model->getIndexBuffer() + objMesh->startIndex);

		if (model->hasNormals())
		{
			glDisableClientState(GL_NORMAL_ARRAY);
		}

		if (model->hasTextureCoords())
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		if (model->hasPositions())
		{
			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}

	glPopMatrix();
}

void OglRenderer::DrawLine(const Vector2& a, const Vector2& b, const Color& color, const float32 width) const
{
	glDisable(GL_TEXTURE_2D);
	
	glColor4ub( color.r, color.g, color.b, color.a );

	glLineWidth(width);

	glBegin( GL_LINES );

	glVertex3f( a.x,  a.y, 0 );
	glVertex3f( b.x,  b.y, 0 );

	glEnd();

	glEnable(GL_TEXTURE_2D);
}


void OglRenderer::DrawPolygon(const Vector2* verts, const int32 n, const Color& color, const bool fill, const float32 outlineWidth) const
{
	OC_ASSERT(verts);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	glColor4ub( color.r, color.g, color.b, color.a );
	glLineWidth(outlineWidth);

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

void OglRenderer::DrawRect(	const Vector2& topleft, const Vector2& bottomright, const float32 rotation,
						   const Color& color, const bool fill) const
{
	Vector2 verts[4];

	verts[0] = Vector2(topleft.x, bottomright.y);
	verts[1] = topleft;
	verts[2] = Vector2(bottomright.x, topleft.y);
	verts[3] = bottomright;

	if (rotation != 0.0f)
	{
		// this calculation should be clear once you draw it on a paper :)
		Vector2 diagonal = bottomright - topleft;
		Matrix22 rotator(rotation);
		Vector2 r = MathUtils::Multiply(rotator, Vector2(1,0));
		r = MathUtils::Dot(r, diagonal) * r;
		verts[0] = topleft + r;
		verts[2] = bottomright - r;
	}

	DrawPolygon(verts, 4, color, fill);
}

void GfxSystem::OglRenderer::ClearScreen( const Color& color ) const
{
	glClearColor((float32)color.r / 255, (float)color.g / 255, (float32)color.b / 255, (float32)color.a / 2);
	glColorMask(true, true, true, true);
	glClear(GL_COLOR_BUFFER_BIT);
}

void GfxSystem::OglRenderer::ClearViewport( const GfxViewport& viewport, const Color& color ) const
{
	if (viewport.AttachedToTexture())
	{
		ClearScreen(color);
		return;
	}

	Point topleft, bottomright;
	viewport.CalculateScreenBoundaries(topleft, bottomright);
	// note that we are subtracting the Y pos from the resolution to workaround a bug in the SDL OpenGL impl
	glScissor(topleft.x, gGfxWindow.GetAppResolutionHeight()-bottomright.y, bottomright.x-topleft.x, bottomright.y-topleft.y);

	glEnable(GL_SCISSOR_TEST);

	glClearColor((float32)color.r / 255, (float)color.g / 255, (float32)color.b / 255, (float32)color.a / 2);
	glColorMask(true, true, true, true);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_SCISSOR_TEST);
}
