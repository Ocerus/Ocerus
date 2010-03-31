/// @file
/// Defines various data structures used by (not only) renderer.

#ifndef GfxStructures_h__
#define GfxStructures_h__

#include "Base.h"

namespace GfxSystem
{
	/// Platform dependent handle to an arbitrary texture.
	typedef unsigned int TextureHandle;

	/// Invalid texture handle.
	const TextureHandle InvalidTextureHandle = 0;

	/// Texture pixel format
	enum ePixelFormat
	{
		PF_AUTO = 0,			//auto
		PF_L = 1,				//grayscale
		PF_LA = 2,				//grayscale with alpha
		PF_RGB = 3,				//RGB
		PF_RGBA = 4				//RGB with alpha
	};

	/// Integer point in the screen or any pixel oriented buffer.
	struct Point
	{
		Point(int32 _x, int32 _y): x(_x), y(_y) {}
		Point(void): x(0), y(0) {}

		int32 x, y;
	};

	/// Integer rectangle in the screen or any pixel oriented buffer.
	struct Rect
	{
		Rect(int32 _x, int32 _y, int32 _w, int32 _h): x(_x), y(_y), w(_w), h(_h) {}
		Rect(void): x(0), y(y), w(w), h(h) {}

		int32 x, y, w, h;

		static Rect Null;
	};

	/// Platform independent RGBA color.
	struct Color
	{
		Color(uint8 _r, uint8 _g, uint8 _b, uint8 _a = 255): r(_r), g(_g), b(_b), a(_a) {}
		Color(uint32 color) : r((uint8)( (color >> 16)&0x000000FF )), g((uint8)( (color >> 8)&0x000000FF )),
			b((uint8)( color&0x000000FF )), a((uint8)(color >> 24)) {}
		Color(void): r(0), g(0), b(0), a(255) {}
		
		inline uint32 GetARGB() const
		{
			return a << 24 | r << 16 | g << 8 | b; 
		}

		inline bool operator==(const Color& rhs)
		{
			return r==rhs.r && g==rhs.g && b==rhs.b && a==rhs.a;
		}

		uint8 r, g, b, a;

		static Color Null;
		static Color Full;
	};

	/// Quad with a texture attached to it.
	struct TexturedQuad
	{
		TexturedQuad(): position(Vector2_Zero), size(Vector2_Zero), scale(1.0f, 1.0f), angle(0), z(0), texture(0), transparency(0) {}

		TexturedQuad(const Vector2& _position, const Vector2& _size, const Vector2& _scale, const float32 _angle,
			const float32 _z, const uint32 _texture, const float32 _transparency ):
			position(_position), size(_size), scale(_scale), angle(_angle), z(_z),
			texture(_texture), transparency(_transparency) {}

		Vector2 position;
		Vector2 size;
		Vector2 scale;
		float32 angle;
		float32 z;
		TextureHandle texture;
		float32 transparency;
	};

	/// Grid attributes. Used in viewports.
	struct GridInfo
	{
		GridInfo(): cellSize(100), color(Color::Full) {}
		float32 cellSize;
		Color color;
	};
}

#endif // GfxStructures_h__