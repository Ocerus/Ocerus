#ifndef _GFXRENDERER_H_
#define _GFXRENDERER_H_

#include "../Common.h"
#include "../Utility/Singleton.h"

namespace GfxSystem
{
	struct Point
	{
		Point(int _x, int _y): x(_x), y(_y) {}
		int32 x, y;
	};

	struct Rect
	{
		Rect(int _x, int _y, int _w, int _h): x(_x), y(_y), w(_w), h(_h) {}
		int32 x, y, w, h;
	};

	struct Color
	{
		Color(int _r, int _g, int _b, int _a): r(_r), g(_g), b(_b), a(_a) {}
		int8 r, g, b, a;

	struct Pen
	{
		Pen(const Color& c): color(c) {}
		Color color;
		//TODO add style

		static Pen NULL(Color(0,0,0,0));
	};

	enum eAnchor { ANCHOR_VCENTER=1<<0, ANCHOR_TOP=1<<1, ANCHOR_BOTTOM=1<<2, ANCHOR_LEFT=1<<3, ANCHOR_RIGHT=1<<4, ANCHOR_HCENTER=1<<5 };

	class GfxRenderer : public Singleton<GfxRenderer>
	{
	public:
		GfxRenderer(const Point& resolution, bool fullscreen);
		~GfxRenderer(void);

		void SetResolution(const Point& resolution);
		Point& GetResolution(void) const;
		void SetFullscreen(bool fullscreen);

		void BeginRendering(void);
		void EndRendering(void);

		// note that anchor determines the rotation/scaling pivot
		bool DrawImage(const TexturePtr& image, int32 x, int32 y, float32 angle = 0.0f, eAnchor anchor = ANCHOR_VCENTER|ANCHOR_HCENTER, int8 alpha = 255, float32 scale = 1.0f);
		bool DrawImage(const TexturePtr& image, const Point& pos, float32 angle = 0.0f, eAnchor anchor = ANCHOR_VCENTER|ANCHOR_HCENTER, int8 alpha = 255, float32 scale = 1.0f);
		bool DrawImage(const TexturePtr& image, const Rect& destRect, int8 alpha = 255);

		bool DrawLine(int x1, int y1, int x2, int y2, const Pen& pen);
		bool DrawLine(const Point& begin, const Point& end, const Pen& pen);

		bool DrawPolygon(Point* vertices, int vertices_len, const TexturePtr& texture, const Pen& outline, float32 angle = 0.0f, int8 alpha = 255, float32 scale = 1.0f, float32 textureAngle = 0.0f, float32 textureScale = 1.0f);
		bool DrawPolygon(const vector<Point>& vertices, const TexturePtr& texture, const Pen& outline, float32 angle = 0.0f, int8 alpha = 255, float32 scale = 1.0f, float32 textureAngle = 0.0f, float32 textureScale = 1.0f);

		bool DrawPolygon(Point* vertices, int vertices_len, const Color& fillColor, const Pen& outline, float32 angle = 0.0f, float32 scale = 1.0f, float32 textureAngle = 0.0f, float32 textureScale = 1.0f);
		bool DrawPolygon(const vector<Point>& vertices, const Color& fillColor, const Pen& outline, float32 angle = 0.0f, float32 scale = 1.0f, float32 textureAngle = 0.0f, float32 textureScale = 1.0f);


	};
}

#endif