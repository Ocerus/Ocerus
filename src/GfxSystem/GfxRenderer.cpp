#include "Common.h"
#include "GfxRenderer.h"
#include <hge.h>
#include "Triangulate.h"
#include "Texture.h"
#include "IScreenResolutionChangeListener.h"

using namespace GfxSystem;

// init null values
Pen Pen::NullPen(Color(0,0,0,0));
Rect Rect::NullRect(0,0,0,0);

namespace GfxSystem
{
	/** This function is called whenever the HGE was notified that the application requested the shutdown.
		Returning false will prevent the application from being shut down. It's advices to return true.
	*/
	bool HgeExitFunction(void)
	{
		static bool alreadyDone = false;
		if (alreadyDone)
			return true;
		alreadyDone = true;

		// we need to save the current position of the window so that we can restore it on next startup
		RECT windowRect;
		if (GetWindowRect(gGfxRenderer.mHGE->System_GetState(HGE_HWND), &windowRect))
		{
			gApp.GetGlobalConfig()->SetInt32("WindowX", windowRect.left, "Windows");
			gApp.GetGlobalConfig()->SetInt32("WindowY", windowRect.top, "Windows");
		}
		return true;
	}
}

GfxRenderer::GfxRenderer(const Point& resolution, bool fullscreen):
	mCameraX(0),
	mCameraY(0),
	mCameraScale(1.0f),
	mHGE(0)
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
	mHGE->System_SetState(HGE_EXITFUNC, (hgeCallback)(&HgeExitFunction));
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
	ClearResolutionChangeListeners();
	assert(mHGE);
	HgeExitFunction();
	mHGE->System_Shutdown();
}

void GfxRenderer::ClearResolutionChangeListeners() {
	mResChangeListeners.clear();
}

uint32 GfxSystem::GfxRenderer::_GetWindowHandle() const
{
	HWND hWnd = mHGE->System_GetState(HGE_HWND);
	assert(hWnd);
	return (uint32)hWnd;
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
	gLogMgr.LogMessage("Changing fullscreen/windowed" + fullscreen);
	mHGE->System_SetState(HGE_WINDOWED,fullscreen);	
	gLogMgr.LogMessage("Fullscreen changed");
}

bool GfxRenderer::BeginRendering(void) const
{
	return mHGE->Gfx_BeginScene();
}

bool GfxRenderer::EndRendering(void) const
{
	mHGE->Gfx_EndScene();
	return true;
}

// hge DWORD is unsigned long
DWORD GetHGEColor(const Color& color)
{
	return ((DWORD(color.a)<<24) + (DWORD(color.r)<<16) + (DWORD(color.g)<<8) + DWORD(color.b));
}

bool GfxRenderer::ClearScreen(const Color& color) const
{
	mHGE->Gfx_Clear(GetHGEColor(color));
	return true;
}

bool GfxRenderer::DrawLine(int x1, int y1, int x2, int y2, const Pen& pen) const
{	
	mHGE->Gfx_RenderLine((float32) x1,(float32) y1,(float32) x2,(float32) y2,GetHGEColor(pen.color));
	return true;
}

bool GfxRenderer::DrawLine( const Point& begin, const Point& end, const Pen& pen ) const
{
	mHGE->Gfx_RenderLine((float32) begin.x,(float32) begin.y,(float32) end.x,(float32) end.y,GetHGEColor(pen.color));
	return true;
}

bool GfxRenderer::DrawLineWithConversion( const Vector2& begin, const Vector2& end, const Pen& pen ) const
{
	return DrawLine(WorldToScreen(begin), WorldToScreen(end), pen);
}

void InitQuad(hgeQuad& q,const TexturePtr& image, int32 x, int32 y,int32 w,int32 h,uint8 alpha,const Rect& textureRect,uint8 anchor)
{
	// set default of weight and height
	if (w == 0)
		w = image->GetWidth();
	if (h == 0)
		h = image->GetHeight();

	// set alpha
	uint32 col = 0xFFFFFFFF + (alpha << 24);
		
	/* init texture region */ 
	// texture width and height
	float32 tw = (float32) (image->GetWidth());
	float32 th = (float32) (image->GetHeight());

	// check whether user wants the whole texture or its section
	Rect tr(0,0,image->GetWidth(),image->GetHeight());
	if (&textureRect != &Rect::NullRect)		
		tr = textureRect;

	/* set float position of top left corner minding the anchor */
	float32 fX = (float32)(x);
	float32 fY = (float32)(y);

	if (ANCHOR_VCENTER & anchor)
		fY -= (float32)(h)/2;
	// anchor top is default value
	if (ANCHOR_BOTTOM & anchor)
		fY -= (float32)(h);
	// anchor left is default value
	if (ANCHOR_RIGHT & anchor)
		fX -= (float32)(w);
	if (ANCHOR_HCENTER & anchor)
		fX -= (float32)(w)/2;

	/* init vertices - set texture position, position, colour (alpha only) */
	// z position is 0, because we have 2d world
	q.v[0].x = fX;
	q.v[0].y = fY;
	q.v[0].z = 0;
	q.v[0].tx = tr.x / tw;
	q.v[0].ty = tr.y / th;
	q.v[0].col = col;

	q.v[1].x = fX + w;
	q.v[1].y = fY;
	q.v[1].z = 0;
	q.v[1].tx = (tr.x + tr.w) / tw;
	q.v[1].ty = tr.y / th;
	q.v[1].col = col;

	q.v[2].x = fX + w;
	q.v[2].y = fY + h;
	q.v[2].z = 0;
	q.v[2].tx = (tr.x + tr.w) / tw;
	q.v[2].ty = (tr.y + tr.h) / th;
	q.v[2].col = col;

	q.v[3].x = fX;
	q.v[3].y = fY + h;
	q.v[3].z = 0;
	q.v[3].tx = tr.x / tw;
	q.v[3].ty = (tr.y + tr.h) / th;
	q.v[3].col = col;

	// set quad's texture
	q.tex = image->GetTexture();
	q.blend = BLEND_ALPHABLEND | BLEND_COLORMUL | BLEND_ZWRITE;
}

bool GfxSystem::GfxRenderer::DrawImage( const TexturePtr& image, int32 x, int32 y, uint8 anchor /*= ANCHOR_VCENTER|ANCHOR_HCENTER*/, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/, int32 width /* = 0 */,int32 height /* = 0 */, const Rect& textureRect /* = 0 */) const
{	
	if (image.IsNull())
	{
		gLogMgr.LogMessage("DrawImage: texture is null", LOG_ERROR);
		return false;
	}
	hgeQuad q;		
	InitQuad(q,image,x,y,width,height,alpha,textureRect,anchor);

	mHGE->Gfx_RenderQuad(&q);
	return true;
}

bool GfxSystem::GfxRenderer::DrawImage( const TexturePtr& image, const Point& pos, uint8 anchor /*= ANCHOR_VCENTER|ANCHOR_HCENTER*/, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/ ) const
{
	if (DrawImage(image,pos.x,pos.y,anchor,angle,alpha,scale,image->GetWidth(),image->GetHeight()))
		return true;
	else
		return false;
}

bool GfxSystem::GfxRenderer::DrawImageWithConversion( const TexturePtr& image, const Vector2& pos, uint8 anchor /*= ANCHOR_VCENTER|ANCHOR_HCENTER*/, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/ ) const
{
	if (DrawImage(image, WorldToScreenX(pos.x),WorldToScreenY(pos.y),anchor,angle,alpha,WorldToScreenS(scale),image->GetWidth(),image->GetHeight()))
		return true;
	else
		return false;
}

bool GfxSystem::GfxRenderer::DrawImage( const TexturePtr& image, const Rect& textureRect, const Rect& destRect, uint8 alpha /*= 255*/ ) const
{
	if (DrawImage(image,destRect.x,destRect.y,0,0,alpha,0,destRect.w,destRect.h,textureRect))
		return true;
	else
		return false;
}

bool createTriangles(const std::vector<Point>& vertices,std::vector<Point>& triangles)
{	
	Vector2dVector a;
	for(std::vector<Point>::const_iterator i = vertices.begin();i != vertices.end();++i)
	{
		a.push_back(Vector2d((float)(i->x),(float)(i->y)));
	}
	
	Vector2dVector result;
	if (Triangulate::Process(a,result))
		for(std::vector<Vector2d>::const_iterator i = result.begin();i != result.end();++i)
			triangles.push_back(Point(round(i->GetX()),round(i->GetY())));
	else
		return false;

	return true;
}

void InitTriple(hgeTriple& t,uint32 hTex,int32 x1,int32 y1,int32 x2,int32 y2,int32 x3,int32 y3,uint32 col = 0xFFFFFFFF)
{
	// set size
	t.v[0].x = (float32)(x1);
	t.v[0].y = (float32)(y1);
	t.v[0].z = 0;
	t.v[0].tx = 0.0f;
	t.v[0].ty = 0.0f;
	t.v[0].col = col;

	t.v[1].x = (float32)(x2);
	t.v[1].y = (float32)(y2);
	t.v[1].z = 0;
	t.v[1].tx = 1.0f;
	t.v[1].ty = 0.0f;
	t.v[1].col = col;

	t.v[2].x = (float32)(x3);
	t.v[2].y = (float32)(y3);
	t.v[2].z = 0;
	t.v[2].tx = 1.0f;
	t.v[2].ty = 1.0f;
	t.v[2].col = col;

	// set texture
	t.tex = hTex;
	t.blend = BLEND_ALPHABLEND | BLEND_COLORMUL | BLEND_ZWRITE;
}

bool GfxSystem::GfxRenderer::DrawPolygon( Point* vertices, int vertices_len, const TexturePtr& image, const Pen& outline, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/, float32 textureAngle /*= 0.0f*/, float32 textureScale /*= 1.0f*/ ) const
{
	return false;
}

bool GfxSystem::GfxRenderer::DrawPolygon( const std::vector<Point>& vertices, const TexturePtr& image, const Pen& outline, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/, float32 textureAngle /*= 0.0f*/, float32 textureScale /*= 1.0f*/ ) const
{
	if (image.IsNull())
	{
		gLogMgr.LogMessage("DrawPolygon: texture is null", LOG_ERROR);
		return false;
	}

	return false;
}

bool GfxSystem::GfxRenderer::DrawPolygon( Point* vertices, int vertices_len, const Color& fillColor, const Pen& outline /* = Pen::NullPen */) const
{	
	// init vector
	std::vector<Point> v;
	for(int i = 0;i < vertices_len;i++)
		v.push_back(vertices[i]);

	// draw polygon using the method beneath this
	if (DrawPolygon(v,fillColor,outline))
		return true;
	else
		return false;
}

bool GfxSystem::GfxRenderer::DrawPolygon( const std::vector<Point>& vertices, const Color& fillColor, const Pen& outline /* = Pen::NullPen */) const
{	
	std::vector<Point> triangles;
	if (createTriangles(vertices,triangles)) // get triangles from points
	{
		// draw filled triangles
		int32 pre2[2][2]; // to store two first points of triangle
		int x = 0;  // simple counter
		hgeTriple t; // triangle
		for(std::vector<Point>::iterator i = triangles.begin();i<triangles.end();++i)
		{
			int m = x % 3;
			if (m == 2) // current vertex is the third needed
			{ 
				// init triangle with the two stored vertices and the current vertex
				InitTriple(t,0,pre2[0][0],pre2[0][1],pre2[1][0],pre2[1][1],i->x,i->y,GetHGEColor(fillColor));
				mHGE->Gfx_RenderTriple(&t); // render it
			}
			else
			{			
				// init first and second triangle vertex
				pre2[m][0] = i->x;
				pre2[m][1] = i->y;
			}					
			++x; // update counter
		}
		
		//draw outline
		if(&outline != &Pen::NullPen)
		{
			for(std::vector<Point>::const_iterator i = vertices.begin();i<vertices.end() - 1;++i)
				DrawLine(*i, *(i + 1), outline);
			DrawLine(*(vertices.end() - 1),vertices[0],outline);
		}
	}
	else 
	{
		gLogMgr.LogMessage("DrawPolygon: Unable to triangulate the polygon", LOG_ERROR);
		return false;
	}
		
	return true;
}

bool GfxSystem::GfxRenderer::DrawPolygonWithConversion( Vector2* vertices, int vertices_len, const Color& fillColor, const Pen& outline /* = Pen::NullPen */) const
{	
	// init vector
	std::vector<Point> v;
	for(int i = 0;i < vertices_len;i++)
		v.push_back(WorldToScreen(vertices[i]));

	// draw polygon using the method beneath this
	if (DrawPolygon(v,fillColor,outline))
		return true;
	else
		return false;
}

int32 GfxSystem::GfxRenderer::WorldToScreenX( const float32 x ) const
{
	return (int32)((x-mCameraX)*mCameraScale);
}

int32 GfxSystem::GfxRenderer::WorldToScreenY( const float32 y ) const
{
	return (int32)((y-mCameraY)*mCameraScale);
}

GfxSystem::Point GfxSystem::GfxRenderer::WorldToScreen( const Vector2& pos ) const
{
	return GfxSystem::Point(WorldToScreenX(pos.x), WorldToScreenY(pos.y));
}