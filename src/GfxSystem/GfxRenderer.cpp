#include "GfxRenderer.h"
#include "../Common.h"

namespace GfxSystem
{ 
	#include <hge.h>
}

using namespace GfxSystem;

Pen Pen::NullPen(Color(0,0,0,0));

GfxRenderer::GfxRenderer(const Point& resolution, bool fullscreen) 
{
	mLogMgr.LogMessage("*** GfxRenderer init ***");

	mHGE = hgeCreate(HGE_VERSION);
	mHGE->System_SetState(HGE_TITLE, "Battleships");
	mHGE->System_SetState(HGE_SCREENWIDTH, resolution.x);
	mHGE->System_SetState(HGE_SCREENWIDTH, resolution.x);
	mHGE->System_SetState(HGE_SCREENHEIGHT, resolution.y);
	mHGE->System_SetState(HGE_WINDOWED, !fullscreen);
	mHGE->System_SetState(HGE_LOGFILE, "HgeLog.txt");
	mHGE->System_SetState(HGE_HIDEMOUSE, false);
	mHGE->System_SetState(HGE_USESOUND, false);
	mHGE->System_SetState(HGE_SHOWSPLASH, false);
	bool success = mHGE->System_Initiate();

	assert(success);
	if (success)
		mLogMgr.LogMessage("HGE inited; logfile="+mHGE->System_GetState(HGE_LOGFILE));
}

GfxRenderer::~GfxRenderer()
{
	assert(mHGE);
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
	mHGE->System_SetState(HGE_SCREENWIDTH,resolution.x);
	mHGE->System_SetState(HGE_SCREENHEIGHT,resolution.y);

	gInputMgr._SetResolution(resolution.x, resolution.y);
}

Point GfxRenderer::GetResolution() const
{
	return Point(mHGE->System_GetState(HGE_SCREENWIDTH),mHGE->System_GetState(HGE_SCREENHEIGHT));
}

void GfxRenderer::SetFullscreen(bool fullscreen)
{
	mHGE->System_SetState(HGE_WINDOWED,fullscreen);	
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
	mHGE->Gfx_RenderLine((float) x1,(float) y1,(float) x2,(float) y2,GetHGEColor(pen.color));
	return true;
}

bool GfxRenderer::DrawLine( const Point& begin, const Point& end, const Pen& pen )
{
	mHGE->Gfx_RenderLine((float) begin.x,(float) begin.y,(float) end.x,(float) end.y,GetHGEColor(pen.color));
	return false;
}

bool GfxSystem::GfxRenderer::DrawImage( const TexturePtr& image, int32 x, int32 y, uint8 anchor /*= ANCHOR_VCENTER|ANCHOR_HCENTER*/, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/ )
{
	//TODO get texture size
	/*hgeQuad q;
	q.v[0].x = (float) x;
	q.v[0].y = (float) y;
	


	mHGE->Gfx_RenderQuad(&q);*/
	return false;
}

bool GfxSystem::GfxRenderer::DrawImage( const TexturePtr& image, const Point& pos, uint8 anchor /*= ANCHOR_VCENTER|ANCHOR_HCENTER*/, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/ )
{
	return false;
}

bool GfxSystem::GfxRenderer::DrawImage( const TexturePtr& image, const Rect& destRect, uint8 alpha /*= 255*/ )
{
	//TODO textures
	/*
	hgeQuad q;
	q.v[0].x = destRect.x;
	q.v[0].y = destRect.y;
	q.v[1].x = destRect.x + destRect.w;
	q.v[1].y = destRect.y;
	q.v[2].x = destRect.x + destRect.w;
	q.v[2].y = destRect.y + destRect.h;
	q.v[3].x = destRect.x;
	q.v[3].y = destRect.y + destRect.h;*/

	return false;
}

bool GfxSystem::GfxRenderer::DrawPolygon( Point* vertices, int vertices_len, const TexturePtr& texture, const Pen& outline, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/, float32 textureAngle /*= 0.0f*/, float32 textureScale /*= 1.0f*/ )
{
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

