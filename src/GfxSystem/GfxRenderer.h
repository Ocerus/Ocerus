#ifndef _GFXRENDERER_H_
#define _GFXRENDERER_H_

#include "Settings.h"
#include "Singleton.h"
#include "Texture.h"
#include "GfxStructures.h"

#define gGfxRenderer GfxSystem::GfxRenderer::GetSingleton()

/// @name Forward declarations
//@{
class HGE;
//@}

namespace GfxSystem
{

	/// @name Forward declarations.
	//@{
	class IScreenListener;
	//@}

	enum eAnchor { ANCHOR_VCENTER=1<<0, ANCHOR_TOP=1<<1, ANCHOR_BOTTOM=1<<2, ANCHOR_LEFT=1<<3, ANCHOR_RIGHT=1<<4, ANCHOR_HCENTER=1<<5 };

	class GfxRenderer : public Singleton<GfxRenderer>
	{
	public:
		GfxRenderer(const Point& resolution, bool fullscreen);
		~GfxRenderer(void);

		inline void ChangeResolution(const Point& resolution) { ChangeResolution(resolution.x, resolution.y); }
		void ChangeResolution(const uint32 width, const uint32 height);
		Point GetResolution(void) const;
		inline int32 GetScreenWidth(void) const { return mScreenWidth; }
		inline int32 GetScreenHeight(void) const { return mScreenHeight; }
		inline int32 GetScreenWidthHalf(void) const { return GetScreenWidth() >> 1; }
		inline int32 GetScreenHeightHalf(void) const { return GetScreenHeight() >> 1; }
		void SetFullscreen(bool fullscreen);
		bool IsFullscreen(void) const;
		bool GetWindowPosition(Point& out);

		inline virtual void AddScreenListener(IScreenListener * listener)
		{
			mScreenListeners.insert(listener);
		}

		inline virtual void RemoveResolutionChangeListener(IScreenListener * listener)
		{
			mScreenListeners.erase(listener);
		}

 		bool BeginRendering(void) const;
		bool EndRendering(void) const;

		inline void SetCameraX(const float32 x) { mCameraX = x; }
		inline void SetCameraY(const float32 y) { mCameraY = y; }
		inline void SetCameraPos(const Vector2& pos) { mCameraX = pos.x; mCameraY = pos.y; }
		inline Vector2 GetCameraPos(void) { return Vector2(mCameraX, mCameraY); }
		inline void SetCameraScale(const float32 s) { mCameraScale = s; mCameraScaleInv = 1.0f/s; }
		inline float32 GetCameraScale(void) const { return mCameraScale; }
		void ZoomCamera(const float32 delta);
		void MoveCamera(const float32 dx, const float32 dy);
		int32 WorldToScreenX(const float32 x) const;
		int32 WorldToScreenY(const float32 y) const;
		inline float32 WorldToScreenScale(const float32 scale) const { return mCameraScale*scale; }
		inline int32 WorldToScreenScalar(const float32 scalar) const { return MathUtils::Round(mCameraScale*scalar); }
		inline float32 WorldToScreenImageScale(const float32 scale) const;
		Point WorldToScreen(const Vector2& pos) const;
		float32 ScreenToWorldX(const int32 x) const;
		float32 ScreenToWorldY(const int32 y) const;
		Vector2 ScreenToWorld(const Point& pos) const;
		Vector2 GetCameraWorldBoxTopLeft(void) const;
		Vector2 GetCameraWorldBoxBotRight(void) const;

		bool ClearScreen(const Color& color) const;

		// note that anchor determines the rotation/scaling pivot
		bool DrawImage(const TexturePtr& image, int32 x, int32 y, uint8 anchor = 0, float32 angle = 0.0f, const Color& color = Color::FullColor, float32 scale = 1.0f) const;
		bool DrawImage(const TexturePtr& image, const Point& pos, uint8 anchor = 0, float32 angle = 0.0f, const Color& color = Color::FullColor, float32 scale = 1.0f) const;
		/// @name This version does a conversion from world space
		bool DrawImageWithConversion(const TexturePtr& image, const Vector2& pos, uint8 anchor = 0, float32 angle = 0.0f, const Color& color = Color::FullColor, float32 scale = 1.0f) const;
		/// @name This version is made specially to support GUI
		bool DrawImage(const TexturePtr& image, const Rect& textureRect, const Rect& destRect, const ColorRect& colors = ColorRect() ) const;
		bool DrawImage(const TexturePtr& image, const Rect& destRect) const;

		bool DrawLine(int32 x1, int32 y1, int32 x2, int32 y2, const Pen& pen) const;
		bool DrawLine(const Point& begin, const Point& end, const Pen& pen) const;
		/// @name This version does a conversion from world space
		bool DrawLineWithConversion(const Vector2& begin, const Vector2& end, const Pen& pen) const;

		bool DrawPolygon(Point* vertices, int vertices_len, const TexturePtr& image, const Pen& outline, float32 angle = 0.0f, const Color& color = Color::FullColor, float32 scale = 1.0f, float32 textureAngle = 0.0f, float32 textureScale = 1.0f) const;
		bool DrawPolygon(const vector<Point>& vertices, const TexturePtr& image, const Pen& outline, float32 angle = 0.0f, const Color& color = Color::FullColor, float32 scale = 1.0f, float32 textureAngle = 0.0f, float32 textureScale = 1.0f) const;
		
		bool DrawPolygon(Point* vertices, int vertices_len, const Color& fillColor, const Pen& outline = Pen::NullPen) const;
		bool DrawPolygon(const vector<Point>& vertices, const Color& fillColor, const Pen& outline = Pen::NullPen) const;

		bool DrawQuad(Point* const vertices, const TexturePtr texture, Vector2* const textureCoords, Color* const vertexColors);

		/// @name This version does a conversion from world space
		//TODO dodelat podobne verze DrawPolygon
		bool DrawPolygonWithConversion(const Vector2* vertices, int vertices_len, const Vector2& offsetPosition, const float32 offsetAngle, const Color& fillColor, const Pen& outline = Pen::NullPen) const;

		bool DrawCircle(const Point& center, const int32 radius, const Color& fillColor, const Pen& outline = Pen::NullPen, const float32 minAngle = 0.0f, const float32 maxAngle = MathUtils::TWO_PI) const;


		/// @name Text drawing methods.
		/// Note that coords are in percentage values relative to the screen bounds.
		//@{
		void DrawString( float32 x, float32 y, const string & id,
							 const string & text, const Color color = Color(255,255,255),
							 uint8 text_anchor =  ANCHOR_LEFT | ANCHOR_TOP,
							 uint8 screen_anchor = ANCHOR_LEFT | ANCHOR_TOP,
							 const string & fontid = "");
		Vector2 GetTextSize( const string & text, const string & fontid = "" );
		//@}

		//TODO add font param
		//bool DrawText(const string& str, font, int32 x, int32 y, uint8 anchor = ANCHOR_VCENTER|ANCHOR_HCENTER, float32 angle = 0.0f, uint8 alpha = 255);

		/// @name Few haxxor functions to workaround HGE and Windows issues.
		//@{
		uint32 _GetWindowHandle(void) const;
		//@}

	private:
		HGE* mHGE; 

		/// @name Screen bounds.
		//@{
		int32 mScreenWidth;
		int32 mScreenHeight;
		//@}

		/// @name Camera center position.
		//@{
		float32 mCameraX;
		float32 mCameraY;
		//@}

		/// @name Camera zoom ratio.
		//@{
		float32 mCameraScale;
		float32 mCameraScaleInv;
		//@}

		friend class Texture;
		friend bool HgeExitFunction(void);

		set<IScreenListener*> mScreenListeners;

	};
}

#endif
