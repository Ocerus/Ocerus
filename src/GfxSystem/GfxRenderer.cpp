#include "GfxRenderer.h"
#include "../Common.h"

using namespace GfxSystem;

Pen Pen::NullPen(Color(0,0,0,0));

GfxRenderer::GfxRenderer(const Point& resolution, bool fullscreen) 
{
	g = hgeCreate(HGE_VERSION);
}

void GfxRenderer::BeginRendering(void) 
{
	assert(g->Gfx_beginScene());
}
void GfxRenderer::EndRendering(void) 
{
	assert(g->Gfx_endScene());
}

bool GfxRenderer::DrawLine(int x1, int y1, int x2, int y2, const Pen& pen) 
{
	// TODO colour 
	//g->Gfx_RenderLine(x1,y1,x2,y2
}

