#ifndef _GFXRENDERER_H_
#define _GFXRENDERER_H_

#include <vector>
#include <set>
#include "../Utility/Settings.h"
#include "../Utility/Singleton.h"
#include "Texture.h"

#define gGfxRenderer GfxSystem::GfxRenderer::GetSingleton()

/// Forward declarations
//@{
class HGE;
//@}

namespace GfxSystem
{

	/// Forward declarations.
	//@{
	class IScreenResolutionChangeListener;
	//@}

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

	class GfxRenderer : public Singleton<GfxRenderer>
	{
	public:
		GfxRenderer(const Point& resolution, bool fullscreen);
		~GfxRenderer(void);

		void ChangeResolution(const Point& resolution);
		Point GetResolution(void) const;
		void SetFullscreen(bool fullscreen);

		inline virtual void AddResolutionChangeListener(IScreenResolutionChangeListener * listener)
		{
			mResChangeListeners.insert(listener);
		}

		inline virtual void RemoveResolutionChangeListener(IScreenResolutionChangeListener * listener)
		{
			mResChangeListeners.erase(listener);
		}

 		bool BeginRendering(void) const;
		bool EndRendering(void) const;

		inline void SetCameraX(const float32 x) { mCameraX = x; }
		inline void SetCameraY(const float32 y) { mCameraY = y; }
		inline void SetCameraPos(const Vector2& pos) { mCameraX = pos.x; mCameraY = pos.y; }
		inline void SetCameraScale(const float32 s) { mCameraScale = s; }
		int32 WorldToScreenX(const float32 x) const;
		int32 WorldToScreenY(const float32 y) const;
		inline float32 WorldToScreenS(const float32 scale) const { return mCameraScale*scale; }
		Point WorldToScreen(const Vector2& pos) const;

		bool ClearScreen(const Color& color) const;

		// note that anchor determines the rotation/scaling pivot
		bool DrawImage(const TexturePtr& image, int32 x, int32 y, uint8 anchor = ANCHOR_VCENTER|ANCHOR_HCENTER, float32 angle = 0.0f, uint8 alpha = 255, float32 scale = 1.0f, int32 width = 0, int32 height = 0, const Rect& textureRect = Rect::NullRect) const;
		bool DrawImage(const TexturePtr& image, const Point& pos, uint8 anchor = ANCHOR_VCENTER|ANCHOR_HCENTER, float32 angle = 0.0f, uint8 alpha = 255, float32 scale = 1.0f) const;
		/// This version does a conversion from world space
		bool DrawImageWithConversion(const TexturePtr& image, const Vector2& pos, uint8 anchor = ANCHOR_VCENTER|ANCHOR_HCENTER, float32 angle = 0.0f, uint8 alpha = 255, float32 scale = 1.0f) const;
		/// This version is made specially to support GUI
		bool DrawImage(const TexturePtr& image, const Rect& textureRect, const Rect& destRect, uint8 alpha = 255) const;

		bool DrawLine(int x1, int y1, int x2, int y2, const Pen& pen) const;
		bool DrawLine(const Point& begin, const Point& end, const Pen& pen) const;
		/// This version does a conversion from world space
		bool DrawLineWithConversion(const Vector2& begin, const Vector2& end, const Pen& pen) const;

		bool DrawPolygon(Point* vertices, int vertices_len, const TexturePtr& image, const Pen& outline, float32 angle = 0.0f, uint8 alpha = 255, float32 scale = 1.0f, float32 textureAngle = 0.0f, float32 textureScale = 1.0f) const;
		bool DrawPolygon(const std::vector<Point>& vertices, const TexturePtr& image, const Pen& outline, float32 angle = 0.0f, uint8 alpha = 255, float32 scale = 1.0f, float32 textureAngle = 0.0f, float32 textureScale = 1.0f) const;
		
		bool DrawPolygon(Point* vertices, int vertices_len, const Color& fillColor, const Pen& outline = Pen::NullPen) const;
		bool DrawPolygon(const std::vector<Point>& vertices, const Color& fillColor, const Pen& outline = Pen::NullPen) const;

		/// This version does a conversion from world space
		//TODO dodelat podobne verze DrawPolygon
		bool DrawPolygonWithConversion(Vector2* vertices, int vertices_len, const Color& fillColor, const Pen& outline = Pen::NullPen = Pen::NullPen) const;

		//TODO add font param
		//bool DrawText(const string& str, font, int32 x, int32 y, uint8 anchor = ANCHOR_VCENTER|ANCHOR_HCENTER, float32 angle = 0.0f, uint8 alpha = 255);

		uint32 _GetWindowHandle(void) const;

	private:
		HGE* mHGE; 
		float32 mCameraX;
		float32 mCameraY;
		float32 mCameraScale;

		friend class Texture;
		friend bool HgeExitFunction(void);

		std::set<IScreenResolutionChangeListener*> mResChangeListeners;

		virtual void ClearResolutionChangeListeners(void);

	};
}

#endif
