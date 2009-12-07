#ifndef GfxStructures_h__
#define GfxStructures_h__

#include "Base.h"

namespace GfxSystem
{
	struct Point
	{
		Point(int32 _x, int32 _y): x(_x), y(_y) {}
		Point(void): x(0), y(0) {}
		int32 x, y;
	};

	struct Rect
	{
		Rect(int32 _x, int32 _y, int32 _w, int32 _h): x(_x), y(_y), w(_w), h(_h) {}
		int32 x, y, w, h;

		Rect(void): x(0), y(y), w(w), h(h) {}
		static Rect NullRect;
	};

	struct Color
	{
		Color(uint8 _r, uint8 _g, uint8 _b, uint8 _a = 255): r(_r), g(_g), b(_b), a(_a) {}
		Color(uint32 color) : r((uint8)( (color >> 16)&0x000000FF )), g((uint8)( (color >> 8)&0x000000FF )),
			b((uint8)( color&0x000000FF )), a((uint8)(color >> 24)) {}
		Color(void): r(0), g(0), b(0), a(255) {}
		inline uint32 GetARGB() const { return a << 24 | r << 16 | g << 8 | b; }

		bool operator==(const Color& rhs)
		{
			return r==rhs.r && g==rhs.g && b==rhs.b && a==rhs.a;
		}

		uint8 r, g, b, a;

		static Color NullColor;
		static Color FullColor;
	};

	struct ColorRect
	{
		Color TopLeft, TopRight, BottomLeft, BottomRight;
		ColorRect(const Color& TopLeft, const Color& TopRight, const Color& BottomLeft, const Color& BottomRight):
		TopLeft(TopLeft), TopRight(TopRight), BottomLeft(BottomLeft), BottomRight(BottomRight)
		{

		}

		ColorRect(void):
		TopLeft(Color::FullColor),
			TopRight(Color::FullColor),
			BottomLeft(Color::FullColor),
			BottomRight(Color::FullColor)
		{

		}
	};

	struct Pen
	{
		Pen(const Color& c): color(c) {}
		Color color;

		static Pen NullPen;
	};

	struct Sprite
	{
		Vector2 position, size;
		float32 z;
		uint32 texture;
		
		Sprite(const Vector2& _position, const Vector2& _size, const float32 _z, const uint32 _texture): 
			position(_position), size(_size), z(_z), texture(_texture) {}
	};
}

#endif // GfxStructures_h__