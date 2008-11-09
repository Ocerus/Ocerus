#include "GfxRenderer.h"
#include "../Common.h"
#include "../Core/Application.h"
#include <hge.h>

using namespace GfxSystem;

Pen Pen::NullPen(Color(0,0,0,0));

GfxRenderer::GfxRenderer(const Point& resolution, bool fullscreen) 
{
	gLogMgr.LogMessage("*** GfxRenderer init ***");

	mHGE = hgeCreate(HGE_VERSION);
	mHGE->System_SetState(HGE_TITLE, "Battleships");
	mHGE->System_SetState(HGE_SCREENWIDTH, resolution.x);
	mHGE->System_SetState(HGE_SCREENHEIGHT, resolution.y);
	mHGE->System_SetState(HGE_SCREENBPP, 32);
	mHGE->System_SetState(HGE_WINDOWX, gApp.GetGlobalConfig()->GetInt32("WindowX", 0, "Windows"));
	mHGE->System_SetState(HGE_WINDOWY, gApp.GetGlobalConfig()->GetInt32("WindowY", 0, "Windows"));
	mHGE->System_SetState(HGE_WINDOWED, !fullscreen);
	mHGE->System_SetState(HGE_LOGFILE, "HgeLog.txt");
	mHGE->System_SetState(HGE_HIDEMOUSE, false);
	mHGE->System_SetState(HGE_USESOUND, false);
	mHGE->System_SetState(HGE_SHOWSPLASH, false);
	bool success = mHGE->System_Initiate();

	assert(success);
	if (success)
		gLogMgr.LogMessage("HGE inited; logfile=", mHGE->System_GetState(HGE_LOGFILE));
}

GfxRenderer::~GfxRenderer()
{
	assert(mHGE);
	RECT windowRect;
	GetWindowRect(mHGE->System_GetState(HGE_HWND), &windowRect);
	gApp.GetGlobalConfig()->SetInt32("WindowX", windowRect.left, "Windows");
	gApp.GetGlobalConfig()->SetInt32("WindowY", windowRect.top, "Windows");
	mHGE->System_Shutdown();
}

uint64 GfxSystem::GfxRenderer::_GetWindowHandle()
{
	HWND hWnd = mHGE->System_GetState(HGE_HWND);
	assert(hWnd);
	return (uint64)(hWnd);
}

void GfxRenderer::ChangeResolution( const Point& resolution )
{
	std::set<IScreenResolutionChangeListener*>::iterator iter = mResChangeListeners.begin();

	gLogMgr.LogMessage("Changing resolution to ", resolution.x, resolution.y);

	mHGE->System_SetState(HGE_SCREENWIDTH,resolution.x);
	mHGE->System_SetState(HGE_SCREENHEIGHT,resolution.y);

	gInputMgr._SetResolution(resolution.x, resolution.y);

	while (iter != mResChangeListeners.end()) {
		(*iter)->EventResolutionChanged(resolution.x, resolution.y);
		++iter;
	}

	gLogMgr.LogMessage("Resolution changed");
}

Point GfxRenderer::GetResolution() const
{
	return Point(mHGE->System_GetState(HGE_SCREENWIDTH),mHGE->System_GetState(HGE_SCREENHEIGHT));
}

void GfxRenderer::SetFullscreen(bool fullscreen)
{
	gLogMgr.LogMessage("Changing fullscreen/dindowed" + fullscreen);
	mHGE->System_SetState(HGE_WINDOWED,fullscreen);	
	gLogMgr.LogMessage("Fullscreen changed");
}

bool GfxRenderer::BeginRendering(void) 
{
	return mHGE->Gfx_BeginScene();
}

bool GfxRenderer::EndRendering(void) 
{
	mHGE->Gfx_EndScene();
	return true;
}

// hge DWORD is unsigned long
DWORD GetHGEColor(const Color& color)
{
	return ((DWORD(color.a)<<24) + (DWORD(color.r)<<16) + (DWORD(color.g)<<8) + DWORD(color.b));
}

bool GfxRenderer::ClearScreen(const Color& color)
{
	mHGE->Gfx_Clear(GetHGEColor(color));
	return true;
}

bool GfxRenderer::DrawLine(int x1, int y1, int x2, int y2, const Pen& pen) 
{	
	mHGE->Gfx_RenderLine((float32) x1,(float32) y1,(float32) x2,(float32) y2,GetHGEColor(pen.color));
	return true;
}

bool GfxRenderer::DrawLine( const Point& begin, const Point& end, const Pen& pen )
{
	mHGE->Gfx_RenderLine((float32) begin.x,(float32) begin.y,(float32) end.x,(float32) end.y,GetHGEColor(pen.color));
	return true;
}

void InitQuad(hgeQuad& q,uint64 hTex, int32 x, int32 y,int32 w,int32 h)
{
	// set size
	q.v[0].x = (float32)(x);
	q.v[0].y = (float32)(y);
	q.v[0].z = 0;
	q.v[0].tx = 0.0f;
	q.v[0].ty = 0.0f;
	q.v[0].col = 0xFFFFFFFF;

	q.v[1].x = (float32)(x+w);
	q.v[1].y = (float32)(y);
	q.v[1].z = 0;
	q.v[1].tx = 1.0f;
	q.v[1].ty = 0.0f;
	q.v[1].col = 0xFFFFFFFF;

	q.v[2].x = (float32)(x+w);
	q.v[2].y = (float32)(y+h);
	q.v[2].z = 0;
	q.v[2].tx = 1.0f;
	q.v[2].ty = 1.0f;
	q.v[2].col = 0xFFFFFFFF;

	q.v[3].x = (float32)(x);
	q.v[3].y = (float32)(y+h);
	q.v[3].z = 0;
	q.v[3].tx = 0.0f;
	q.v[3].ty = 1.0f;
	q.v[3].col = 0xFFFFFFFF;

	// set texture
	q.tex = hTex;
	q.blend = BLEND_ALPHAADD | BLEND_COLORMUL | BLEND_ZWRITE;
}

bool GfxSystem::GfxRenderer::DrawImage( const TexturePtr& image, int32 x, int32 y, uint8 anchor /*= ANCHOR_VCENTER|ANCHOR_HCENTER*/, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/ )
{	
	if (image.IsNull())
	{
		gLogMgr.LogMessage("DrawImage: texture is null", LOG_ERROR);
		return false;
	}
	hgeQuad q;
	InitQuad(q,image->GetTexture(),x,y,image->GetWidth(),image->GetHeight());

	mHGE->Gfx_RenderQuad(&q);
	return true;
}

bool GfxSystem::GfxRenderer::DrawImage( const TexturePtr& image, const Point& pos, uint8 anchor /*= ANCHOR_VCENTER|ANCHOR_HCENTER*/, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/ )
{
	if (image.IsNull())
	{
		gLogMgr.LogMessage("DrawImage: texture is null", LOG_ERROR);
		return false;
	}
	hgeQuad q;
	InitQuad(q,image->GetTexture(),pos.x,pos.y,image->GetWidth(),image->GetHeight());

	mHGE->Gfx_RenderQuad(&q);
	return true;
}

bool GfxSystem::GfxRenderer::DrawImage( const TexturePtr& image, const Rect& srcRect, const Rect& destRect, uint8 alpha /*= 255*/ )
{
	if (image.IsNull())
	{
		gLogMgr.LogMessage("DrawImage: texture is null", LOG_ERROR);
		return false;
	}
	hgeQuad q;
	InitQuad(q,image->GetTexture(),destRect.x,destRect.y,destRect.w,destRect.h);

	mHGE->Gfx_RenderQuad(&q);
	return true;
}

bool GfxSystem::GfxRenderer::DrawPolygon( Point* vertices, int vertices_len, const TexturePtr& texture, const Pen& outline, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/, float32 textureAngle /*= 0.0f*/, float32 textureScale /*= 1.0f*/ )
{
	//TODO pokud je textura IsNull, tak zapsat neco do logu, ale vykreslit aspon outlinu
	return false;
}

bool GfxSystem::GfxRenderer::DrawPolygon( const std::vector<Point>& vertices, const TexturePtr& texture, const Pen& outline, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/, float32 textureAngle /*= 0.0f*/, float32 textureScale /*= 1.0f*/ )
{
	return false;
}

bool GfxSystem::GfxRenderer::DrawPolygon( Point* vertices, int vertices_len, const Color& fillColor, const Pen& outline, float32 angle /*= 0.0f*/, float32 scale /*= 1.0f*/, float32 textureAngle /*= 0.0f*/, float32 textureScale /*= 1.0f*/ )
{
	return false;
}

bool GfxSystem::GfxRenderer::DrawPolygon( const std::vector<Point>& vertices, const Color& fillColor, const Pen& outline, float32 angle /*= 0.0f*/, float32 scale /*= 1.0f*/, float32 textureAngle /*= 0.0f*/, float32 textureScale /*= 1.0f*/ )
{
	
	return false;
}

