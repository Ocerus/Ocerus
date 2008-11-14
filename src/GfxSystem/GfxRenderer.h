#ifndef _GFXRENDERER_H_
#define _GFXRENDERER_H_

#include <vector>
#include <set>
#include "../Utility/Settings.h"
#include "../Utility/Singleton.h"
#include "Texture.h"

#define gGfxRenderer GfxSystem::GfxRenderer::GetSingleton()

class HGE;

namespace GfxSystem
{
	struct Point
	{
		Point(int32 _x, int32 _y): x(_x), y(_y) {}
		int32 x, y;
	};

	struct Rect
	{
		Rect(int32 _x, int32 _y, int32 _w, int32 _h): x(_x), y(_y), w(_w), h(_h) {}
		int32 x, y, w, h;
		
		/// default constructor for Rect:
		Rect(): x(0), y(y), w(w), h(h) {}
		static Rect NullRect;
	};

	struct Color
	{
		Color(uint8 _r, uint8 _g, uint8 _b, uint8 _a = 255): r(_r), g(_g), b(_b), a(_a) {}
		uint8 r, g, b, a;
	};

	struct Pen
	{
		Pen(const Color& c): color(c) {}
		Color color;
		//TODO add style

		static Pen NullPen;
	};

	enum eAnchor { ANCHOR_VCENTER=1<<0, ANCHOR_TOP=1<<1, ANCHOR_BOTTOM=1<<2, ANCHOR_LEFT=1<<3, ANCHOR_RIGHT=1<<4, ANCHOR_HCENTER=1<<5 };

	// kvi kvi kviiiiii chro chro chro

	class IScreenResolutionChangeListener {
	public:
		virtual void EventResolutionChanged(int x, int y) = 0;
	};

	class GfxRenderer : public Singleton<GfxRenderer>
	{
	private:
		HGE* mHGE; 

		friend class Texture;

	protected:
		std::set<IScreenResolutionChangeListener*> mResChangeListeners;

		virtual void ClearResolutionChangeListeners();

	public:
		GfxRenderer(const Point& resolution, bool fullscreen);
		~GfxRenderer(void);

		void ChangeResolution(const Point& resolution);
		Point GetResolution(void) const;
		void SetFullscreen(bool fullscreen);

		inline virtual void AddResolutionChangeListener(IScreenResolutionChangeListener * listener) {
			mResChangeListeners.insert(listener);
		}
		inline virtual void RemoveResolutionChangeListener(IScreenResolutionChangeListener * listener) {
			mResChangeListeners.erase(listener);
		}

		bool BeginRendering(void);
		bool EndRendering(void);

		bool ClearScreen(const Color& color);

		// note that anchor determines the rotation/scaling pivot
		bool DrawImage(const TexturePtr& image, int32 x, int32 y, uint8 anchor = ANCHOR_VCENTER|ANCHOR_HCENTER, float32 angle = 0.0f, uint8 alpha = 255, float32 scale = 1.0f, int32 width = 0, int32 height = 0, const Rect& textureRect = Rect::NullRect);
		bool DrawImage(const TexturePtr& image, const Point& pos, uint8 anchor = ANCHOR_VCENTER|ANCHOR_HCENTER, float32 angle = 0.0f, uint8 alpha = 255, float32 scale = 1.0f);
		// todoooo Santhos: add functionality to srcRect, needed for CEGUI, as well as other stuff we might have to do
		bool DrawImage(const TexturePtr& image, const Rect& textureRect, const Rect& destRect, uint8 alpha = 255);

		bool DrawLine(int x1, int y1, int x2, int y2, const Pen& pen);
		bool DrawLine(const Point& begin, const Point& end, const Pen& pen);

		bool DrawPolygon(Point* vertices, int vertices_len, const TexturePtr& image, const Pen& outline, float32 angle = 0.0f, uint8 alpha = 255, float32 scale = 1.0f, float32 textureAngle = 0.0f, float32 textureScale = 1.0f);
		bool DrawPolygon(const std::vector<Point>& vertices, const TexturePtr& image, const Pen& outline, float32 angle = 0.0f, uint8 alpha = 255, float32 scale = 1.0f, float32 textureAngle = 0.0f, float32 textureScale = 1.0f);

		bool DrawPolygon(Point* vertices, int vertices_len, const Color& fillColor/*, const Pen& outline*/);
		bool DrawPolygon(const std::vector<Point>& vertices, const Color& fillColor/*, const Pen& outline*/);

		//TODO add font param
		//bool DrawText(const string& str, font, int32 x, int32 y, uint8 anchor = ANCHOR_VCENTER|ANCHOR_HCENTER, float32 angle = 0.0f, uint8 alpha = 255);

		uint64 _GetWindowHandle();

	};
}

#endif