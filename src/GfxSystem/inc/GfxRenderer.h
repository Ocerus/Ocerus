// Graphic renderer
#ifndef GFXRENDERER_H
#define GFXRENDERER_H

#include <../utility/inc/settings.h>

typedef vector<Vertex> vPositon;

namespace GfxSystem 
{

	struct Vertex 
	{
		float32 x,y,z;		
	}

	struct Triple {
		Vertex vertices[3];
		Pen* outline;
		Texture* tex;
	}

	struct Quad {
		Vertex vertices[4];
		Pen* outline;
		Texture* tex;
	}

	class Texture;
	class Pen;

	class GfxRenderer 
	{
		virtual ~GfxRenderer();
	 
		virtual bool DrawPolygon(int vertexCnt, Vertex* vertices, Pen* outline, Texture* tex = 0) = 0;
		virtual bool DrawPolygon(vVertex* vertices, Pen* outline, Texture* tex = 0) = 0;
		virtual bool DrawElipse(Vertex* vertices, Pen* outline, Texture* tex = 0);
		virtual bool DrawElipse(vVertex* vertices, Pen* outline, Texture* tex = 0);
		virtual bool DrawLine(Vertex* pos,Pen* outline);		
		virtual bool DrawCircle(Vertex* pos, float32 rad, Pen* outline);
		virtual bool DrawTriple(const Triple& tri);
		virtual bool DrawTriple(Vertex* vertices, Pen* outline, Texture* tex = 0);
		virtual bool DrawTriple(vVertex* vertices, Pen* outline, Texture* tex = 0);
		virtual bool DrawQuad(const Quad& qd);
		virtual bool DrawQuad(Vertex* vertices, Pen* outline, Texture* tex = 0);
		virtual bool DrawQuad(vVertex* vertices, Pen* outline, Texture* tex = 0);
		
		// double buffering
		virtual bool flipBuffers();		
	}
}

#endif