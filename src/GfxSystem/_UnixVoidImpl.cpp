#include "Common.h"
#include "GfxRenderer.h"
#include "Triangulate.h"
#include "Texture.h"
#include "IScreenListener.h"
#include "../Core/Application.h"
#include <cstdio>
#include <X11/Xlib.h>
#include "LogSystem/LogMgr.h"

using namespace GfxSystem;

GfxRenderer::GfxRenderer( void ):
	mCameraX(0),
	mCameraY(0),
	mCameraScale(1.0f),
	mHGE(0),
    mX11WindowId(0),
	mX11Display(0)
{
}

void GfxRenderer::Init( const Point& resolution, bool fullscreen )
{
    gLogMgr.LogMessage("*** GfxRenderer init ***");
	int screen;
    mX11Display = XOpenDisplay(NULL);
    if (mX11Display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
    screen = DefaultScreen(mX11Display);
    mX11WindowId = XCreateSimpleWindow(mX11Display, RootWindow(mX11Display, screen), 10, 10,
		resolution.x, resolution.y, 1, BlackPixel(mX11Display, screen), WhitePixel(mX11Display, screen));
	XSelectInput(mX11Display, mX11WindowId, StructureNotifyMask);
    XMapWindow(mX11Display, mX11WindowId);

	XEvent e;
	//Wait for Window to show up
	do { XNextEvent(mX11Display, &e); } while(e.type != MapNotify);
}

GfxRenderer::~GfxRenderer()
{
	if (mX11Display != NULL) {
		XCloseDisplay(mX11Display);
	}
}

uint64 GfxRenderer::_GetWindowId() const
{
    return mX11WindowId;
}

_XDisplay* GfxRenderer::_GetDisplay() const
{
	return mX11Display;
}

void GfxRenderer::ChangeResolution( const uint32 width, const uint32 height )
{
}

Point GfxRenderer::GetResolution() const
{
}

void GfxRenderer::SetFullscreen(bool fullscreen)
{
}

bool GfxRenderer::BeginRendering(void) const
{
}

bool GfxRenderer::EndRendering(void) const
{
}

bool GfxRenderer::ClearScreen(const Color& color) const
{
}

bool GfxRenderer::DrawLine(int x1, int y1, int x2, int y2, const Pen& pen) const
{
}

bool GfxRenderer::DrawLine( const Point& begin, const Point& end, const Pen& pen ) const
{
}

bool GfxRenderer::DrawLineWithConversion( const Vector2& begin, const Vector2& end, const Pen& pen ) const
{
}

bool GfxRenderer::DrawImage( const TexturePtr& image, int32 x, int32 y, uint8 anchor, float32 angle, const Color& color, float32 scale) const
{
    return 0;
}

bool GfxRenderer::DrawImage( const TexturePtr& image, const Point& pos, uint8 anchor, float32 angle, const Color& color, float32 scale ) const
{
}

bool GfxRenderer::DrawImageWithConversion( const TexturePtr& image, const Vector2& pos, uint8 anchor, float32 angle, const Color& color, float32 scale ) const
{
}

bool GfxRenderer::DrawImage( const TexturePtr& image, const Rect& destRect ) const
{
}

bool GfxRenderer::DrawImage( const TexturePtr& image, const Rect& textureRect, const Rect& destRect, const ColorRect& colors ) const
{
}

bool GfxRenderer::DrawPolygon( Point* vertices, int vertices_len, const TexturePtr& image, const Pen& outline, float32 angle, const Color& color, float32 scale, float32 textureAngle, float32 textureScale ) const
{
	return 0;
}

bool GfxRenderer::DrawPolygon( const vector<Point>& vertices, const TexturePtr& image, const Pen& outline, float32 angle, const Color& color, float32 scale, float32 textureAngle, float32 textureScale ) const
{
	return false;
}

bool GfxRenderer::DrawPolygon( Point* vertices, int vertices_len, const Color& fillColor, const Pen& outline) const
{
	return false;
}

bool GfxRenderer::DrawPolygonWithConversion( const Vector2* vertices, int vertices_len, const Vector2& offsetPosition, const float32 offsetAngle, const Color& fillColor, const Pen& outline) const
{
	return false;
}

bool GfxRenderer::DrawPolygon( const vector<Point>& vertices, const Color& fillColor, const Pen& outline) const
{
	return true;
}

bool GfxRenderer::DrawCircle( const Point& center, const int32 radius, const Color& fillColor, const Pen& outline, const float32 minAng, const float32 maxAng ) const
{
	return true;
}

int32 GfxRenderer::WorldToScreenX( const float32 x ) const
{
	return 0;
}

int32 GfxRenderer::WorldToScreenY( const float32 y ) const
{
	return 0;
}

Point GfxRenderer::WorldToScreen( const Vector2& pos ) const
{
	return Point(0, 0);
}

float32 GfxRenderer::WorldToScreenImageScale( const float32 scale ) const
{
	return 0;
}

float32 GfxRenderer::ScreenToWorldX( const int32 x ) const
{
	return 0;
}

float32 GfxRenderer::ScreenToWorldY( const int32 y ) const
{
	return 0;
}

Vector2 GfxRenderer::ScreenToWorld( const Point& pos ) const
{
	return Vector2(0, 0);
}

void GfxRenderer::ZoomCamera( const float32 delta )
{
}

void GfxRenderer::MoveCamera( const float32 dx, const float32 dy )
{
}

Vector2 GfxRenderer::GetCameraWorldBoxTopLeft( void ) const
{
	return Vector2(0, 0);
}

Vector2 GfxRenderer::GetCameraWorldBoxBotRight( void ) const
{
	return Vector2(0, 0);
}

bool GfxRenderer::IsFullscreen( void ) const
{
	return 0;
}

void GfxRenderer::DrawString( float32 x, float32 y, const string & id,
							 const string & text, const Color color,
							 uint8 text_anchor, uint8 screen_anchor, const string & fontid )
{
}

Vector2 GfxRenderer::GetTextSize( const string & text, const string & fontid )
{
	return Vector2(0, 0);
}

bool GfxRenderer::DrawQuad( GfxSystem::Point* const vertices, const TexturePtr texture, Vector2* const textureCoords, Color* const vertexColors )
{
	return true;
}

bool GfxRenderer::GetWindowPosition( Point& out )
{
	return 0;
}

// init null values
Pen Pen::NullPen(Color(0,0,0,0));
Rect Rect::NullRect(0,0,0,0);
Color Color::NullColor(0,0,0,0);
Color Color::FullColor(255,255,255,255);

#include "ParticleResource.h"

ResourceSystem::ResourcePtr ParticleResource::CreateMe(void)
{
	return ResourceSystem::ResourcePtr(new ParticleResource());
}

ParticleResource::~ParticleResource(void)
{

}

int32 ReadInt(InputStream& is)
{
	int a = is.get();
	int b = is.get();
	int c = is.get();
	int d = is.get();
	return a + (b<<8) + (c<<16) + (d<<24);
}

float32 ReadFloat(InputStream& is)
{
	union
	{
		float32 f;
		unsigned char b[4];
	} dat1;
	dat1.b[0] = is.get();
	dat1.b[1] = is.get();
	dat1.b[2] = is.get();
	dat1.b[3] = is.get();
	return dat1.f;
}


bool ParticleResource::LoadImpl(void)
{
	return true;
}

bool ParticleResource::UnloadImpl(void)
{

}

ParticleSystem::ParticleSystem(hgeParticleSystem* ps):
	mPs(ps),
	mRenderDone(false),
	mLoaded(true),
	mActive(false),
	mScale(1),
	mPositionX(0),
	mPositionY(0),
	mLastPositionX(0),
	mLastPositionY(0) {}

ParticleSystem::~ParticleSystem(void)
{
}

void ParticleSystem::Unload(void)
{
}

void ParticleSystem::MoveTo(float32 x, float32 y, bool bMoveParticles)
{
}


void ParticleSystem::SetScale(float32 scale)
{
}

void ParticleSystem::SetAngle(float32 angle)
{
}

void ParticleSystem::SetSpeed(float32 fSpeedMin, float32 fSpeedMax)
{
}

hgeParticleSystem* ParticleSystem::GetPS(void)
{
}

void ParticleSystem::FireAt(float32 x, float32 y)
{
}

void ParticleSystem::Fire(void)
{
}

void ParticleSystem::Stop(void)
{
}

void ParticleSystem::Update(float32 delta)
{
}

void ParticleSystem::Render()
{
}

int32 ParticleSystem::GetParticlesAlive(void) const
{
    return 0;
}

float32 ParticleSystem::GetAge(void) const
{
    return 0;
}

bool ParticleSystem::GetRenderDone(void) const
{
    return 0;
}

void ParticleSystem::SetRenderDone(void)
{
}

float32 ParticleSystem::GetLifeTime(void) const
{
	return 0;
}

float32 ParticleSystem::GetWorldX(void) const
{
	return mPositionX;
}

float32 ParticleSystem::GetWorldY(void) const
{
	return mPositionY;
}

ParticleSystemMgr::ParticleSystemMgr(void)
{
}

ParticleSystemMgr::~ParticleSystemMgr(void)
{
}

void ParticleSystemMgr::Update(float32 delta)
{
}

void ParticleSystemMgr::Render(float32 scale)
{
}

void ParticleSystemMgr::SetScale(float32 s)
{
}

ParticleSystemPtr ParticleSystemMgr::SpawnPS(StringKey group, StringKey name)
{
}

ParticleSystemPtr ParticleSystemMgr::SpawnPS(char* groupSlashName)
{
}

ParticleSystemPtr ParticleSystemMgr::SpawnPS(StringKey group, StringKey name, int32 x, int32 y)
{
}

ParticleSystemPtr ParticleSystemMgr::SpawnPS(char* groupSlashName, int32 x, int32 y)
{
}

bool ParticleSystemMgr::IsPSAlive(ParticleSystemPtr ps)
{
}

void ParticleSystemMgr::UnregisterPS(ParticleSystemPtr ps)
{
}

void ParticleSystemMgr::KillAll(void)
{
}


Texture::Texture( void ): mHandle(0) {}

Texture::~Texture( void ) {}

ResourceSystem::ResourcePtr Texture::CreateMe()
{
	return ResourceSystem::ResourcePtr(new Texture());
}

void Texture::Init()
{
}

bool Texture::LoadImpl()
{
}

bool Texture::UnloadImpl()
{
}

uint32 Texture::GetWidth(/*bool bOriginal*/)
{
}

uint32 Texture::GetHeight(/*bool bOriginal */)
{
}

uint32 Texture::GetTexture()
{
}

bool Texture::LoadFromBitmap( const void* pixels, uint32 pixelsLength, uint32 width, uint32 height, ePixelFormat format )
{
}

float Triangulate::Area(const Vector2dVector &contour)
{
}

bool Triangulate::InsideTriangle(float Ax, float Ay,
                      float Bx, float By,
                      float Cx, float Cy,
                      float Px, float Py)

{
}

bool Triangulate::Snip(const Vector2dVector &contour,int u,int v,int w,int n,int *V)
{
}

bool Triangulate::Process(const Vector2dVector &contour,Vector2dVector &result)
{
  return true;
}
