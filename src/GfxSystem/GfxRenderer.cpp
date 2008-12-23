#include "Common.h"
#include "GfxRenderer.h"
#include <hge.h>
#include "Triangulate.h"
#include "Texture.h"
#include "IScreenListener.h"

namespace GfxSystem
{

	inline DWORD ConvertColorToDWORD(const Color& color) {
		return ARGB(color.a, color.r, color.g, color.b);
	}

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
		Point pos;
		if (!gGfxRenderer.IsFullscreen() && gGfxRenderer.GetWindowPosition(pos))
		{
			gApp.GetGlobalConfig()->SetInt32("WindowX", pos.x, "Windows");
			gApp.GetGlobalConfig()->SetInt32("WindowY", pos.y, "Windows");
		}
		return true;
	}
}

using namespace GfxSystem;

#define DEFAULT_IMAGE_WORLD_SCALE 0.02f

// init null values
Pen Pen::NullPen(Color(0,0,0,0));
Rect Rect::NullRect(0,0,0,0);
Color Color::NullColor(0,0,0,0);
Color Color::FullColor(255,255,255,255);

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
	mHGE->System_SetState(HGE_HIDEMOUSE, true);
	mHGE->System_SetState(HGE_USESOUND, false);
	mHGE->System_SetState(HGE_SHOWSPLASH, false);
	bool success = mHGE->System_Initiate();

	assert(success);
	if (success)
		gLogMgr.LogMessage("HGE inited; logfile=", mHGE->System_GetState(HGE_LOGFILE));

	mCameraScaleInv = 1.0f / mCameraScale;
}

GfxRenderer::~GfxRenderer()
{
	assert(mHGE);
	HgeExitFunction();
	mHGE->System_Shutdown();
}

uint32 GfxRenderer::_GetWindowHandle() const
{
	HWND hWnd = mHGE->System_GetState(HGE_HWND);
	assert(hWnd);
	return (uint32)hWnd;
}

void GfxRenderer::ChangeResolution( const uint32 width, const uint32 height )
{
	assert(mHGE);

	std::set<IScreenListener*>::iterator iter = mScreenListeners.begin();

	gLogMgr.LogMessage("Changing resolution to ", width, height);

	mHGE->System_ChangeResolution(width, height);

	/*mHGE->System_SetState(HGE_SCREENWIDTH,width);
	mHGE->System_SetState(HGE_SCREENHEIGHT,height);*/

	while (iter != mScreenListeners.end())
	{
		(*iter)->ResolutionChanged(width, height);
		++iter;
	}

	gLogMgr.LogMessage("Resolution changed");
}

Point GfxRenderer::GetResolution() const
{
	return Point(GetScreenWidth(), GetScreenHeight());
}

int32 GfxRenderer::GetScreenWidth( void ) const
{
	return mHGE->System_GetState(HGE_SCREENWIDTH);
}

int32 GfxRenderer::GetScreenHeight( void ) const
{
	return mHGE->System_GetState(HGE_SCREENHEIGHT);
}

void GfxRenderer::SetFullscreen(bool fullscreen)
{
	gLogMgr.LogMessage("Changing fullscreen/windowed", fullscreen);
	mHGE->System_SetState(HGE_WINDOWED, !fullscreen);	
	gLogMgr.LogMessage("Fullscreen changed to", IsFullscreen());
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

void InitQuad(hgeQuad& q,const TexturePtr& image, const Rect& textureRect, const ColorRect& colors )
{
	/* init texture region */ 
	// texture width and height
	float32 tw = (float32) (image->GetWidth());
	float32 th = (float32) (image->GetHeight());

	// check whether user wants the whole texture or its section
	Rect tr(0,0,image->GetWidth(),image->GetHeight());
	if (&textureRect != &Rect::NullRect)		
		tr = textureRect;

	/* init vertices - set texture position, position, colour (alpha only) */
	// z position is 0, because we have 2d world
	q.v[0].x = 0;
	q.v[0].y = 0;
	q.v[0].z = 0;
	q.v[0].tx = tr.x / tw;
	q.v[0].ty = tr.y / th;
	q.v[0].col = ConvertColorToDWORD(colors.TopLeft);

	q.v[1].x = tw;
	q.v[1].y = 0;
	q.v[1].z = 0;
	q.v[1].tx = (tr.x + tr.w) / tw;
	q.v[1].ty = tr.y / th;
	q.v[1].col = ConvertColorToDWORD(colors.TopRight);

	q.v[2].x = tw;
	q.v[2].y = th;
	q.v[2].z = 0;
	q.v[2].tx = (tr.x + tr.w) / tw;
	q.v[2].ty = (tr.y + tr.h) / th;
	q.v[2].col = ConvertColorToDWORD(colors.BottomRight);

	q.v[3].x = 0;
	q.v[3].y = th;
	q.v[3].z = 0;
	q.v[3].tx = tr.x / tw;
	q.v[3].ty = (tr.y + tr.h) / th;
	q.v[3].col = ConvertColorToDWORD(colors.BottomLeft);

	// set quad's texture
	q.tex = image->GetTexture();
	q.blend = BLEND_ALPHABLEND | BLEND_COLORMUL | BLEND_ZWRITE;
}

bool GfxRenderer::DrawImage( const TexturePtr& image, int32 x, int32 y, uint8 anchor /*= ANCHOR_VCENTER|ANCHOR_HCENTER*/, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/, int32 width /* = 0 */,int32 height /* = 0 */, const Rect& textureRect /* = 0 */) const
{	
	if (image.IsNull())
	{
		gLogMgr.LogMessage("DrawImage: texture is null", LOG_ERROR);
		return false;
	}

	Color alphacolor = Color(255, 255, 255, alpha);
	ColorRect alphas(alphacolor, alphacolor, alphacolor, alphacolor);
	hgeQuad q;
	uint32 imgW = image->GetWidth();
	uint32 imgH = image->GetHeight();
	InitQuad(q, image, Rect(0, 0, imgW, imgH), alphas);

	// scaling
	imgW = MathUtils::Round(scale*imgW);
	imgH = MathUtils::Round(scale*imgH);
	float32 imgW_half = 0.5f*imgW;
	float32 imgH_half = 0.5f*imgH;

	// basic positioning of the quad
	float32 qx = -imgW_half;
	float32 qy = -imgH_half;

	// anchoring
	assert(!(anchor&ANCHOR_LEFT)||!(anchor&ANCHOR_RIGHT)||!(anchor&ANCHOR_HCENTER) && "Coliding anchors");
	assert(!(anchor&ANCHOR_TOP)||!(anchor&ANCHOR_BOTTOM)||!(anchor&ANCHOR_VCENTER) && "Coliding anchors");
	if (anchor & ANCHOR_LEFT)
		qx += imgW_half;
	else if (anchor & ANCHOR_RIGHT)
		qx -= imgW_half;
	else { assert((anchor&ANCHOR_HCENTER) && "Wrong anchor"); }
	if (anchor & ANCHOR_TOP)
		qy += imgH_half;
	else if (anchor & ANCHOR_BOTTOM)
		qy -= imgH_half;
	else { assert((anchor&ANCHOR_VCENTER) && "Wrong anchor"); }

	// position the quad
	q.v[0].x = qx;
	q.v[0].y = qy;
	q.v[1].x = qx + imgW;
	q.v[1].y = qy;
	q.v[2].x = qx + imgW;
	q.v[2].y = qy + imgH;
	q.v[3].x = qx;
	q.v[3].y = qy + imgH;

	// rotate the quad around anchor
	Matrix22 rot(angle);
	for (int i=0; i<4; ++i)
	{
		Vector2 res = MathUtils::Multiply(rot, Vector2(q.v[i].x, q.v[i].y));
		q.v[i].x = res.x;
		q.v[i].y = res.y;
	}

	// move the quad to the correct position
	for (int i=0; i<4; ++i)
	{
		q.v[i].x += x;
		q.v[i].y += y;
	}

	mHGE->Gfx_RenderQuad(&q);
	return true;
}

bool GfxRenderer::DrawImage( const TexturePtr& image, const Point& pos, uint8 anchor /*= ANCHOR_VCENTER|ANCHOR_HCENTER*/, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/ ) const
{
	return DrawImage(image, pos.x, pos.y, anchor, angle, alpha, scale, image->GetWidth(), image->GetHeight());
}

bool GfxRenderer::DrawImageWithConversion( const TexturePtr& image, const Vector2& pos, uint8 anchor /*= ANCHOR_VCENTER|ANCHOR_HCENTER*/, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/ ) const
{
	return DrawImage(image, WorldToScreenX(pos.x), WorldToScreenY(pos.y), anchor, angle, alpha, WorldToScreenImageScale(scale), image->GetWidth(), image->GetHeight());
}

bool GfxRenderer::DrawImage( const TexturePtr& image, const Rect& destRect ) const
{
	return DrawImage(image, Rect(0,0,image->GetWidth(),image->GetHeight()), destRect);
}

bool GfxRenderer::DrawImage( const TexturePtr& image, const Rect& textureRect, const Rect& destRect, const ColorRect& colors ) const
{
	if (image.IsNull())
	{
		gLogMgr.LogMessage("DrawImage: texture is null", LOG_ERROR);
		return false;
	}
	hgeQuad q;		
	InitQuad(q, image, textureRect, colors);

	q.v[0].x = (float32)(destRect.x);
	q.v[0].y = (float32)(destRect.y);
	q.v[1].x = (float32)(destRect.x + destRect.w);
	q.v[1].y = (float32)(destRect.y);
	q.v[2].x = (float32)(destRect.x + destRect.w);
	q.v[2].y = (float32)(destRect.y + destRect.h);
	q.v[3].x = (float32)(destRect.x);
	q.v[3].y = (float32)(destRect.y + destRect.h);

	mHGE->Gfx_RenderQuad(&q);
	return true;
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
			triangles.push_back(Point(MathUtils::Round(i->GetX()),MathUtils::Round(i->GetY())));
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

bool GfxRenderer::DrawPolygon( Point* vertices, int vertices_len, const TexturePtr& image, const Pen& outline, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/, float32 textureAngle /*= 0.0f*/, float32 textureScale /*= 1.0f*/ ) const
{
	return false;
}

bool GfxRenderer::DrawPolygon( const std::vector<Point>& vertices, const TexturePtr& image, const Pen& outline, float32 angle /*= 0.0f*/, uint8 alpha /*= 255*/, float32 scale /*= 1.0f*/, float32 textureAngle /*= 0.0f*/, float32 textureScale /*= 1.0f*/ ) const
{
	if (image.IsNull())
	{
		gLogMgr.LogMessage("DrawPolygon: texture is null", LOG_ERROR);
		return false;
	}

	return false;
}

bool GfxRenderer::DrawPolygon( Point* vertices, int vertices_len, const Color& fillColor, const Pen& outline /* = Pen::NullPen */) const
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

bool GfxRenderer::DrawPolygonWithConversion( const Vector2* vertices, int vertices_len, const Vector2& offsetPosition, const float32 offsetAngle, const Color& fillColor, const Pen& outline) const
{	
	// init vector
	std::vector<Point> v;
	XForm xf(offsetPosition, Matrix22(offsetAngle));
	for(int i = 0;i < vertices_len;i++)
		v.push_back(WorldToScreen(MathUtils::Multiply(xf, vertices[i])));

	// draw polygon using the method beneath this
	if (DrawPolygon(v,fillColor, outline))
		return true;
	else
		return false;
}

bool GfxRenderer::DrawPolygon( const std::vector<Point>& vertices, const Color& fillColor, const Pen& outline) const
{	
	//TODO tahle funkce musi bejt schopna neco vykreslit i kdyz je polygon degenerovanej a nejde triangulovat
	//TODO prozkoumat, jestli nejde pres DirectX kreslit polygon lip a pripadne implementovat prislusnou funkci dovnitr HGE

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

bool GfxRenderer::DrawCircle( const Point& center, const int32 radius, const Color& fillColor, const Pen& outline, const float32 minAng, const float32 maxAng ) const
{
	//TODO tohle je hrozne pomaly
	float32 minAngle = MathUtils::WrapAngle(minAng);
	float32 maxAngle = MathUtils::WrapAngle(maxAng);
	if (maxAngle < minAngle)
		maxAngle += MathUtils::TWO_PI;
	const float32 numSegments = 16;
	const float32 eachAngle = MathUtils::TWO_PI / numSegments;
	int32 x, y, oldX=-1, oldY=-1;

	bool fill = &fillColor != &Color::NullColor;
	std::vector<Point> points;
	if (fill)
		points.push_back(center);	

	bool exit = false;
	for(float32 a=minAngle; !exit; a+=eachAngle)
	{ 
		if (a >= maxAngle)
		{
			a = maxAngle;
			exit = true;
		}
		x = MathUtils::Round(radius * MathUtils::Cos(a)); 
		y = MathUtils::Round(radius * MathUtils::Sin(a)); 
		if (fill && (x != oldX || y != oldY))
			points.push_back(Point(x + center.x, y + center.y));
		if (a != minAngle)
			DrawLine(oldX + center.x, oldY + center.y, x + center.x, y + center.y, outline);
		oldX = x;
		oldY = y;
	} 

	if (fill)
		DrawPolygon(points, fillColor);

	return true;
}

int32 GfxRenderer::WorldToScreenX( const float32 x ) const
{
	return MathUtils::Round((x-mCameraX)*mCameraScale) + GetScreenWidthHalf();
}

int32 GfxRenderer::WorldToScreenY( const float32 y ) const
{
	return MathUtils::Round((y-mCameraY)*mCameraScale) + GetScreenHeightHalf();
}

Point GfxRenderer::WorldToScreen( const Vector2& pos ) const
{
	return Point(WorldToScreenX(pos.x), WorldToScreenY(pos.y));
}

float32 GfxRenderer::WorldToScreenImageScale( const float32 scale ) const
{
	return DEFAULT_IMAGE_WORLD_SCALE * scale * mCameraScale;
}

float32 GfxRenderer::ScreenToWorldX( const int32 x ) const
{
	return mCameraScaleInv*(x-GetScreenWidthHalf()) + mCameraX;
}

float32 GfxRenderer::ScreenToWorldY( const int32 y ) const
{
	return mCameraScaleInv*(y-GetScreenHeightHalf()) + mCameraY;
}

Vector2 GfxRenderer::ScreenToWorld( const Point& pos ) const
{
	return Vector2(ScreenToWorldX(pos.x), ScreenToWorldY(pos.y));
}

void GfxRenderer::ZoomCamera( const float32 delta )
{
	// zooms around the screen center
	SetCameraScale(mCameraScale + delta);
}

void GfxRenderer::MoveCamera( const float32 dx, const float32 dy )
{
	mCameraX += dx;
	mCameraY += dy;
}

Vector2 GfxRenderer::GetCameraWorldBoxTopLeft( void ) const
{
	return Vector2(ScreenToWorldX(0),ScreenToWorldY(0));
}

Vector2 GfxRenderer::GetCameraWorldBoxBotRight( void ) const
{
	return Vector2(ScreenToWorldX(GetScreenWidth()),ScreenToWorldY(GetScreenHeight()));
}

bool GfxRenderer::IsFullscreen( void ) const
{
	assert(mHGE);
	return !mHGE->System_GetState(HGE_WINDOWED);
}


//------------------------------------------------------------------------
// Haxxor functions follow!

#include <Windows.h>

bool GfxRenderer::GetWindowPosition( Point& out )
{
	RECT windowRect;
	bool result = GetWindowRect(gGfxRenderer.mHGE->System_GetState(HGE_HWND), &windowRect) != 0;
	if (result)
	{
		out.x = windowRect.left;
		out.y = windowRect.top;
	}
	return result;
}
